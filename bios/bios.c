// Copyright 2022, Brian Swetland <swetland@frotz.net>
// Licensed under the Apache License, Version 2.0

#include <hw/riscv.h>
#include <hw/debug.h>

#define SVC_ENTRY 0x80004000

extern void mach_exception_entry(void);

const char* cause(uint32_t n) {
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

const char* mode(uint32_t n) {
	switch (n) {
	case 0: return "User";
	case 1: return "Supervisor";
	case 3: return "Machine Mode";
	default: return "???";
	}
}

void mach_exception_handler(uint32_t regs[32]) {
	uint32_t n = csr_read(CSR_MCAUSE);
	xprintf("\n** MACHINE EXCEPTION %08x %08x\n", n, csr_read(CSR_MTVAL));
	xprintf("** %s\n", cause(n));
	n = csr_read(CSR_MSTATUS);
	xprintf("** in %s mode\n\n", mode((n >> MSTATUS_MPP_SHIFT) & 3));

	xprintf("pc %08x ra %08x sp %08x gp %08x\n",
		regs[0], regs[1], regs[2], regs[3]);
	xprintf("tp %08x t0 %08x t1 %08x t2 %08x\n",
		regs[4], regs[5], regs[6], regs[7]);
	xprintf("fp %08x s1 %08x a0 %08x a1 %08x\n",
		regs[8], regs[9], regs[10], regs[11]);
	xprintf("a2 %08x a3 %08x a4 %08x a5 %08x\n",
		regs[12], regs[13], regs[14], regs[15]);
	xprintf("\n** HALT\n");
	for (;;) ;
}

void enter_mode_u(uint32_t a0, uint32_t a1, uint32_t pc, uint32_t sp);
void enter_mode_s(uint32_t a0, uint32_t a1, uint32_t pc, uint32_t sp);

#if 0
void user_start(uint32_t user_pc, uint32_t user_sp) {
	xprintf("Hello, User Mode %08x %08x\n", user_pc, user_sp);

	// illegal write from user mode
	// csr_write(CSR_MEPC, 0x42);

	// syscall
	asm volatile ("ecall");

	for (;;) ;
}
#endif

#define INT_LIST (INTb_SVC_SW|INTb_SVC_TIMER|INTb_SVC_EXTERN)
#define EXC_LIST (EXCb_ECALL_UMODE)

void start(uint32_t hartid, uint32_t fdt) {
	xprintf("** Frobozz Magic BIOS v0.1 **\n");

	// delegate interrupts and exceptions
	//xprintf("MIDELEG %08x MEDELEG %08x\n",
	//	csr_read(CSR_MIDELEG), csr_read(CSR_MEDELEG));
	csr_set(CSR_MIDELEG, INT_LIST);
	csr_set(CSR_MEDELEG, EXC_LIST);
	//xprintf("MIDELEG %08x MEDELEG %08x\n\n",
	//	csr_read(CSR_MIDELEG), csr_read(CSR_MEDELEG));

	// set previous status to S_MODE, previous interrupt status ENABLED
	csr_write(CSR_MSTATUS, (PRIV_S << MSTATUS_MPP_SHIFT) | MSTATUS_MPIE);
	//xprintf("MSTATUS %08x\n", csr_read(CSR_MSTATUS));

	// set mach exception vector and stack pointer
	csr_write(CSR_MTVEC, (uintptr_t) mach_exception_entry);
	csr_write(CSR_MSCRATCH, SVC_ENTRY);

	enter_mode_s(hartid, fdt, SVC_ENTRY, 0);
}

