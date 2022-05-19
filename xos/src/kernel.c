// Copyright 2022, Brian Swetland <swetland@frotz.net>
// Licensed under the Apache License, Version 2.0

#include "kernel.h"

#include <hw/debug.h>
#include <hw/platform.h>
#include <hw/litex.h>

#include <string.h>

// kernel page directory
static uint32_t *kpgdir;

// load general registers from eframe
// save ef + sizeof(eframe_t) as kernel sp and tp for next trap
// enter user mode at ef->pc
void user_mode_entry(eframe_t* ef);

extern char __extra_start[];
extern char __extra_end[];

// the top bits on a kernel stack
typedef struct {
	eframe_t eframe;
	thread_t thread;
} kstack_top_t;

void start_user_program(void) {
	vaddr_t user_start = 0x10000000;
	vaddr_t user_stack = 0x10400000;

	xprintf("user.bin %u bytes\n", (__extra_end - __extra_start));

	// allocate 16KB (4 pages) for user text/data
	vm_map_4k(kpgdir, user_start + 0*PAGE_SIZE, ppage_alloc_z(), USER_RWX);
	vm_map_4k(kpgdir, user_start + 1*PAGE_SIZE, ppage_alloc_z(), USER_RWX);
	vm_map_4k(kpgdir, user_start + 2*PAGE_SIZE, ppage_alloc_z(), USER_RWX);
	vm_map_4k(kpgdir, user_start + 3*PAGE_SIZE, ppage_alloc_z(), USER_RWX);

	// allocate a 4KB (1 page) for user stack
	vm_map_4k(kpgdir, user_stack - 1*PAGE_SIZE, ppage_alloc_z(), USER_RW);

	// allow S-MODE writes to U-MODE pages
	csr_set(CSR_SSTATUS, SSTATUS_SUM);

	// copy embedded user.bin into user memory
	memcpy((void*) user_start, __extra_start, __extra_end - __extra_start);

	csr_clr(CSR_SSTATUS, SSTATUS_SUM);

	// allocate a kernel stack page
	// setup an eframe with the initial user register state
	kstack_top_t *kst = kpage_alloc_z() + PAGE_SIZE - sizeof(kstack_top_t);
	kst->eframe.pc = user_start;
	kst->eframe.sp = user_stack;

	// set previous privilege mode to user (0) 
	csr_clr(CSR_SSTATUS, SSTATUS_SPP);

	xprintf("\n[ starting user mode program ]\n\n");

	user_mode_entry(&kst->eframe);
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
	xprintf("X/OS Kernel v0.1\n\n");

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

	// give all other pages from end-of-kernel to boot pagedir
	// to the virtual memory manager
	vmm_init(kva_to_pa(__image_end), kva_to_pa(__memory_top) - 2 * PAGE_SIZE);

	// build a more correct page table
	kpgdir = kpage_alloc_z();

	char *va = __text_start;
	// map kernel text RX
	while (va < __rodata_start) {
		vm_map_4k(kpgdir, (vaddr_t) va, kva_to_pa(va), KERNEL_RX);
		va += PAGE_SIZE;
	}
	// map kernel rodata RO
	while (va < __data_start) {
		vm_map_4k(kpgdir, (vaddr_t) va, kva_to_pa(va), KERNEL_RO);
		va += PAGE_SIZE;
	}
	// map kernel data and the rest of ram RW
	char *end = (void*) ((((uint32_t) __image_end) + 0x003FFFFF) & 0xFFC00000);
	while (va < end) {
		vm_map_4k(kpgdir, (vaddr_t) va, kva_to_pa(va), KERNEL_RW);
		va += PAGE_SIZE;
	}
	// map as much as possible as 4MB megapages
	while (va < __memory_top) {
		vm_map_4m(kpgdir, (vaddr_t) va, kva_to_pa(va), KERNEL_RW);
		va += 4*1024*1024;
	}
	// map mmio region
	vm_map_4m(kpgdir, 0xF0000000, 0xF0000000, KERNEL_RW);

	csr_write(CSR_SATP, SATP_MODE_SV32 | (kva_to_pa(kpgdir) >> 12));
	tlb_flush_all();

	start_user_program();
}

status_t sys_xputc(uint32_t ch) {
	xputc(ch);
	return XOS_OK;
}

status_t sys_exit(int n) {
	xprintf("\n[ user exit (status %d) ]\n", n);
	for (;;) ;
	return XOS_OK;
}

void interrupt_handler(void) {
}

// if an exception occurs, dump register state and halt
void exception_handler(eframe_t *ef) {
	xprintf("\n** SUPERVISOR EXCEPTION **\n");
	xprint_s_exception(ef);
	xprintf("\nHALT\n");
	for (;;) ;
}

void panic(const char *msg) {
	xprintf("\n** kernel panic: %s **\n", msg);
	for (;;) ;
}
