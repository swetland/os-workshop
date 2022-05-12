// Copyright 2022, Brian Swetland <swetland@frotz.net>
// Licensed under the Apache License, Version 2.0

#include <hw/riscv.h>
#include <hw/context.h>
#include <hw/intrinsics.h>
#include <hw/debug.h>

#include <hw/platform.h>
#include <hw/litex.h>

#include "boot.h"

// we expect the supervisor program to be in memory after the end of the bootloader
#define SVC_ENTRY (DRAM_BASE + BOOTLOADER_SIZE)

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
	xprintf("\n** Frobozz Magic Bootloader v0.2 **\n\n");

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

	// set previous status to S_MODE, previous interrupt status ENABLED
	csr_write(CSR_MSTATUS, (PRIV_S << MSTATUS_MPP_SHIFT) | MSTATUS_MPIE);

	xprintf("SVC ENTRY @0x%08x\n\n", SVC_ENTRY);

	exit_mode_m(hartid, fdt, SVC_ENTRY, 0);

}

