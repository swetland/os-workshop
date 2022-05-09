// Copyright 2022, Brian Swetland <swetland@frotz.net>
// Licensed under the Apache License, Version 2.0

#include <hw/riscv.h>
#include <hw/debug.h>
#include <hw/platform.h>

#define MEMORY_TOP (DRAM_BASE + DRAM_SIZE)

#define SVC_SP (MEMORY_TOP - 8*1024)
#define USER_SP (MEMORY_TOP - 16*1024)

extern void svc_exception_entry(void);
void enter_mode_u(uint32_t a0, uint32_t a1, uint32_t user_pc, uint32_t user_sp);

void svc_exception_handler(uint32_t regs[32]) {
	uint32_t cause = csr_read(CSR_SCAUSE);
	uint32_t val = csr_read(CSR_STVAL);
	xprintf("\nSUPERVISOR EXCEPTION %08x %08x %08x\n",
		cause, val, (unsigned) regs);

	xprintf("pc %08x ra %08x sp %08x gp %08x\n",
		regs[0], regs[1], regs[2], regs[3]);
	xprintf("tp %08x t0 %08x t1 %08x t2 %08x\n",
		regs[4], regs[5], regs[6], regs[7]);
	xprintf("fp %08x s1 %08x a0 %08x a1 %08x\n",
		regs[8], regs[9], regs[10], regs[11]);
	xprintf("a2 %08x a3 %08x a4 %08x a5 %08x\n\n",
		regs[12], regs[13], regs[14], regs[15]);

	if (cause == EXCn_ECALL_UMODE) {
		// advance return address to next instr
		regs[0] += 4;
		xprintf("RETURNING FROM ECALL\n");
		return;
	}

	xprintf("HALTED\n");
	for (;;) ;
}

void user_start(uint32_t hartid, uint32_t fdt) {
	xprintf("Hello, User Mode hartid=%08x fdt=%08x\n", hartid, fdt);

	// syscall
	asm volatile ("ecall");

	// illegal write from user mode
	csr_write(CSR_MEPC, 0x42);

	xprintf("\nSTOP\n");
	for (;;) ;
}

void start(uint32_t hartid, uint32_t fdt) {
	xprintf("Hello, Trap Test %08x %08x\n", hartid, fdt);	

	// set svc exception vector and stack pointer
	csr_write(CSR_STVEC, (uintptr_t) svc_exception_entry);
	csr_write(CSR_SSCRATCH, SVC_SP);

	//csr_set(CSR_SIE, INTb_SVC_SW);

	enter_mode_u(hartid, fdt, (uintptr_t)user_start, USER_SP);

	for (;;) ;
}

