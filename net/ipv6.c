// Copyright 2022, Brian Swetland <swetland@frotz.net>
// Licensed under the Apache License, Version 2.0.

#include <string.h>
#include <net/ipv6.h>
#include <hw/debug.h>

void hexdump(void *data, int len) {
	unsigned off = 0;
	while (len > 0) {
		xprintf("%04x:", off);
		for (int n = 0; n < 16; n++) {
			if (n == len) break;
			xprintf(" %02x", ((uint8_t*)data)[n]);
			if (n == 7) xputc(' ');
		}
		xputc('\n');
		len -= 16;
		data += 16;
		off += 16;
	}
}

#define NETBUF_MAX 1536  // 1.5KB

#define TRACE_DISCARD 1

typedef struct netbuf {
	struct netbuf* next;
	uint32_t rsvd0;
	uint32_t rsvd1;
	uint32_t rsvd2;
	uint8_t data[NETBUF_MAX];
} netbuf_t;

// fast mac matching via word comparison
typedef union {
	uint32_t w[2];
	uint8_t b[8];
} match_t;

match_t m_dev_mac = { .b = { 0, 0, 0x72, 0x72, 0xaa, 0xbb, 0xcc, 0xdd } };
match_t m_mcast_all = { .b = { 0, 0, 0x33, 0x33, 0x00, 0x00, 0x00, 0x01 } };
match_t m_mcast_ns = { .b = { 0, 0, 0x33, 0x33, 0xff, 0xbb, 0xcc, 0xdd } };

uint8_t device_mac[ETH_ADDR_LEN] = { 0x72, 0x72, 0xaa, 0xbb, 0xcc, 0xdd };

ip6_addr_t device_ip6 = { .b = {
	0xFE, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x70, 0x72, 0xaa, 0xff, 0xfe, 0xbb, 0xcc, 0xdd, } };

ip6_addr_t mcast_all_ip6 = { .b = {
	0xFF, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, } };

ip6_addr_t mcast_ns_ip6 = { .b = {
	0xFF, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x01, 0xff, 0xbb, 0xcc, 0xdd } };

// returns 16bit checksum of provided byte data buffer
static uint32_t checksum(const void* _data, unsigned len, uint32_t sum) {
	const uint16_t* data = _data;
	while (len > 1) {
		sum += *data++;
		len -= 2;
	}
	if (len) {
		sum += (*data & 0xFF);
	}
	while (sum > 0xFFFF) {
		sum = (sum & 0xFFFF) + (sum >> 16);
	}
	return sum;
}

// Takes pointer to ip6 packet starting with ip6 header (filled out)
// and length of payload (exclusive of the ip6 header length)
//
// Returns 16bit ip6 checksum
static uint32_t checksum_ip6(ip6_hdr_t* hdr, unsigned len) {
	// sum of length and protocol fields from pseudo-header
	uint32_t sum = checksum(&hdr->length, 2, htons(hdr->next_header));
	// sum of src and dst fields from pseudo-header + payload
	sum = checksum(&hdr->src, len + 32, sum);
	// avoid return sum 0
	return (sum == 0xffff) ? sum : ~sum;
}

void net_rx_udp6(void *data, unsigned dlen, int mcast) {
	if (dlen < sizeof(udp_hdr_t)) {
		xprintf("UPD6: runt\n");
		return;
	}
	udp_hdr_t *udp = data;
	uint16_t port = ntohs(udp->dst_port);
	uint16_t len = ntohs(udp->length);
	if ((len < sizeof(udp_hdr_t)) || (len > dlen)) {
		xprintf("UDP6: badlen %u (dlen %u)\n", len, dlen);
		// bogus length or short packet
		return;
	}
	pkt_rx_udp(data + sizeof(udp_hdr_t), len - sizeof(udp_hdr_t), port, mcast);
}

