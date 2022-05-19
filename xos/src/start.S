// Copyright 2022, Brian Swetland <swetland@frotz.net>
// Licensed under the Apache License, Version 2.0

#include <hw/riscv.h>

// We start running at the physical start of memory (0x40008000)
// We are linked at the virtual address 0xC0000000

// So we must first setup an initial page table that maps
// ram to 0xC0000000, maps the first megapage of ram
// to 0x40000000 (so we don't pull the rug out from under us
// when we switch on the MMU), and maps the start of MMIO
// space at 0xF0000000 so we can still talk to the serial port

#define PTE_BITS_RWX (PTE_A | PTE_D | PTE_X | PTE_W | PTE_R | PTE_V)
#define PTE_BITS_RW  (PTE_A | PTE_D | PTE_W | PTE_R | PTE_V)

.globl _start
_start:
	// zero BSS
	la t0, __bss_start
	la t1, __bss_end
zero_loop:
	beq t0, t1, zero_done
	sw zero, 0(t0)
	add t0, t0, 4
	j zero_loop
zero_done:

	// physical memory top
	// (since it's pc relative and pc is physical right now)
	la sp, __memory_top

	// use the next to last page (before the stack) 
	// for a bootstrap MMU page directory
	li t4, 4096
	sub t1, sp, t4    // t1 = end of pagetable
	sub t0, t1, t4    // t0 = start of pagetable
	mv t2, t0         // t2 = pagetable

	// virtual memory top / boot stack
	lw sp, start_sp

ptbl_zero_loop:
	sw zero, 0(t0)
	addi t0, t0, 4
	bne t0, t1, ptbl_zero_loop

	// identity map a 4MB page where we're running now
	li t0, (0x40000000 >> 2) | PTE_BITS_RWX
	li t1, (0x40000000 >> 20) // offset of pte
	add t1, t2, t1
	sw t0, 0(t1)

	// map ram at 0xC0000000 where the kernel is linked
	li t1, (0xC0000000 >> 20) // offset of pte at start of ram
	srli t3, sp, 20           // offset of pte at end of ram
	li t4, (0x00400000 >> 2)  // 4MB ppn increment
ptbl_map_loop:
	add t5, t2, t1            // t5 = pagetable + pte offset
	sw t0, 0(t5)              // store pte
	add t1, t1, 4             // increment offset to next pte
	add t0, t0, t4            // increment ppn to next megapage
	bne t1, t3, ptbl_map_loop

	// identity map the first 4MB of the mmio space
	li t0, (0xF0000000 >> 2) | PTE_BITS_RW
	li t1, (0xF0000000 >> 20) // offset of pte
	add t1, t2, t1
	sw t0, 0(t1)

	// enable paging
	srli t0, t2, 12
	li t1, SATP_MODE_SV32
	or t0, t0, t1
	csrw satp, t0

	// flush TLB 
	sfence.vma zero,zero

	nop
	nop
	nop
	nop
	nop

	// absolute jump to kernel start
	lw t0, start_pc
	jr t0

start_sp:
	.word __memory_top
start_pc:
	.word start
