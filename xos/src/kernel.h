// Copyright 2022, Brian Swetland <swetland@frotz.net>
// Licensed under the Apache License, Version 2.0

#pragma once

#include <xos/status.h>
#include <xos/types.h>

#include <hw/riscv.h>
#include <hw/context.h>
#include <hw/intrinsics.h>

#define PAGE_SIZE 4096

#define KVA_BASE 0xC0000000
#define KPA_BASE 0x40000000

void panic(const char* msg);

// vm.c
static inline void* pa_to_kva(paddr_t pa) {
	return (void*) (pa + (KVA_BASE - KPA_BASE));
}
static inline paddr_t kva_to_pa(void *kva) {
	return ((paddr_t) kva) - (KVA_BASE - KPA_BASE);
}

// allocate a (zero'd) physical page, return ppa
paddr_t ppage_alloc_z(void);

// allocate a (zero'd) physical page, return kva
void *kpage_alloc_z(void);

void vmm_init(paddr_t lo, paddr_t hi);
status_t vm_map_4k(uint32_t *pdir, vaddr_t va, paddr_t pa, uint32_t attrs);
status_t vm_map_4m(uint32_t *pdir, vaddr_t va, paddr_t pa, uint32_t attrs);

#define USER_RW (PTE_D|PTE_A|PTE_U|PTE_R|PTE_W|PTE_V)
#define USER_RX (PTE_D|PTE_A|PTE_U|PTE_R|PTE_X|PTE_V)
#define USER_RWX (PTE_D|PTE_A|PTE_U|PTE_R|PTE_W|PTE_X|PTE_V)

#define KERNEL_RO (PTE_D|PTE_A|PTE_R|PTE_V)
#define KERNEL_RW (PTE_D|PTE_A|PTE_R|PTE_W|PTE_V)
#define KERNEL_RX (PTE_D|PTE_A|PTE_R|PTE_X|PTE_V)

// thread.c
typedef struct {
	uint32_t magic;
	uint32_t id;
	uint32_t *pgdir;
	uint32_t reschedule;

	uint32_t reserved0;
	uint32_t reserved1;
	uint32_t reserved2;
	uint32_t reserved3;
} thread_t;

// syscall entry points
#define __SYSCALL(n,rtype,name,args) rtype sys_##name args;
#include <xos/syscall-tmpl.h>
