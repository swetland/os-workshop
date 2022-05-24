// Copyright 2022, Brian Swetland <swetland@frotz.net>
// Licensed under the Apache License, Version 2.0.

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <poll.h>
#include <time.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <net/if.h>
#include <fcntl.h>

#include "netboot.h"

uint64_t get_time_ms(void) {
	struct timespec ts;
	if (clock_gettime(CLOCK_MONOTONIC, &ts)) {
		exit(-1);
	}
	return (((uint64_t)ts.tv_nsec) / 1000000ULL) + (((uint64_t)ts.tv_sec) * 1000ULL);
}

int open_udp6_socket(struct sockaddr_in6 *addr, int con) {
	int s, n = 1;
	if ((s = socket(AF_INET6, SOCK_DGRAM, IPPROTO_UDP)) < 0) {
		perror("open_udp6_socket: open socket");
		return -1;
	}
	if (setsockopt(s, SOL_SOCKET, SO_REUSEPORT, &n, sizeof(n)) < 0) {
		perror("open_udp6_socket: SO_REUSEPORT");
		goto fail;
	}
	if (setsockopt(s, SOL_SOCKET, SO_REUSEADDR, &n, sizeof(n)) < 0) {
		perror("open_udp6_socket: SO_REUSEADDR");
		goto fail;
	}
	if (fcntl(s, F_SETFL, O_NONBLOCK) < 0) {
		perror("open_ud6_socket: set O_NONBLOCK");
		goto fail;
	}
	if (con) {
		if (connect(s, (struct sockaddr*) addr, sizeof(struct sockaddr_in6)) < 0) {
			perror("open_udp6_socket: connect");
			goto fail;
		}
	} else {
		if (bind(s, (struct sockaddr*) addr, sizeof(struct sockaddr_in6)) < 0) {
			perror("open_udp6_socket: bind");
			goto fail;
		}
	}
	return s;
fail:
	close(s);
	return -1;
}

static uint32_t seq = 1;

int open_node(int idx, const char *nodename) {
	int s, r;

	struct sockaddr_in6 addr = {
		.sin6_family = AF_INET6,
		.sin6_scope_id = idx,
	};
	if ((s = open_udp6_socket(&addr, 0)) < 0) {
		return -1;
	}

	struct sockaddr_in6 mcast = {
		.sin6_family = AF_INET6,
		.sin6_port = htons(NB_PORT_QUERY),
		.sin6_scope_id = idx,
	};
	inet_pton(AF_INET6, "ff02::1", &mcast.sin6_addr);

	netboot_msg_t msg = {
		.magic = NB_MAGIC,
		.cmd = NB_CMD_QUERY,
		.arg = 0,
	};
	uint32_t msglen = strlen(nodename);
	if (msglen > NB_MSG_MAX) {
		return -1;
	}
	memcpy(msg.db, nodename, msglen);
	msglen += NB_MSG_MIN;

	uint64_t next = get_time_ms() + 500;
	for (;;) {
		uint64_t now = get_time_ms();
		if (now >= next) {
			next = now + 500;
			msg.seq = ++seq;
			r = sendto(s, &msg, msglen, 0, (struct sockaddr*) &mcast, sizeof(mcast));
			if (r != msglen) {
				perror("write");
				return -1;
			}
		}
		struct pollfd pfd = {
			.fd = s,
			.events = POLLIN,
		};
		if (poll(&pfd, 1, next - now) == 1) {
			uint8_t rxb[2048];
			struct sockaddr_in6 rxaddr;
			socklen_t rxalen = sizeof(rxaddr);
			if ((r = recvfrom(s, rxb, sizeof(rxb), 1, (void*)&rxaddr, &rxalen)) > 0) {
				netboot_msg_t *msg = (void*)rxb;
				if (r < NB_MSG_MIN) {
					continue;
				}
				if ((msg->magic != NB_MAGIC) ||
					(msg->cmd != NB_CMD_STATUS) ||
					(msg->seq != seq)) {
					continue;
				}
				char _tmp[128];
				const char *tmp = inet_ntop(AF_INET6, &rxaddr.sin6_addr, _tmp, sizeof(_tmp));
				fprintf(stderr, "\n[ located '%s' at %s/%u ]\n", nodename,
					tmp ? tmp : "unknown", rxaddr.sin6_scope_id);

				int s0 = open_udp6_socket(&rxaddr, 1);
				if (s0 >= 0) {
					close(s);
					return s0;
				}
			}
		}
	}
}


