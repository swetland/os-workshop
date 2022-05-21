// Copyright 2022, Brian Swetland <swetland@frotz.net>
// Licensed under the Apache License, Version 2.0

#include <hw/riscv.h>
#include <hw/context.h>
#include <hw/intrinsics.h>
#include <hw/debug.h>

#include <hw/platform.h>
#include <hw/litex.h>

#include <string.h>

#include "boot.h"

eframe_t *EF;
volatile int STOP = 0;

// we expect the supervisor program to be in memory after the end of the bootloader
#define SVC_ENTRY (DRAM_BASE + BOOTLOADER_SIZE)


#define uart_rd(a) io_rd32(UART0_BASE + LX_UART_ ## a)
#define uart_wr(a,v) io_wr32(UART0_BASE + LX_UART_ ## a, v)

static char cmdbuf[128];
static unsigned cmdlen = 0;

void console_char(uint32_t ch) {
	if ((ch == '\r') || (ch == '\n')) {
		xputs("\r\n");
		cmdbuf[cmdlen] = 0;
		// rd32safe, rd8safe modify MTVEC/MSTATUS
		// ensure they're saved and restored here
		uint32_t status = csr_read(CSR_MSTATUS);
		console_line(cmdbuf);
		csr_write(CSR_MSTATUS, status);
		csr_write(CSR_MTVEC, ((uintptr_t) mach_exception_entry) );
		cmdlen = 0;
		return;
	}
	if ((ch == 8) || (ch == 127)) {
		if (cmdlen > 0) {
			xputs("\x08 \x08");
			cmdlen--;
		} else {
			xputc(7);
		}
		return;
	}
	if ((ch < ' ') || (ch > 127)) {
		return;
	}
	if (cmdlen == (sizeof(cmdbuf) - 1)) {
		xputc(7);
		return;
	}
	cmdbuf[cmdlen++] = ch;
	xputc(ch);
}

void mach_exception_handler(eframe_t *ef) {
	uint32_t cause = csr_read(CSR_MCAUSE);

	if (cause == 0x8000000b) {
		EF = ef;
		do {
			while (uart_rd(RXEMPTY) == 0) {
				unsigned ch = uart_rd(RX);
				uart_wr(EV_PENDING, LX_UART_EVb_RX);
				console_char(ch);
			}
		} while (STOP);
		return;
	}

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

	uart_wr(EV_ENABLE, LX_UART_EVb_RX);
	uart_wr(EV_PENDING, LX_UART_EVb_RX);
	csr_write(CSR_M_INTC_ENABLE, UART0_IRQb);
	csr_set(CSR_MIE, INTb_MACH_EXTERN);

	exit_mode_m(hartid, fdt, SVC_ENTRY, 0);

}

