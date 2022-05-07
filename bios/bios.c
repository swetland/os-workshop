// Copyright 2022, Brian Swetland <swetland@frotz.net>
// Licensed under the Apache License, Version 2.0

#include <hw/riscv.h>
#include <hw/riscv-clint.h>
#include <hw/debug.h>
#include "bios.h"

#include <hw/platform.h>
#include <hw/litex.h>

// we expect the supervisor program to be in memory after the end of the BIOS
#define SVC_ENTRY (DRAM_BASE + BIOS_SIZE)

static const char* cause_name(uint32_t n) {
	if (n & 0x80000000U) {
		return "Interrupt";
	} else {
		switch(n & 0x7FFFFFFFU) {
		case 0: return "Instruction Address Misaligned";
		case 1: return "Instruction Address Fault";
		case 2: return "Illegal Instruction";
		case 3: return "Breakpoint";
		case 4: return "Load Address Misaligned";
		case 5: return "Load Address Fault";
		case 6: return "Store Address Misaligned";
		case 7: return "Store Address Fault";
		case 8: return "User Mode ECALL";
		case 9: return "Supervisor Mode ECALL";
		case 10: return "Machine Mode ECALL";
		case 12: return "Instruction Page Fault";
		case 13: return "Load Page Fault";
		case 14: return "Store Page Fault";
		}
	}
	return "Unknown";
}

static const char* mode_name(uint32_t n) {
	switch (n) {
	case 0: return "User";
	case 1: return "Supervisor";
	case 3: return "Machine Mode";
	default: return "???";
	}
}

void mach_exception_handler(uint32_t regs[32]) {
	uint32_t mcause = csr_read(CSR_MCAUSE);
	uint32_t mstatus = csr_read(CSR_MSTATUS);
	uint32_t mtval = csr_read(CSR_MTVAL);
	uint32_t mtinst = 0; //csr_read(CSR_MTINST); // not supported

#if EMULATE_MISSING_CSR_READS
	// fragile: no mtval on qemu
	if (mcause == 2) mtval = *((uint32_t*) regs[0]);

	if ((mcause == 2) && ((mtval & (~0xFFF00F80)) == 0x00002073)) {
		uint32_t rd = (mtval >> 7) & 31;
		xprintf("BAD CSR READ @%08x -> r%u\n", regs[0], rd);
		regs[rd] = 0xDEADBEEF;
		regs[0] += 4;
		return;
	}
#endif

	xprintf("\n** MACHINE EXCEPTION **\n");
	xprintf("** %s (in %s mode)\n\n", cause_name(mcause),
		mode_name((mstatus >> MSTATUS_MPP_SHIFT) & 3));

	xprintf("pc %08x ra %08x sp %08x gp %08x  MSTATUS %08x\n",
		regs[0], regs[1], regs[2], regs[3], mstatus);
	xprintf("tp %08x t0 %08x t1 %08x t2 %08x  MCAUSE  %08x\n",
		regs[4], regs[5], regs[6], regs[7], mcause);
	xprintf("fp %08x s1 %08x a0 %08x a1 %08x  MTVAL   %08x\n",
		regs[8], regs[9], regs[10], regs[11], mtval);
	xprintf("a2 %08x a3 %08x a4 %08x a5 %08x  MTINST  %08x\n",
		regs[12], regs[13], regs[14], regs[15], mtinst);
	xprintf("\n** HALT\n");
	for (;;) ;
}

// interrupts and exceptions to delegate to supervisor mode
#define INT_LIST (INTb_SVC_SW|INTb_SVC_TIMER|INTb_SVC_EXTERN)
#define EXC_LIST (EXCb_ECALL_UMODE)

void start(uint32_t hartid, uint32_t fdt) {
	xprintf("\n** Frobozz Magic BIOS v0.1 **\n\n");

	int qemu = (csr_read(CSR_MVENDORID) == 0);

	// delegate interrupts and exceptions
	csr_set(CSR_MIDELEG, INT_LIST);
	csr_set(CSR_MEDELEG, EXC_LIST);

	// set previous status to S_MODE, previous interrupt status ENABLED
	csr_write(CSR_MSTATUS, (PRIV_S << MSTATUS_MPP_SHIFT) | MSTATUS_MPIE);

	// set mach exception vector and stack pointer
	csr_write(CSR_MTVEC, ((uintptr_t) mach_exception_entry) );

	// use the free ram below the supervisor entry as our exception stack
	csr_write(CSR_MSCRATCH, SVC_ENTRY);

#if USE_CLINT_TIMER
#define CLINT_BASE 0x2000000
#define TIME_TICK 10000000

	// leaving room for the timer interrupt workspace above the stack
	csr_write(CSR_MSCRATCH, SVC_ENTRY - IWS_SIZE);

	uint32_t* iws = (void*) (SVC_ENTRY - IWS_SIZE);
	uint32_t mtimecmp = CLINT_BASE + CLINT_MTIMECMP(hartid);
	uint32_t mtime = CLINT_BASE + CLINT_MTIME;

	iws[IWS_TIMECMP/4] =  mtimecmp;
	iws[IWS_TICKINC/4] = TIME_TICK;

	// set initial tick
	uint64_t next = *((uint64_t*) mtime) + TIME_TICK;
	*((uint64_t*) mtimecmp) = next;

	csr_set(CSR_MIE, INTb_MACH_TIMER);
#endif

	// QEMU currently emulates memory protection, which we must appease
	if (qemu) {
		// U/S allow access to all memory
		csr_write(CSR_PMPCFG(0), PMP_CFG_A_TOR | PMP_CFG_X | PMP_CFG_W | PMP_CFG_R);
		csr_write(CSR_PMPADDR(0), 0xFFFFFFFF);
	}

	xprintf("SVC ENTRY @0x%08x\n\n", SVC_ENTRY);
	enter_mode_s(hartid, fdt, SVC_ENTRY, 0);
}

