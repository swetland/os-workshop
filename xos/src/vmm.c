// Copyright 2022, Brian Swetland <swetland@frotz.net>
// Licensed under the Apache License, Version 2.0

#include "kernel.h"

#include <string.h>

static paddr_t pa_top;
static paddr_t pa_bottom;

void vmm_init(paddr_t start, paddr_t end) {
	pa_bottom = start;
	pa_top = end;
}

paddr_t ppage_alloc_z(void) {
	if (pa_top == pa_bottom) {
		panic("out of physical pages");
	}
	pa_top -= PAGE_SIZE;
	memset(pa_to_kva(pa_top), 0, PAGE_SIZE);
	return pa_top;
}

void *kpage_alloc_z(void) {
	return pa_to_kva(ppage_alloc_z());
}

#define PTE_PA(pte) (((pte) & 0xFFFFFC00) << 2)

status_t vm_map_4k(uint32_t* pdir, vaddr_t va, paddr_t pa, uint32_t flags) {
	uint32_t idx1 = va >> 22;
	uint32_t idx0 = (va >> 12) & 0x3FF;
	uint32_t *ptbl;

	if ((va & 0x00000FFF) || (pa & 0x00000FFF)) {
		return XOS_ERR_BAD_PARAM;
	}

	uint32_t pte = pdir[idx1];
	if (pte & PTE_V) { // valid pgdir entry
		if (pte & (PTE_X|PTE_W|PTE_R)) { 
			return XOS_ERR_CONFLICT;
		}
		ptbl = pa_to_kva(PTE_PA(pte));
	} else { // no entry, allocate a pagetable
		uint32_t ptbl_pa = ppage_alloc_z();
		ptbl = pa_to_kva(ptbl_pa);
		pdir[idx1] = (ptbl_pa >> 2) | PTE_V;
	}

	pte = ptbl[idx0];
	if (pte & PTE_V) {
		return -1; // 4KB page already here
	}

	ptbl[idx0] = ((pa & 0xFFFFF000) >> 2) | (flags & 0x3FF);

	tlb_flush_all();
	return XOS_OK;
}

status_t vm_map_4m(uint32_t* pdir, vaddr_t va, paddr_t pa, uint32_t flags) {
	uint32_t idx1 = va >> 22;

	if ((va & 0x003FFFFF) || (pa & 0x003FFFFF)) {
		return XOS_ERR_BAD_PARAM;
	}

	uint32_t pte = pdir[idx1];
	if (pte & PTE_V) { // valid pgdir entry
		return XOS_ERR_CONFLICT;
	}
	pdir[idx1] = ((pa & 0xFFFFF000) >> 2) | (flags & 0x3FF);

	tlb_flush_all();
	return XOS_OK;
}