int send_msg(int s, netboot_msg_t *msg, unsigned len) {
	int r;
	for (int retry = 0; retry < 5; retry++) {
		msg->seq = ++seq;
		if ((r = write(s, msg, len)) != len) {
			return -1;
		}
		struct pollfd pfd = {
			.fd = s,
			.events = POLLIN,
		};
		if (poll(&pfd, 1, 250) != 1) {
			fprintf(stderr, "T");
			continue;
		}
		netboot_msg_t rsp;
		r = read(s, &rsp, sizeof(rsp));
		if ((r < NB_MSG_MIN) || (rsp.magic != NB_MAGIC) || (rsp.cmd != NB_CMD_STATUS)) {
			fprintf(stderr, "X");
			continue;
		}
		if (rsp.seq != seq) {
			fprintf(stderr, "S");
			continue;
		}
		if (rsp.arg != NB_OK) {
			fprintf(stderr, "E");
			return -1;
		}
		fprintf(stderr, ".");
		return 0;
	}
	return -1;
}

int send_file(int s, const char* fn, uint32_t addr) {
	int r;

	netboot_msg_t msg;
	msg.magic = NB_MAGIC;
	msg.cmd = NB_CMD_WRITE;

	int fd = open(fn, O_RDONLY);
	if (fd < 0) {
		fprintf(stderr, "error: cannot open '%s'\n", fn);
		return -1;
	}
	fprintf(stderr, "\n[ sending '%s' to 0x%08x ]\n", fn, addr);
	for (;;) {
		if ((r = read(fd, msg.db, NB_DATA_MAX)) <= 0) {
			close(fd);
			return r;
		}
		msg.arg = addr;
		addr += r;
		if (send_msg(s, &msg, r + NB_MSG_MIN)) {
			close(fd);
			return -1;
		}
	}
}

typedef struct {
	const char* name;
	uint32_t addr;
} image_t;

#define IMGMAX 8

#define consume() \
	do { argc--; argv++; if (argc == 1) { \
		fprintf(stderr, "error: missing argument\n"); \
		return -1; } \
	} while (0)

void usage(void) {
	fprintf(stderr, "\n"
	"usage:    netboot <option>* <image>+\n"
	"\n"
	"option:   -i <interface-name>\n"
	"          -n <node-name>\n"
	"\n"
	"image:    <filename>@<hex-load-address>\n"
	"\n"
	"Send one or more images to <node-name> on <interface-name>.\n"
	"Start execution at the address of the first image.\n\n");
}

int main(int argc, char **argv) {
	int s, idx;

	int do_exit = 0;
	const char *ifname = "qemu0";
	const char *nodename = "device";
	image_t imagelist[IMGMAX];
	int imagecount = 0;

	char *tmp;
	while (argc > 1) {
		if ((tmp = strchr(argv[1], '@'))) {
			if (imagecount == IMGMAX) {
				fprintf(stderr, "error: too many images\n");
				return -1;
			}
			*tmp++ = 0;
			imagelist[imagecount].name = argv[1];
			imagelist[imagecount].addr = strtoul(tmp, NULL, 16);
			imagecount++;
		} else if (!strcmp("-i", argv[1])) {
			consume();
			ifname = argv[1];
		} else if (!strcmp("-n", argv[1])) {
			consume();
			nodename = argv[1];
		} else if (!strcmp("-h", argv[1])) {
			usage();
			return 0;
		} else if (!strcmp("-x", argv[1])) {
			do_exit = 1;
		} else {
			fprintf(stderr, "error: unknown argument '%s'\n", argv[1]);
			return -1;
		} 
		argc--;
		argv++;
	}

	if (imagecount == 0) {
		fprintf(stderr, "error: no files to send?\n");
		return -1;
	}
	if ((idx = if_nametoindex(ifname)) == 0) {
		fprintf(stderr, "unknown network interface: '%s'\n", argv[1]);
		return -1;
	}

	fprintf(stderr, "\n[ netboot v0.1 ]\n");
	fprintf(stderr, "[ netifc '%s' ]\n", ifname);
	fprintf(stderr, "[ nodename '%s' ]\n", nodename);

	for (;;) {
again:
		if ((s = open_node(idx, "device")) < 0) {
			return -1;
		}

		for (unsigned n = 0; n < imagecount; n++) {
			if (send_file(s, imagelist[n].name, imagelist[n].addr)) {
				fprintf(stderr, "\n[ send failure ]\n");
				close(s);
				goto again;
			}
		}
		fprintf(stderr, "\n[ start execution at 0x%08x ]\n", imagelist[0].addr);

		netboot_msg_t msg;
		msg.magic = NB_MAGIC;
		msg.cmd = NB_CMD_EXEC;
		msg.seq = ++seq;
		msg.arg = imagelist[0].addr;
		send_msg(s, &msg, NB_MSG_MIN);
		fprintf(stderr, "\n");
		close(s);

		if (do_exit) break;
	}

	return 0;
}
