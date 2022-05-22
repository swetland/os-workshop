// Copyright 2022, Brian Swetland <swetland@frotz.net>
// Licensed under the Apache License, Version 2.0

#include <hw/riscv.h>
#include <hw/context.h>
#include <hw/debug.h>
#include <hw/intrinsics.h>

#include <hw/platform.h>
#include <hw/litex.h>

#include <net/ipv6.h>

#include <string.h>

#define eth_rd(a) io_rd32(ETHMAC_BASE + LX_ETHMAC_ ## a)
#define eth_wr(a,v) io_wr32(ETHMAC_BASE + LX_ETHMAC_ ## a, v)

void interrupt_handler(void) { xputc('@'); }

void pkt_rx_udp(void *data, unsigned len, unsigned port, int mcast) {
	xprintf("UDP %u: %p(%u) %s\n", port, data, len, mcast ? "M" : "");
	net_tx_udp_reply(data, "multipass", 10, port);
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

void start(void) {
	xprintf("Example 05 - Ethernet\n\n");

	// set trap vector to trap_entry() in trap-entry-single.S
	// it will call exception_handler() or interrupt_handler()
	csr_write(CSR_STVEC, (uintptr_t) trap_entry);

	// enable timer0 irq
	//csr_set(CSR_S_INTC_ENABLE, TIMER0_IRQb);

	// enable external interrupts
	//csr_set(CSR_SIE, INTb_SVC_EXTERN);

	// enable interrupts 
	//irq_enable();

	uint8_t mac[6] = { 0x42,0x42,0x10,0x20,0x30,0x40 };
	net_init(mac);
	eth_init();

	while (1) {
		if (eth_rd(WR_EV_PENDING) & 1) {
			uint32_t slot = eth_rd(WR_SLOT);
			uint8_t *rxb = (void*) (ETHMAC_SRAM_BASE + ETHMAC_SLOT_SIZE * slot);
			eth_rx(rxb, eth_rd(WR_LEN));
			eth_wr(WR_EV_PENDING, 1);
		}
	}
}

// if an exception occurs, dump register state and halt
void exception_handler(eframe_t *ef) {
	xprintf("\n** SUPERVISOR EXCEPTION **\n");
	xprint_s_exception(ef);
	xprintf("\nHALT\n");
	for (;;) ;
}

