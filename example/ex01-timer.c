// Copyright 2022, Brian Swetland <swetland@frotz.net>
// Licensed under the Apache License, Version 2.0

#include <hw/riscv.h>
#include <hw/context.h>
#include <hw/debug.h>
#include <hw/intrinsics.h>

#include <hw/platform.h>
#include <hw/litex.h>

#define timer_rd(a) io_rd32(TIMER0_BASE + LX_TIMER_ ## a)
#define timer_wr(a,v) io_wr32(TIMER0_BASE + LX_TIMER_ ## a, v)

void timer_init(void) {
	// disable, clear pending irqs
	timer_wr(EN, 0);
	timer_wr(EV_PENDING, LX_TIMER_EVb_ZERO);

	// set for repeating every 100ms
	timer_wr(LOAD, 0);
	timer_wr(RELOAD, 50000000/10);

	// enable timer and timer wrap irq
	timer_wr(EN, 1);
	timer_wr(EV_ENABLE, LX_TIMER_EVb_ZERO);
}

volatile uint32_t ticks = 0;

void interrupt_handler(void) {
	if (timer_rd(EV_PENDING)) {
		timer_wr(EV_PENDING, LX_TIMER_EVb_ZERO);
		ticks++;
	}
}

void start(void) {
	xprintf("Example 01 - Timer\n\n");

	// set trap vector to trap_entry() in trap-entry-single.S
	// it will call exception_handler() or interrupt_handler()
	csr_write(CSR_STVEC, (uintptr_t) trap_entry);

	// enable timer0 irq
	csr_set(CSR_S_INTC_ENABLE, TIMER0_IRQb);

	// enable external interrupts
	csr_set(CSR_SIE, INTb_SVC_EXTERN);

	// enable interrupts 
	irq_enable();

	timer_init();

	while (1) {
		uint32_t now = ticks;
		xprintf("%02u:%02u.%1u\r", now/600, (now/10) % 60, now % 10);

		// wait for ticks to change
		while (now == ticks) ;
	}
}

// if an exception occurs, dump register state and halt
void exception_handler(eframe_t *ef) {
	xprintf("\n** SUPERVISOR EXCEPTION **\n");
	xprint_s_exception(ef);
	xprintf("\nHALT\n");
	for (;;) ;
}

