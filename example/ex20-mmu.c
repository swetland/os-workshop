// Copyright 2022, Brian Swetland <swetland@frotz.net>
// Licensed under the Apache License, Version 2.0

#include <hw/riscv.h>
#include <hw/context.h>
#include <hw/debug.h>
#include <hw/intrinsics.h>

#include <hw/platform.h>
#include <hw/litex.h>

#include <string.h>

// use inline assembly to defeat gcc from converting stuff like
// *((unsigned*) 0) = 42; to unimplemented instructions...

static inline void wr32(uint32_t addr, uint32_t val) {
	asm volatile ("sw %0, 0(%1)" :: "r"(val), "r"(addr));
}
static inline uint32_t rd32(uint32_t addr) {
	uint32_t val;
	asm volatile ("lw %0, 0(%1)" : "=r"(val) : "r"(addr));
	return val;
}

// this ideally should do a read safe from faulting
uint32_t read32_safe(uint32_t addr) {
	return *((volatile uint32_t*) addr);
}

void dump_pte(uint32_t addr, uint32_t vaddr, uint32_t pte, uint32_t sz) {
	char bits[11];
	for (unsigned n = 0; n < 10; n++) {
		bits[9-n] = (pte & (1<<n)) ? "vrwxugad01"[n] : '-';
	}
	bits[10] = 0;
	xprintf("%08x %08x %08x0 %08x %s\n",
		addr, vaddr, (pte & 0xFFFFFC00) >> 2, sz, bits);
}

void dump_mmu0(uint32_t ptbl, uint32_t vaddr) {
	for (unsigned n =  0; n < 1024; n++) {
		uint32_t addr = ptbl + n * 4;
		uint32_t pte = read32_safe(addr);
		dump_pte(addr, vaddr | (n << 12), pte, 4*1024);
	}
}

void dump_mmu(void *_ptbl) {
	xprintf("pte addr vaddr    paddr     size     flags\n");
	xprintf("-------- -------- --------- -------- ----------\n");
	uint32_t ptbl = (uint32_t) _ptbl;
	for (unsigned n = 0; n < 1024; n++) {
		uint32_t addr = ptbl + n * 4;
		uint32_t pte = read32_safe(addr);
		if (pte & PTE_V) {
			dump_pte(addr, n << 22, pte, 4*1024*1024);
			if (!(pte & (PTE_X|PTE_W|PTE_R))) {
				dump_mmu0((pte & 0xFFFFFC00) << 2, n << 22);
			}
		}
	}
}

uint32_t alloc_top = 0x40000000 + 4*1024*1024;

void* alloc_page(void) {
	alloc_top -= 4096;
	return (void*) alloc_top;
}

volatile uint32_t expect_cause = 0;
volatile uint32_t expect_addr = 0;

void start(void) {
	xprintf("Example 20 - MMU\n\n");

	// set trap vector to trap_entry() in trap-entry-single.S
	// it will call exception_handler() or interrupt_handler()
	csr_write(CSR_STVEC, (uintptr_t) trap_entry);

	uint32_t* pt1 = alloc_page();
	memset(pt1, 0, 4096);

	// map ram as a set of 4MB pages 1:1
	uint32_t phys;
	for (phys = 0x40000000 ; phys < 0x42000000; phys += 0x00400000) {
		pt1[phys >> 22] = (phys >> 2) | PTE_D | PTE_A | PTE_X | PTE_W | PTE_R | PTE_V;
	}

	// unset A & D bit for second to last 4MB page
	pt1[0x41800000 >> 22] &= ~(PTE_A | PTE_D);

	// map mmio region 1:1
	pt1[0xF0000000 >> 22] = (0xF0000000 >> 2) | PTE_D | PTE_A | PTE_W | PTE_R | PTE_V;

	dump_mmu(pt1);

	// enable MMU
	csr_write(CSR_SATP, 0x80000000 | ((uintptr_t) pt1) >> 12);
	tlb_flush_all();
	asm volatile ("nop ; nop ; nop ; nop ; nop");

	xprintf("\n[ mmu enabled ]\n\n");

	// read and write second to last page and observe the A & D bits changing
	// note: CPU is allowed to fault instead of setting A or D
	//       qemu sets A/D and does not fault
	//       vexrisc does not set A or D but also does not fault

	xprintf("PTE for vaddr 41800000 is %08x\n", rd32((uintptr_t) &pt1[0x41800000 >> 22]));
	xprintf("[ read from 0x41800000 ]\n");
	rd32(0x41800000);
	xprintf("PTE for vaddr 41800000 is %08x\n", rd32((uintptr_t) &pt1[0x41800000 >> 22]));
	
	xprintf("[ write to 0x41800000 ]\n");
	wr32(0x41800000, 17);
	xprintf("PTE for vaddr 41800000 is %08x\n", rd32((uintptr_t) &pt1[0x41800000 >> 22]));


	// cause some faults
	expect_cause = EXCn_STORE_PAGE_FAULT;
	expect_addr = 0;
	wr32(0, 42);

	expect_cause = EXCn_LOAD_PAGE_FAULT;
	expect_addr = 0x100000;
	rd32(0x100000);

	void (*fn)(void) = (void*) 0x777000;
	fn();
}

void interrupt_handler(void) {
}

// if an exception occurs, dump register state and halt
void exception_handler(eframe_t *ef) {
	xprintf("\n** SUPERVISOR EXCEPTION **\n");
	xprint_s_exception(ef);
	
	if ((expect_cause == csr_read(CSR_SCAUSE)) &&
		(expect_addr == csr_read(CSR_STVAL))) {
		ef->pc += 4;
		xprintf("\n");
		return;
	}

	xprintf("\nHALT\n");
	for (;;) ;
}

