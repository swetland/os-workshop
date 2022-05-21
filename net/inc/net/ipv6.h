// Copyright 2022, Brian Swetland <swetland@frotz.net>
// Licensed under the Apache License, Version 2.0.

#pragma once

#include <stdint.h>
#include <assert.h>

#define ETH_ADDR_LEN 6
#define IP6_ADDR_LEN 16

#define ETH_HDR_LEN 14
#define IP6_HDR_LEN 40
#define UDP_HDR_LEN 8
#define ICMP_HDR_LEN 4
#define NDP_HDR_LEN 24

#define IP6_MTU 1280

#define ETH_TYPE_IP4 0x0800
#define ETH_TYPE_ARP 0x0806
#define ETH_TYPE_IP6 0x86DD

#define IP_HDR_TCP 6
#define IP_HDR_UDP 17
#define IP_HDR_ICMP 58

#define ICMP_ECHO_REQUEST 128
#define ICMP_ECHO_REPLY 129
#define ICMP_NDP_N_SOLICIT 135
#define ICMP_NDP_N_ADVERTISE 136

#define NDP_N_SRC_LL_ADDR 1
#define NDP_N_TGT_LL_ADDR 2
#define NDP_N_PREFIX_INFO 3
#define NDP_N_REDIRECTED_HDR 4
#define NDP_N_MTU 5

typedef union {
	uint8_t b[IP6_ADDR_LEN];
	uint32_t w[IP6_ADDR_LEN / 4];
} ip6_addr_t;

typedef struct {
	uint8_t pad[2];
	uint8_t dst[ETH_ADDR_LEN];
	uint8_t src[ETH_ADDR_LEN];
	uint16_t type;
} eth_hdr_t;

typedef struct {
	uint32_t bits;
	uint16_t length;
	uint8_t next_header;
	uint8_t hop_limit;
	ip6_addr_t src;
	ip6_addr_t dst;
} ip6_hdr_t;

typedef struct {
	uint16_t src_port;
	uint16_t dst_port;
	uint16_t length;
	uint16_t checksum;
} udp_hdr_t;

typedef struct {
	uint8_t type;
	uint8_t code;
	uint16_t checksum;
} icmp_hdr_t;

typedef struct {
	uint8_t type;
	uint8_t code;
	uint16_t checksum;
	uint32_t flags;
	ip6_addr_t target;
	uint8_t options[];
} ndp_hdr_t;

// ensure structures are right size
static_assert(sizeof(ip6_addr_t) == IP6_ADDR_LEN, "");
static_assert(sizeof(eth_hdr_t) == (ETH_HDR_LEN + 2), "");
static_assert(sizeof(ip6_hdr_t) == IP6_HDR_LEN, "");
static_assert(sizeof(udp_hdr_t) == UDP_HDR_LEN, "");
static_assert(sizeof(icmp_hdr_t) == ICMP_HDR_LEN, "");
static_assert(sizeof(ndp_hdr_t) == NDP_HDR_LEN, "");


#define ntohs(val) __builtin_bswap16(val)
#define ntohl(val) __builtin_bswap32(val)

#define htons(val) __builtin_bswap16(val)
#define htonl(val) __builtin_bswap32(val)


static inline void ip6_addr_copy(ip6_addr_t *dst, ip6_addr_t *src) {
	dst->w[0] = src->w[0];
	dst->w[1] = src->w[1];
	dst->w[2] = src->w[2];
	dst->w[3] = src->w[3];
}

static inline void eth_addr_copy(uint8_t *_dst, uint8_t *_src) {
	uint16_t *dst = (void*) _dst;
	uint16_t *src = (void*) _src;
	dst[0] = src[0];
	dst[1] = src[1];
	dst[2] = src[2];
}

// push rx packets into the network stack
void net_rx_eth(void *data, unsigned len);

// send a reply from pkt_rx_udp()
// replyto must be the data buffer passed in to pkt_rx_udp()
void net_tx_udp_reply(void *replyto, void *data, unsigned len, unsigned port);

void net_tx_ip6_reply(void *replyto, void *data, unsigned len,
			unsigned type, unsigned chk_off);

// callback on udp packet receipt
void pkt_rx_udp(void *data, unsigned len, unsigned port, int mcast);

// callback to transmit a packet
void eth_tx(void *data, unsigned len);

