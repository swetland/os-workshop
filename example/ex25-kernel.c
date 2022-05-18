// Copyright 2022, Brian Swetland <swetland@frotz.net>
// Licensed under the Apache License, Version 2.0

#include <hw/riscv.h>
#include <hw/context.h>
#include <hw/debug.h>
#include <hw/intrinsics.h>

#include <hw/platform.h>
#include <hw/litex.h>

#include <string.h>

#include "ex25-syscalls.h"

#define PAGE_SIZE 4096

typedef uint32_t paddr_t;
typedef uint32_t vaddr_t;

// physical address to kernel virtual address
static inline void* pa_to_kva(paddr_t pa) {
	return (void*) (pa + (0xC0000000 - 0x40000000));
}
static inline paddr_t kva_to_pa(void *kva) {
	return ((paddr_t) kva) - (0xC0000000 - 0x40000000);
}

// trivial physical page allocator
paddr_t ppage_next = 0;

paddr_t alloc_ppage(void) {
	ppage_next -= PAGE_SIZE;
	memset(pa_to_kva(ppage_next), 0, PAGE_SIZE);
	return ppage_next;
}

#define PTE_PA(pte) (((pte) & 0xFFFFFC00) << 2)

uint32_t* pdir = 0;

#define USER_RW (PTE_D|PTE_A|PTE_U|PTE_R|PTE_W|PTE_V)
#define USER_RX (PTE_D|PTE_A|PTE_U|PTE_R|PTE_X|PTE_V)
#define USER_RWX (PTE_D|PTE_A|PTE_U|PTE_R|PTE_W|PTE_X|PTE_V)

#define KERNEL_RO (PTE_D|PTE_A|PTE_R|PTE_V)
#define KERNEL_RW (PTE_D|PTE_A|PTE_R|PTE_W|PTE_V)
#define KERNEL_RX (PTE_D|PTE_A|PTE_R|PTE_X|PTE_V)

int map_page(vaddr_t va, paddr_t pa, uint32_t flags) {
	uint32_t idx1 = va >> 22;
	uint32_t idx0 = (va >> 12) & 0x3FF;
	uint32_t *ptbl;

	//xprintf("map page va=0x%08x(%d,%d) -> pa=0x%08x (4K)\n", va, idx1, idx0, pa);
	uint32_t pte = pdir[idx1];
	if (pte & PTE_V) { // valid pgdir entry
		if (pte & (PTE_X|PTE_W|PTE_R)) { 
			return -1; // 4MB megapage already here
		}
		ptbl = pa_to_kva(PTE_PA(pte));
		//xprintf("old ptbl pa=0x%08x va=%p\n", PTE_PA(pte), ptbl);
	} else { // no entry, allocate a pagetable
		uint32_t ptbl_pa = alloc_ppage();
		ptbl = pa_to_kva(ptbl_pa);
		//xprintf("new ptbl pa=0x%08x va=%p\n", ptbl_pa, ptbl);
		pdir[idx1] = (ptbl_pa >> 2) | PTE_V;
	}

	pte = ptbl[idx0];
	if (pte & PTE_V) {
		return -1; // 4KB page already here
	}

	ptbl[idx0] = ((pa & 0xFFFFF000) >> 2) | (flags & 0x3FF);

	tlb_flush_all();

	return 0;
}

int map_page_4m(vaddr_t va, paddr_t pa, uint32_t flags) {
	uint32_t idx1 = va >> 22;

	//xprintf("map page va=0x%08x(%d) -> pa=0x%08x (4M)\n", va, idx1, pa);
	uint32_t pte = pdir[idx1];
	if (pte & PTE_V) { // valid pgdir entry
		return -1;
	}
	pdir[idx1] = ((pa & 0xFFFFF000) >> 2) | (flags & 0x3FF);

	tlb_flush_all();
	return 0;
}

// load general registers from eframe
// save ef + sizeof(eframe_t) as kernel sp for next trap
// enter user mode at ef->pc
void user_mode_entry(eframe_t* ef);

extern char __extra_start[];
extern char __extra_end[];

