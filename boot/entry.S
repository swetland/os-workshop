// Copyright 2022, Brian Swetland <swetland@frotz.net>
// Licensed under the Apache License, Version 2.0

#include <hw/riscv.h>

// This exception handler is conservative and stores *all*
// the general registers on the stack on exception entry.
//
// This allows the exception handler to easily inspect and
// modify the previous register state.
//
// A more efficient handler could only store the callee-save
// registers (that would otherwise be possibly clobbered by
// the C code it calls into), and trust that the others would
// be saved and restored as needed, further down the call chain.

.globl mach_exception_entry
mach_exception_entry:
	// swap active sp with value in MSCRATCH (exception stack)
	// interupts have been disabled on exception entry
	csrrw sp, mscratch, sp

	// allocate space for eframe_t
	addi sp, sp, - (32 * 4)

	// save the tframe_t
	sw ra, 0x00(sp)
	sw t0, 0x04(sp)
	sw t1, 0x08(sp)
	sw t2, 0x0C(sp)
	sw t3, 0x10(sp)
	sw t4, 0x14(sp)
	sw t5, 0x18(sp)
	sw t6, 0x1C(sp)
	sw a0, 0x20(sp)
	sw a1, 0x24(sp)
	sw a2, 0x28(sp)
	sw a3, 0x2C(sp)
	sw a4, 0x30(sp)
	sw a5, 0x34(sp)
	sw a6, 0x38(sp)
	sw a7, 0x3C(sp)

	// save the cframe_t
	csrr t0, mepc
	csrr t1, mscratch
	sw t0, 0x40(sp) // pc
	sw t1, 0x44(sp) // sp
	sw gp, 0x48(sp)
	sw tp, 0x4C(sp)
	sw s0, 0x50(sp)
	sw s1, 0x54(sp)
	sw s2, 0x58(sp)
	sw s3, 0x5C(sp)
	sw s4, 0x60(sp)
	sw s5, 0x64(sp)
	sw s6, 0x68(sp)
	sw s7, 0x6C(sp)
	sw s8, 0x70(sp)
	sw s9, 0x74(sp)
	sw s10, 0x78(sp)
	sw s11, 0x7C(sp)

	// call into C handler
	mv a0, sp
	jal mach_exception_handler

	// restore trap sp to mscratch
	addi t0, sp, (32 * 4)
	csrw mscratch, t0

	// return pc goes into mepc for mret
	lw t1, 0x40(sp) // pc
	csrw mepc, t1

	// restore registers from cframe_t
	lw gp, 0x48(sp)
	lw tp, 0x4C(sp)
	lw s0, 0x50(sp)
	lw s1, 0x54(sp)
	lw s2, 0x58(sp)
	lw s3, 0x5C(sp)
	lw s4, 0x60(sp)
	lw s5, 0x64(sp)
	lw s6, 0x68(sp)
	lw s7, 0x6C(sp)
	lw s8, 0x70(sp)
	lw s9, 0x74(sp)
	lw s10, 0x78(sp)
	lw s11, 0x7C(sp)

	// restore registers from tframe_t
	lw ra, 0x00(sp)
	lw t0, 0x04(sp)
	lw t1, 0x08(sp)
	lw t2, 0x0C(sp)
	lw t3, 0x10(sp)
	lw t4, 0x14(sp)
	lw t5, 0x18(sp)
	lw t6, 0x1C(sp)
	lw a0, 0x20(sp)
	lw a1, 0x24(sp)
	lw a2, 0x28(sp)
	lw a3, 0x2C(sp)
	lw a4, 0x30(sp)
	lw a5, 0x34(sp)
	lw a6, 0x38(sp)
	lw a7, 0x3C(sp)

	// restore previous sp last
	lw sp, 0x44(sp)

	// return from exception
	mret

.globl exit_mode_m // a0, a1, pc, sp
exit_mode_m:
	csrw mepc, a2
	mv sp, a3
	mret
