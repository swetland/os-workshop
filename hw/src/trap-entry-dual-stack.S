// Copyright 2022, Brian Swetland <swetland@frotz.net>
// Licensed under the Apache License, Version 2.0

// This file provides assembly glue for handling traps
// (interrupts or exceptions) and safely saving and
// restoring the processor state of the code running
// when the trap happened.

// It assume a dual stack setup -- when an interrupt
// or exception occurs, a kernel stack will be used to
// save the state -- whether or not the active user
// stack is valid will not matter.

// It will call into interrupt_handler() or exception_handler()
// to process the interrupt or exception.

// For interrupts, only the registers that must be saved
// are saved (the others are callee-save so the C code
// called into will save them if necessary).

// For exceptions, ALL registers are saved so that the
// exception handler my inspect, display, or modify them
// as necesary.

// hw/context.h defines structures tframe_t and eframe_t 
// that correspond to how the registers are saved on the
// stack

.global trap_entry
trap_entry:
	// swap the user sp with the kernel sp
	// stored in SSCRATCH 
	csrrw sp, sscratch, sp

	// reserve stack space for a full eframe_t
	addi sp, sp, - (32 * 4)

	// save caller-save registers (tframe_t)
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

	// save user pc which hw stashed in SEPC
	// and user sp which we stashed in SSCRATCH
	csrr t0, sepc
	csrr t1, sscratch
	sw t0, 0x40(sp)
	sw t1, 0x44(sp)

	// if the scause high bit is clear, it is an exception
	csrr t0, scause
	bge t0, zero, exception_entry

	// otherwise it is an interrupt, call interrupt_handler()
	jal interrupt_handler

.globl trap_exit
trap_exit:
	// restore kernel sp to sscratch
	addi t0, sp, (32 * 4)
	csrw sscratch, t0

	// user sp goes into sepc for sret
	lw t0, 0x40(sp)
	csrw sepc, t0

	// restore the caller-save registers
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

	// finally restore the user sp
	lw sp, 0x44(sp)

	// and return from trap
	sret

exception_entry:
	// save the callee-save registers (cframe_t)
	// user pc and sp already stored at 0x40(sp), 0x44(sp)
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

	// call exception_handler() passing eframe_t*
	// as the first argument
	mv a0, sp
	jal exception_handler

.globl exception_exit
exception_exit:
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

	j trap_exit

.globl user_mode_entry
user_mode_entry:
	mv sp, a0
	j exception_exit