void start_user_program(void) {
	vaddr_t user_start = 0x10000000;
	vaddr_t user_stack = 0x10400000;

	xprintf("user.bin %u bytes\n", (__extra_end - __extra_start));

	// allocate 16KB (4 pages) for user text/data
	map_page(user_start + 0*PAGE_SIZE, alloc_ppage(), USER_RWX);
	map_page(user_start + 1*PAGE_SIZE, alloc_ppage(), USER_RWX);
	map_page(user_start + 2*PAGE_SIZE, alloc_ppage(), USER_RWX);
	map_page(user_start + 3*PAGE_SIZE, alloc_ppage(), USER_RWX);

	// allocate a 4KB (1 page) for user stack
	map_page(user_stack - 1*PAGE_SIZE, alloc_ppage(), USER_RW);

	// allow S-MODE writes to U-MODE pages
	csr_set(CSR_SSTATUS, SSTATUS_SUM);

	// copy embedded user.bin into user memory
	memcpy((void*) user_start, __extra_start, __extra_end - __extra_start);

	csr_clr(CSR_SSTATUS, SSTATUS_SUM);

	// allocate a kernel stack page
	// setup an eframe with the initial user register state
	eframe_t* ef = pa_to_kva(alloc_ppage() + PAGE_SIZE - sizeof(eframe_t));
	ef->pc = user_start;
	ef->sp = user_stack;

	// set previous privilege mode to user (0) 
	csr_clr(CSR_SSTATUS, SSTATUS_SPP);

	xprintf("\n[ starting user mode program ]\n\n");

	user_mode_entry(ef);
	// does not return
}

extern char __text_start[];
extern char __rodata_start[];
extern char __data_start[];
extern char __bss_start[];
extern char __bss_end[];
extern char __image_end[];
extern char __memory_top[];

// On entry:
// SP = __memory_top  (top of ram)
// SATP points at __memory_top - 8K

void start(void) {
	xprintf("Example 25 - kernel\n\n");

	xprintf("text   %p %u\n", __text_start, __rodata_start - __text_start);
	xprintf("rodata %p %u\n", __rodata_start, __data_start - __rodata_start);
	xprintf("data   %p %u\n", __data_start, __bss_start - __data_start);
	xprintf("bss    %p %u\n", __bss_start, __bss_end - __bss_start);
	xprintf("free   %p %u\n", __image_end, __memory_top - __image_end);
	xprintf("memtop %p\n", __memory_top);

	// set trap vector to trap_entry() in trap-entry-single.S
	// it will call exception_handler() or interrupt_handler()
	csr_write(CSR_STVEC, (uintptr_t) trap_entry);

	// the topmost page is the boot stack
	// the second topmost page is the boot page directory

	// setup page allocator to start grabbing pages from the top of ram
	ppage_next = kva_to_pa(__memory_top) - 2 * PAGE_SIZE;

#if 0
	// use the boot page table
	pdir = (void*) __memory_top - 2 * PAGE_SIZE;
#else
	// build a more correct page table
	pdir = pa_to_kva(alloc_ppage());

	char *va = __text_start;
	// map kernel text RX
	while (va < __rodata_start) {
		map_page((vaddr_t) va, kva_to_pa(va), KERNEL_RX);
		va += PAGE_SIZE;
	}
	// map kernel rodata RO
	while (va < __data_start) {
		map_page((vaddr_t) va, kva_to_pa(va), KERNEL_RO);
		va += PAGE_SIZE;
	}
	// map kernel data and the rest of ram RW
	char *end = (void*) ((((uint32_t) __image_end) + 0x003FFFFF) & 0xFFC00000);
	while (va < end) {
		map_page((vaddr_t) va, kva_to_pa(va), KERNEL_RW);
		va += PAGE_SIZE;
	}
	// map as much as possible as 4MB megapages
	while (va < __memory_top) {
		map_page_4m((vaddr_t) va, kva_to_pa(va), KERNEL_RW);
		va += 4*1024*1024;
	}
	// map mmio region
	map_page_4m(0xF0000000, 0xF0000000, KERNEL_RW);

	csr_write(CSR_SATP, SATP_MODE_SV32 | (kva_to_pa(pdir) >> 12));
	tlb_flush_all();
#endif

	start_user_program();
}

void interrupt_handler(void) {
}

// if an exception occurs, dump register state and halt
void exception_handler(eframe_t *ef) {
	uint32_t cause = csr_read(CSR_SCAUSE);

	if (cause == EXCn_ECALL_UMODE) {
		switch (ef->t0) { // syscall number
		case SYS_EXIT:
			xprintf("\n[ user exit (status %d) ]\n", ef->a0);
			for (;;) ;
		case SYS_XPUTC:
			xputc(ef->a0);
			break;
		default:
			xprintf("\n[ invalid syscall %u ]\n", ef->t0);
			for (;;) ;
		}
		ef->pc += 4;
		return;
	}

	xprintf("\n** SUPERVISOR EXCEPTION **\n");
	xprint_s_exception(ef);
	xprintf("\nHALT\n");
	for (;;) ;
}

