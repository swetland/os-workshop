// Copyright 2022, Brian Swetland <swetland@frotz.net>
// Licensed under the Apache License, Version 2.0

#include <string.h>
#include <stdio.h>

#include <hw/riscv.h>
#include <hw/context.h>
#include <hw/intrinsics.h>
#include <hw/debug.h>

#include <hw/platform.h>
#include <hw/litex.h>

#include <net/ipv6.h>
#include <netboot.h>

#include "boot.h"

// we expect the supervisor program to be in memory after the end of the bootloader
#define SVC_ENTRY (DRAM_BASE + BOOTLOADER_SIZE)

#define uart_rd(a) io_rd32(UART0_BASE + LX_UART_ ## a)
#define uart_wr(a,v) io_wr32(UART0_BASE + LX_UART_ ## a, v)
#define eth_rd(a) io_rd32(ETHMAC_BASE + LX_ETHMAC_ ## a)
#define eth_wr(a,v) io_wr32(ETHMAC_BASE + LX_ETHMAC_ ## a, v)

void halt_peripherals(void) {
	eth_wr(WR_EV_ENABLE, 0);
	eth_wr(RD_EV_ENABLE, 0);
}

const char *nodename = "device";

void query_reply(void *replyto) {
	netboot_msg_t *msg = replyto;
	unsigned len = snprintf((void*)msg->db, NB_DATA_MAX, "name=%s", nodename);
	msg->cmd = NB_CMD_STATUS;
	msg->arg = NB_OK;
	net_tx_udp_reply(replyto, msg, NB_MSG_MIN + len + 1, NB_PORT_CTRL);
}

void pkt_rx_udp(void *data, unsigned len, unsigned port, int mcast) {
	netboot_msg_t* msg = data;
	if ((len < NB_MSG_MIN) || (len > NB_MSG_MAX) || (msg->magic != NB_MAGIC)) {
		return;
	}

	if (port == NB_PORT_QUERY) {
		unsigned nlen = strlen(nodename);
		// query port only replies to valid queries that match
		if ((msg->cmd != NB_CMD_QUERY) ||
			(len != (nlen + NB_MSG_MIN)) ||
			(memcmp(msg->db, nodename, nlen))) {
			return;
		}
		query_reply(data);
		return;
	}
	if (port != NB_PORT_CTRL) {
		return;
	}

	switch (msg->cmd) {
	case NB_CMD_QUERY:
		xputc('Q');
		query_reply(data);
		return;
	case NB_CMD_WRITE:
		xputc('.');
		// TODO: apply range limits?
		memcpy((void*) msg->arg, msg->db, len - NB_MSG_MIN);
		msg->cmd = NB_CMD_STATUS;
		msg->arg = NB_OK;
		net_tx_udp_reply(data, msg, NB_MSG_MIN, NB_PORT_CTRL);
		return;
	case NB_CMD_EXEC: {
		xputc('!');
		uint32_t entry = msg->arg;
		msg->cmd = NB_CMD_STATUS;
		msg->arg = NB_OK;
		net_tx_udp_reply(data, msg, NB_MSG_MIN, NB_PORT_CTRL);

		csr_write(CSR_MSTATUS, PRIV_S << MSTATUS_MPP_SHIFT);
		xprintf("\n\n[ execution starts at 0x%08x ]\n\n", entry);
		exit_mode_m(0, 0, entry, 0);
		return;
	}
	default:
		xputc('E');
		msg->cmd = NB_CMD_STATUS;
		msg->arg = NB_ERR_BADCMD;
		net_tx_udp_reply(data, msg, NB_MSG_MIN, NB_PORT_CTRL);
		break;
	}
}

uint8_t rxbuf[1536] = { 0, };

void eth_rx(uint8_t *rxb, unsigned rxlen) {
	if (rxlen > 1534) return;
	memcpy(rxbuf + 2, rxb, rxlen);
	net_rx_eth(rxbuf, rxlen + 2);
}


static unsigned txslot;

void eth_tx(void *txb, unsigned txlen) {
	if (txlen > 1534) return;

	while (eth_rd(RD_READY) != 1) ;

	memcpy((void*)(ETHMAC_SRAM_BASE + ETHMAC_SLOT_SIZE * (ETHMAC_RX_SLOTS + txslot)), txb, txlen);

	eth_wr(RD_SLOT, txslot);
	eth_wr(RD_LEN, txlen);
	eth_wr(RD_START, 1);

	txslot ^= 1;
}

void eth_init(void) {
	eth_wr(WR_EV_ENABLE, 0);
	eth_wr(RD_EV_ENABLE, 0);

	eth_wr(WR_EV_PENDING, 1);
	eth_wr(RD_EV_PENDING, 1);

	txslot = 0;
	eth_wr(RD_SLOT, txslot);

	eth_wr(WR_EV_ENABLE, 1);
	eth_wr(RD_EV_ENABLE, 1);
}

void eth_handle_rx(void) {
	if (eth_rd(WR_EV_PENDING) & 1) {
		uint32_t slot = eth_rd(WR_SLOT);
		uint8_t *rxb = (void*) (ETHMAC_SRAM_BASE + ETHMAC_SLOT_SIZE * slot);
		eth_rx(rxb, eth_rd(WR_LEN));
		eth_wr(WR_EV_PENDING, 1);
	}
}

void netboot(void) {
	uint8_t mac[6] = { 0x42,0x42,0x10,0x20,0x30,0x40 };
	net_init(mac);
	eth_init();
	for (;;) {
		eth_handle_rx();
	}
}

void mach_exception_handler(eframe_t *ef) {
	xprintf("\n** MACHINE EXCEPTION **\n");
	xprint_m_exception(ef);
	xprintf("\nHALT\n");
	for (;;) ;
}

// interrupts and exceptions to delegate to supervisor mode
#define INT_LIST (INTb_SVC_SW|INTb_SVC_TIMER|INTb_SVC_EXTERN)
//#define EXC_LIST (EXCb_ECALL_UMODE)
#define EXC_LIST (0xFFFF)

void start(uint32_t hartid, uint32_t fdt) {
	xprintf("\n** Frobozz Magic (Network) Bootloader v0.2 **\n\n");

	int qemu = (csr_read(CSR_MVENDORID) == 0);

	// set mach exception vector and stack pointer
	csr_write(CSR_MTVEC, ((uintptr_t) mach_exception_entry) );

	// use the free ram below the supervisor entry as our exception stack
	csr_write(CSR_MSCRATCH, SVC_ENTRY);

	// QEMU currently emulates memory protection, which we must appease
	if (qemu) {
		// U/S allow access to all memory
		csr_write(CSR_PMPCFG(0), PMP_CFG_A_TOR | PMP_CFG_X | PMP_CFG_W | PMP_CFG_R);
		csr_write(CSR_PMPADDR(0), 0xFFFFFFFF);
	}

	// delegate interrupts and exceptions
	csr_set(CSR_MIDELEG, INT_LIST);
	csr_set(CSR_MEDELEG, EXC_LIST);

	netboot();
}