void net_rx_icmp6(void *data, unsigned dlen, int mcast) {
	if (dlen < sizeof(icmp_hdr_t)) {
		xprintf("ICMP6 runt\n");
		return;
	}
	icmp_hdr_t *icmp = data;

	switch (icmp->type) {
	case ICMP_NDP_N_SOLICIT: {
		ndp_hdr_t *ndp = data;
		if ((dlen < sizeof(ndp_hdr_t)) || (ndp->code != 0)) {
			return;
		}
		if (memcmp(&ndp->target, &device_ip6, sizeof(ip6_addr_t))) {
			return; // not for us
		}

		struct {
			ndp_hdr_t hdr;
			uint8_t opt[8];
		} reply = {
			.hdr.type = ICMP_NDP_N_ADVERTISE,
			.hdr.code = 0,
			.hdr.checksum = 0,
			.hdr.flags = 0x60, // (S)olicited and (O)verride
			.opt[0] = NDP_N_TGT_LL_ADDR,
			.opt[1] = 1,
		};
		memcpy(&reply.hdr.target, &device_ip6, sizeof(ip6_addr_t));
		memcpy(&reply.opt[2], device_mac, ETH_ADDR_LEN);

		net_tx_ip6_reply(data, &reply, sizeof(reply), IP_HDR_ICMP, offsetof(ndp_hdr_t, checksum));
		return;
	}
	case ICMP_ECHO_REQUEST: {
		icmp_hdr_t *icmp = data;
		if (dlen < sizeof(icmp_hdr_t)) {
			return;
		}
		icmp->type = ICMP_ECHO_REPLY;
		icmp->checksum = 0;
		net_tx_ip6_reply(data, icmp, dlen, IP_HDR_ICMP, offsetof(icmp_hdr_t, checksum));
		return;
	}
	default:
		xprintf("ICMP ? %u %u %s\n", icmp->type, icmp->code, mcast ? "M" : "");
	}
}

// len >= 44
void net_rx_ip6(void *data, unsigned len, int mcast) {
	ip6_hdr_t *ip6 = data;

	if (mcast) {
		if (memcmp(&ip6->dst, &mcast_all_ip6, sizeof(ip6_addr_t)) &&
			memcmp(&ip6->dst, &mcast_ns_ip6, sizeof(ip6_addr_t))) {
			xprintf("IP6: MA ?\n");
			return;
		}
	} else {
		if (memcmp(&ip6->dst, &device_ip6, sizeof(ip6_addr_t))) {
			xprintf("IP6: UA ?\n");
			return;
		}
	}

	switch (ip6->next_header) {
	case IP_HDR_ICMP:
		net_rx_icmp6(data + sizeof(ip6_hdr_t), len - sizeof(ip6_hdr_t), mcast);
		return;
	case IP_HDR_UDP:
		net_rx_udp6(data + sizeof(ip6_hdr_t), len - sizeof(ip6_hdr_t), mcast);
		return;
	default:
		xprintf("IP6: HDR %u ?\n", ip6->next_header);
		return;
	}
}

void net_rx_eth(void *data, unsigned len) {
	if (len < 60) {
		xprintf("ETH runt\n");
		return; // runt packet
	}

	// discard non-ipv6 packets
	eth_hdr_t *eth = data;
	if (eth->type != htons(ETH_TYPE_IP6)) {
		return;
	}

	//hexdump(data + 2, len - 2);

	// accept packets which match our ethernet MAC or multicast address
	uint32_t *w = data;
	if ((w[0] == m_dev_mac.w[0]) && (w[1] == m_dev_mac.w[1])) {
		net_rx_ip6(data + sizeof(eth_hdr_t), len - sizeof(eth_hdr_t), 0);	
	} else if ((w[0] == m_mcast_ns.w[0]) && (w[1] == m_mcast_ns.w[1])) {
		net_rx_ip6(data + sizeof(eth_hdr_t), len - sizeof(eth_hdr_t), 1);
	} else if ((w[0] == m_mcast_all.w[0]) && (w[1] == m_mcast_all.w[1])) {
		net_rx_ip6(data + sizeof(eth_hdr_t), len - sizeof(eth_hdr_t), 1);
	} else {
		xprintf("ETH ? %02x %02x %02x %02x %02x %02x\n",
			eth->dst[0], eth->dst[1], eth->dst[2],
			eth->dst[3], eth->dst[4], eth->dst[5]);
	}

}

