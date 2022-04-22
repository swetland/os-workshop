// Copyright 2022, Brian Swetland <swetland@frotz.net>
// Licensed under the Apache License, Version 2.0

#include <hw/debug.h>
#include <hw/riscv.h>

void main(void) {
	xprintf("Hello?\n");
	xprintf("MSTATUS    %08x\n", csr_read(CSR_MSTATUS));
	xprintf("MISA       %08x\n", csr_read(CSR_MISA));
	xprintf("MCYCLE     %08x\n", csr_read(CSR_MCYCLE));

	uint32_t n = csr_read(CSR_MISA);
	xprintf("ISA: RV32");
	for (int i = 0; i < 26; i++) {
		if (n & (1<<i)) xputc('A'+i);
	}
	xputc('\n');
}
