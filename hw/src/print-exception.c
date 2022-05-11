// Copyright 2022, Brian Swetland <swetland@frotz.net>
// Licensed under the Apache License, Version 2.0

#include <hw/debug.h>
#include <hw/riscv.h>
#include <hw/intrinsics.h>

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

void xprint_exception(uint32_t regs[32]) {
	uint32_t mcause = csr_read(CSR_MCAUSE);
	uint32_t mstatus = csr_read(CSR_MSTATUS);
	uint32_t mtval = csr_read(CSR_MTVAL);

	xprintf("** %s (in %s mode)\n\n", cause_name(mcause),
		mode_name((mstatus >> MSTATUS_MPP_SHIFT) & 3));

	xprintf("pc %08x ra %08x sp %08x gp %08x  MSTATUS %08x\n",
		regs[0], regs[1], regs[2], regs[3], mstatus);
	xprintf("tp %08x t0 %08x t1 %08x t2 %08x  MCAUSE  %08x\n",
		regs[4], regs[5], regs[6], regs[7], mcause);
	xprintf("fp %08x s1 %08x a0 %08x a1 %08x  MTVAL   %08x\n",
		regs[8], regs[9], regs[10], regs[11], mtval);
	xprintf("a2 %08x a3 %08x a4 %08x a5 %08x\n",
		regs[12], regs[13], regs[14], regs[15]);
}