typedef struct {
	eth_hdr_t eth;
	ip6_hdr_t ip6;
	udp_hdr_t udp;
	uint8_t data[];
} udp6_pkt_t;

static_assert(sizeof(udp6_pkt_t) == (sizeof(eth_hdr_t) + sizeof(ip6_hdr_t) + sizeof(udp_hdr_t)), "");
#define MAX_UDP_PAYLOAD (NETBUF_MAX - sizeof(udp6_pkt_t))

void net_tx_udp_reply(void *replyto, void *data, unsigned len, unsigned port) {
	uint8_t packet[NETBUF_MAX];
	udp6_pkt_t *tx = (void*) packet;
	udp6_pkt_t *rx = replyto - sizeof(udp6_pkt_t); 

	if (len > MAX_UDP_PAYLOAD) {
		return;
	}

	eth_addr_copy(tx->eth.dst, rx->eth.src);
	eth_addr_copy(tx->eth.src, device_mac);
	tx->eth.type = htons(ETH_TYPE_IP6);

	tx->ip6.bits = 0x00000060; // ver=0, tc=0, flow=0
	tx->ip6.length = htons(UDP_HDR_LEN + len);
	tx->ip6.next_header = IP_HDR_UDP;
	tx->ip6.hop_limit = 255;
	ip6_addr_copy(&tx->ip6.src, &device_ip6);
	ip6_addr_copy(&tx->ip6.dst, &rx->ip6.src);

	tx->udp.src_port = htons(port);
	tx->udp.dst_port = rx->udp.src_port;
	tx->udp.length = htons(len + UDP_HDR_LEN);
	tx->udp.checksum = 0;

	memcpy(tx->data, data, len); 

	tx->udp.checksum = checksum_ip6(&tx->ip6, len + UDP_HDR_LEN);

	eth_tx(packet + 2, ETH_HDR_LEN + IP6_HDR_LEN + UDP_HDR_LEN + len);
}

typedef struct {
	eth_hdr_t eth;
	ip6_hdr_t ip6;
	uint8_t data[];
} ip6_pkt_t;

static_assert(sizeof(ip6_pkt_t) == (sizeof(eth_hdr_t) + sizeof(ip6_hdr_t)), "");
#define MAX_IP6_PAYLOAD (NETBUF_MAX - sizeof(ip6_pkt_t))

void net_tx_ip6_reply(void *replyto, void *data, unsigned len,
		unsigned type, unsigned chk_off) {
	uint8_t packet[NETBUF_MAX];
	ip6_pkt_t *tx = (void*) packet;
	ip6_pkt_t *rx = replyto - sizeof(ip6_pkt_t); 

	if (len > MAX_IP6_PAYLOAD) {
		return;
	}

	eth_addr_copy(tx->eth.dst, rx->eth.src);
	eth_addr_copy(tx->eth.src, device_mac);
	tx->eth.type = htons(ETH_TYPE_IP6);

	tx->ip6.bits = 0x00000060; // ver=0, tc=0, flow=0
	tx->ip6.length = htons(len);
	tx->ip6.next_header = type;
	tx->ip6.hop_limit = 255;
	ip6_addr_copy(&tx->ip6.src, &device_ip6);
	ip6_addr_copy(&tx->ip6.dst, &rx->ip6.src);

	memcpy(tx->data, data, len); 

	*((uint16_t*) (tx->data + chk_off)) = checksum_ip6(&tx->ip6, len);

	//hexdump(packet + 2, ETH_HDR_LEN + IP6_HDR_LEN + len);

	eth_tx(packet + 2, ETH_HDR_LEN + IP6_HDR_LEN + len);
}

