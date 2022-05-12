// Copyright 2022, Brian Swetland <swetland@frotz.net>
// Licensed under the Apache License, Version 2.0

#include <hw/debug.h>
#include <hw/riscv.h>
#include <hw/context.h>
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

static void xprint_regs(eframe_t* ef, uint32_t xstatus, uint32_t xcause, uint32_t xtval) {
	xprintf("pc %08x ra %08x sp %08x gp %08x  xstatus %08x\n",
		ef->pc, ef->ra, ef->sp, ef->gp, xstatus);
	xprintf("tp %08x t0 %08x t1 %08x t2 %08x  xcause  %08x\n",
		ef->tp, ef->t0, ef->t1, ef->t2, xcause);
	xprintf("fp %08x s1 %08x a0 %08x a1 %08x  xtval   %08x\n",
		ef->s0, ef->s1, ef->a0, ef->a1, xtval);
	xprintf("a2 %08x a3 %08x a4 %08x a5 %08x\n",
		ef->a2, ef->a3, ef->a4, ef->a5);
	xprintf("a6 %08x a7 %08x s2 %08x s3 %08x\n",
		ef->a6, ef->a7, ef->s2, ef->s3);
	xprintf("s4 %08x s5 %08x s6 %08x s7 %08x\n",
		ef->s4, ef->s5, ef->s6, ef->s7);
	xprintf("s8 %08x s9 %08x 10 %08x 11 %08x\n",
		ef->s8, ef->s9, ef->s10, ef->s11);
	xprintf("t3 %08x t4 %08x t5 %08x t6 %08x\n",
		ef->t3, ef->t4, ef->t5, ef->t6);
}

void xprint_m_exception(eframe_t* ef) {
	uint32_t mstatus = csr_read(CSR_MSTATUS);
	uint32_t mcause = csr_read(CSR_MCAUSE);
	uint32_t mtval = csr_read(CSR_MTVAL);
	xprintf("** %s (in %s mode)\n\n", cause_name(mcause),
		mode_name((mstatus >> MSTATUS_MPP_SHIFT) & 3));
	xprint_regs(ef, mstatus, mcause, mtval);

}
void xprint_s_exception(eframe_t* ef) {
	uint32_t sstatus = csr_read(CSR_SSTATUS);
	uint32_t scause = csr_read(CSR_SCAUSE);
	uint32_t stval = csr_read(CSR_STVAL);
	xprintf("** %s (in %s mode)\n\n", cause_name(scause),
		mode_name((sstatus >> SSTATUS_SPP_SHIFT) & 1));
	xprint_regs(ef, sstatus, scause, stval);
}

