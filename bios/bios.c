// Copyright 2022, Brian Swetland <swetland@frotz.net>
// Licensed under the Apache License, Version 2.0

#include <hw/riscv.h>
#include <hw/riscv-clint.h>
#include <hw/debug.h>
#include "bios.h"

#include <hw/platform.h>
#include <hw/litex.h>

#define uart_rd(a) io_rd32(UART0_BASE + LX_UART_ ## a)
#define uart_wr(a,v) io_wr32(UART0_BASE + LX_UART_ ## a, v)

#define SVC_ENTRY (DRAM_BASE + BIOS_SIZE)

void mach_exception_entry(void);
void enter_mode_s(uint32_t a0, uint32_t a1, uint32_t pc, uint32_t sp);

static const char* _cause(uint32_t n) {
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

static const char* _mode(uint32_t n) {
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

#if IRQ_TEST
	if (mcause == 0x8000000b) {
		xprintf("IP %x\n", csr_read(CSR_M_INTC_PENDING));
		if (uart_rd(EV_PENDING) & LX_UART_EVb_RX) {
			unsigned ch = uart_rd(RX);
			uart_wr(EV_PENDING, LX_UART_EVb_RX);
			//csr_clr(CSR_MIP, INTb_MACH_EXTERN);
			xputc('.');
			xputc(ch);
		} else {
			xputc('?');
			//csr_clr(CSR_MIP, INTb_MACH_EXTERN);
		}
		csr_write(CSR_MIP, 0);
		//csr_write(CSR_M_INTC_PENDING, 1);
		return;
	}
#endif

#if EMULATE_MISSING_CSRS
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
	uint32_t mtinst = 0; //csr_read(CSR_MTINST);
	xprintf("** %s (in %s mode)\n\n", _cause(mcause),
		_mode((mstatus >> MSTATUS_MPP_SHIFT) & 3));

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

#define INT_LIST (INTb_SVC_SW|INTb_SVC_TIMER|INTb_SVC_EXTERN)
#define EXC_LIST (EXCb_ECALL_UMODE)

#define CLINT_BASE 0x2000000
#define TIME_TICK 10000000

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
	xprintf("MSTATUS %08x\n", csr_read(CSR_MSTATUS));

	// set mach exception vector and stack pointer
	csr_write(CSR_MTVEC, ((uintptr_t) mach_exception_entry) );

	// leaving room for the timer interrupt workspace above the stack
	csr_write(CSR_MSCRATCH, SVC_ENTRY - IWS_SIZE);

	xprintf("HARTID %08x\n", hartid);

#if USE_CLINT_TIMER
	uint32_t* iws = (void*) (SVC_ENTRY - IWS_SIZE);
	uint32_t mtimecmp = CLINT_BASE + CLINT_MTIMECMP(hartid);
	uint32_t mtime = CLINT_BASE + CLINT_MTIME;

	iws[IWS_TIMECMP/4] =  mtimecmp;
	iws[IWS_TICKINC/4] = TIME_TICK;

	// set initial tick
	uint64_t next = *((uint64_t*) mtime) + TIME_TICK;
	*((uint64_t*) mtimecmp) = next;

	csr_set(CSR_MIE, INTb_MACH_TIMER);
	xprintf("MIE %08x\n", csr_read(CSR_MIE));
#endif

	// U/S allow access to all memory
	csr_write(CSR_PMPCFG(0), PMP_CFG_A_TOR | PMP_CFG_X | PMP_CFG_W | PMP_CFG_R);
	csr_write(CSR_PMPADDR(0), 0xFFFFFFFF);

#if UART_DEBUG_GOOP
	uart_wr(EV_ENABLE, 3);
	// enable machine mode external interrupts (cpu)
	csr_set(CSR_MIE, INTb_MACH_EXTERN);
	xprintf("MIE %08x\n", csr_read(CSR_MIE));

	// enable machine mode interrupts (cpu)
	csr_set(CSR_MSTATUS, MSTATUS_MIE);

	uart_wr(EV_ENABLE, 0);

	// clear any outstanding UART interrupts
	uart_wr(EV_PENDING, LX_UART_EVb_RX);

	//csr_set(CSR_M_INTC_PENDING, UART0_IRQb);

	// enable UART0 interrupts (intc)
	csr_set(CSR_M_INTC_ENABLE, UART0_IRQb);

	// enable RX interrupts (uart0)
	uart_wr(EV_ENABLE, LX_UART_EVb_RX);
	xprintf("WHEE!!!\n");

	for (;;) ;
#endif
#if MORE_UART_DEBUG
	for (;;) {
		while (uart_rd(RXEMPTY)) ;
		//uart_wr(TX, '.');
		xprintf("RX %02x %x %x EVP%x EVS%x\n", uart_rd(TX), uart_rd(RXEMPTY), uart_rd(RXFULL),
uart_rd(EV_PENDING), uart_rd(EV_STATUS));
//		while(uart_rd(RXEMPTY) == 0) xputc('.');
//		uart_wr(TX, (*((volatile uint8_t*) (UART0_BASE + LX_UART_RX)))); //uart_rd(RX));
//		uart_wr(EV_PENDING, LX_UART_EVb_RX);
	}
#endif

#if DUMP_CSRS	
#define X(csr) xprintf("%08x " #csr "\n", csr_read(csr))

#define CSR_MTVEC_WO
#define CSR_MIDELEG_WO
#define CSR_MEDELEG_WO

	csr_write(CSR_MIDELEG, 0);
	csr_write(CSR_MEDELEG, 0);

	X(CSR_MVENDORID);
	X(CSR_MARCHID);
	X(CSR_MIMPID);
	X(CSR_MHARTID);
	X(CSR_MCONFIGPTR);
	X(CSR_MSTATUS);
	X(CSR_MISA);
	X(CSR_MEDELEG);
	X(CSR_MIDELEG);
	X(CSR_MIE);
	X(CSR_MTVEC);
	X(CSR_MCOUNTEREN);
	X(CSR_MSTATUSH);
	X(CSR_MSCRATCH);
	X(CSR_MEPC);
	X(CSR_MCAUSE);
	X(CSR_MTVAL);
	X(CSR_MIP);
	X(CSR_MTINST);
	X(CSR_MTVAL2);
	X(CSR_MENVCFG);
	X(CSR_MCYCLE);
	X(CSR_MINSTRET);
	X(CSR_SATP);
	X(CSR_SIP);
	X(CSR_STVAL);
	X(CSR_SCAUSE);
	X(CSR_SEPC);
	X(CSR_SSCRATCH);
	X(CSR_SENVCFG);
	X(CSR_SCOUNTEREN);
	X(CSR_STVEC);
	X(CSR_SIE);
	X(CSR_SSTATUS);

	X(CSR_PMPCFG(0));
	X(CSR_PMPADDR(0));
	xprintf("DONE\n");

	for (;;) ;
#endif

	enter_mode_s(hartid, fdt, SVC_ENTRY, 0);
}

