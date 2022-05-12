// Copyright 2022, Brian Swetland <swetland@frotz.net>
// Licensed under the Apache License, Version 2.0

#include <hw/riscv.h>
#include <hw/context.h>
#include <hw/debug.h>
#include <hw/intrinsics.h>

// if an exception occurs, dump register state and halt
void exception_handler(eframe_t *ef) {
	xprintf("\n** SUPERVISOR EXCEPTION **\n");
	xprint_s_exception(ef);
	xprintf("\nHALT\n");
	for (;;) ;
}

// no interrupts to handle
void interrupt_handler(void) {
	xprintf("WAT?");
	for (;;) ;
}

void start(void) {
	xprintf("Example 00 - Hello\n");

	// set trap vector to trap_entry() in trap-entry-single.S
	// it will call exception_handler() or interrupt_handler()
	csr_write(CSR_STVEC, (uintptr_t) trap_entry);

	// this is illegal in supervisor mode
	csr_write(CSR_MTVEC, 42);
}
