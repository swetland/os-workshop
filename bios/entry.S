// Copyright 2022, Brian Swetland <swetland@frotz.net>
// Licensed under the Apache License, Version 2.0

#include <hw/riscv.h>

.globl mach_exception_entry
mach_exception_entry:
	// swap active sp with value in MSCRATCH (exception stack)
	// interupts have been disabled on exception entry
	csrrw sp, CSR_MSCRATCH, sp

	// save previous registers to stack
	addi sp, sp, - 32 * 4
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
	mv a0, sp

	// save previous pc (hw stashed it in MEPC)
	csrr t0, CSR_MEPC
	sw t0, 0x00(sp)

	// save previous sp (we stashed it in MSCRATCH)
	csrr t0, CSR_MSCRATCH
	sw t0, 0x08(sp)

	j mach_exception_handler
	j .

.globl enter_mode_u
enter_mode_u: // (a0, a1, user_pc, user_sp)
	csrw CSR_SEPC, a2
	mv sp, a3
	sret

.globl enter_mode_s
enter_mode_s: // (a0, a1, svc_pc, svc_sp)
	csrw CSR_MEPC, a2
	mv sp, a3
	mret

