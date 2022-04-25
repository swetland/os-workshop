// Copyright 2022, Brian Swetland <swetland@frotz.net>
// Licensed under the Apache License, Version 2.0

#include <hw/riscv.h>

.globl svc_exception_entry
svc_exception_entry:
	// swap active sp with value in SSCRATCH (svc stack)
	// interupts have been disabled on exception entry
	csrrw sp, CSR_SSCRATCH, sp

	// todo: beqz svc_nested_irq_entry

	// save previous registers to stack
	addi sp, sp, - (32 * 4)
	sw x1, 0x04(sp)
	sw x3, 0x0C(sp)
	sw x4, 0x10(sp)
	sw x5, 0x14(sp)
	sw x6, 0x18(sp)
	sw x7, 0x1C(sp)
	sw x8, 0x20(sp)
	sw x9, 0x24(sp)
	sw x10, 0x28(sp)
	sw x11, 0x2C(sp)
	sw x12, 0x30(sp)
	sw x13, 0x34(sp)
	sw x14, 0x38(sp)
	sw x15, 0x3C(sp)
	sw x16, 0x40(sp)
	sw x17, 0x44(sp)
	sw x18, 0x48(sp)
	sw x19, 0x4C(sp)
	sw x20, 0x50(sp)
	sw x21, 0x54(sp)
	sw x22, 0x58(sp)
	sw x23, 0x5C(sp)
	sw x24, 0x60(sp)
	sw x25, 0x64(sp)
	sw x26, 0x68(sp)
	sw x27, 0x6C(sp)
	sw x28, 0x70(sp)
	sw x29, 0x74(sp)
	sw x30, 0x78(sp)
	sw x31, 0x7C(sp)

	// save user pc (hw stashed it in SEPC)
	csrr t0, CSR_SEPC
	sw t0, 0x00(sp)

	// save user sp (we stashed it in SSCRATCH)
	// and stick 0 in CSR_SSCRATCH so we can detect nested irqs
	csrrw t0, CSR_SSCRATCH, x0
	sw t0, 0x08(sp)

	mv a0, sp
	jal svc_exception_handler

	// save svc sp in SSCRATCH for next interrupt
	addi t0, sp, (32 * 4)
	csrw CSR_SSCRATCH, t0

	// user pc goes into SEPC for sret
	lw t0, 0x00(sp)
	csrw CSR_SEPC, t0

	lw x1, 0x04(sp)
	lw x3, 0x0C(sp)
	lw x4, 0x10(sp)
	lw x5, 0x14(sp)
	lw x6, 0x18(sp)
	lw x7, 0x1C(sp)
	lw x8, 0x20(sp)
	lw x9, 0x24(sp)
	lw x10, 0x28(sp)
	lw x11, 0x2C(sp)
	lw x12, 0x30(sp)
	lw x13, 0x34(sp)
	lw x14, 0x38(sp)
	lw x15, 0x3C(sp)
	lw x16, 0x40(sp)
	lw x17, 0x44(sp)
	lw x18, 0x48(sp)
	lw x19, 0x4C(sp)
	lw x20, 0x50(sp)
	lw x21, 0x54(sp)
	lw x22, 0x58(sp)
	lw x23, 0x5C(sp)
	lw x24, 0x60(sp)
	lw x25, 0x64(sp)
	lw x26, 0x68(sp)
	lw x27, 0x6C(sp)
	lw x28, 0x70(sp)
	lw x29, 0x74(sp)
	lw x30, 0x78(sp)
	lw x31, 0x7C(sp)

	// grab user sp back off of the svc stack
	lw sp, 0x08(sp)

	// save previous pc (hw stashed it in MEPC)
	sret

.globl enter_mode_u
enter_mode_u: // (a0, a1, user_pc, user_sp)
	csrw CSR_SEPC, a2
	mv sp, a3
	sret

