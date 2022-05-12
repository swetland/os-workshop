// Copyright 2022, Brian Swetland <swetland@frotz.net>
// Licensed under the Apache License, Version 2.0

// inline assembly helpers for use from C

// note: "volatile" instructs GCC to not discard the assmebly
// or move it outside of loops, etc, even if the optimizer
// thinks it's safe to do so. 

#pragma once

#include <stdint.h>
#include <stddef.h>

#include <hw/riscv.h>

typedef uint32_t irqstate_t;

#define __S(x) #x

// CSR Accessors

#define csr_read(csr) ({ \
	uint32_t v; asm volatile ("csrr %0, " __S(csr) : "=r"(v)); v; })

#define csr_write(csr, val) ({ \
	asm volatile ("csrw " __S(csr) ", %0" :: "rK"(val)); });

#define csr_swap(csr, val) ({ \
	uint32_t v; asm volatile ("csrrw %0, " __S(csr) ", %1" : "=r"(v) : "rK"(val)); v; })

#define csr_set(csr, bit) ({ \
	asm volatile ("csrrs x0, " __S(csr) ", %0" :: "rK"(bit)); })

#define csr_clr(csr, bit) ({ \
	asm volatile ("csrrc x0, " __S(csr) ", %0" :: "rK"(bit)); })

// Disable Interrupts
static inline void irq_disable(void) {
	asm volatile("csrrc x0, sstatus, %0" :: "i"(SSTATUS_SIE));
}

// Enable Interrupts
static inline void irq_enable(void) {
	asm volatile("csrrs x0, sstatus, %0" :: "i"(SSTATUS_SIE));
}

// Disable Interrupts and return the previous interrupt state
static inline irqstate_t irq_disable_save(void) {
	uint32_t v; asm volatile("csrrc %0, sstatus, %1" : "=r"(v) : "i"(SSTATUS_SIE));
	return v & SSTATUS_SIE;
}

// Restore the previous interrupt state
static inline void irq_restore(irqstate_t state) {
	if (state) {
		asm volatile("csrrs x0, sstatus, %0" :: "i"(SSTATUS_SIE));
	}
}

// Thread Pointer Accessors

static inline void* threadptr_get(void) {
	void *ptr; asm volatile ("mv %0, tp" : "=r"(ptr)); return ptr;
}

static inline void threadptr_set(void* ptr) {
	asm volatile ("mv tp, %0" :: "r"(ptr));
}

#define threadptr_getf(type, field) ({\
	type T; typeof(T.field) v; asm volatile ("lw %0, %1(tp)" : "=r"(v) : "i"(offsetof(type,field))); v; })

#define threadptr_setf(type, field, val) ({\
	asm volatile ("sw %0, %1(tp)" :: "r"(val), "i"(offsetof(type,field))); })


// TLB Cache Managemant

static inline void tlb_flush_all(void) {
	asm volatile("sfence.vma zero, zero");
}

static inline void tlb_fush_addr(uint32_t a) {
	asm volatile("sfence.vma %0, zero" :: "r"(a));
}

// sfence.vma rs2, rs1  (asid, addr)
// sfence.vma x0, x0    orders all r/w to any level page table
//                      invalidates all TLB entries
// sfence.vma x1, x0    orders all r/w to any level page table for ASID x1
//                      invalidates all non-global ASID x1 TLB entries
// sfence.vma x0, x2    orders all r/w to leaf PTEs for vaddr in x2
//                      invalidates TLB entries for vaddr x2
// sfence.vma x1, x2    as last but for ASID x1


