// Copyright 2022, Brian Swetland <swetland@frotz.net>
// Licensed under the Apache License, Version 2.0

#include <hw/riscv.h>
#include <xos/status.h>

.global trap_entry
trap_entry:
	// swap the user sp with the kernel sp
	// stored in SSCRATCH 
	csrrw sp, sscratch, sp

	// reserve stack space for a full eframe_t
	addi sp, sp, - (32 * 4)

	// save caller-save registers
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
	csrr t1, sepc
	csrr t2, sscratch
	sw t1, 0x40(sp)
	sw t2, 0x44(sp)

	// save callee-save registers
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

	// setup kernel thread pointer
	// thread struct is at top of kstack
	addi tp, sp, (32 * 4)

	// if the scause high bit is clear, it is an exception
	csrr t1, scause
	bge t1, zero, exception_entry

	// otherwise it is an interrupt, call interrupt_handler()
	jal interrupt_handler

.globl trap_exit
trap_exit:
	// TODO: check tp->reschedule flag

	// restore initial kernel sp to sscratch
	// (which is the same as the kernel tp)
	csrw sscratch, tp

	// restore callee-save registers
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

	// user pc goes into sepc for sret
	lw t1, 0x40(sp)
	csrw sepc, t1

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
	// if scause is ECALL_UMODE, it is a system call
	addi t2, zero, EXCn_ECALL_UMODE
	andi t3, t1, 0xFF
	beq t2, t3, syscall_entry
 
	mv a0, sp
	la ra, trap_exit
	j exception_handler

syscall_table:
#define __SYSCALL(n,rtype,name,args) .word sys_##name
#include <xos/syscall-tmpl.h>
syscall_table_end:

syscall_entry:
	li t1, SYSCALL_COUNT
	bgeu t0, t1, illegal_syscall

	// fn = [syscall_table + (syscall_number << 2)]
	slli t0, t0, 2
	la t1, syscall_table
	add t2, t1, t0
	lw t3, 0(t2)

	jalr t3

	// result -> user.a0
	sw a0, 0x20(sp)
	// user.pc += 4
	lw t0, 0x40(sp)
	addi t0, t0, 4
	sw t0, 0x40(sp)

	j trap_exit

illegal_syscall:
	addi t0, zero, XOS_ERR_BAD_SYSCALL
	sw t0, 0x20(sp)
	j trap_exit

.globl user_mode_entry
user_mode_entry:
	mv sp, a0
	addi tp, sp, (32 * 4)
	j trap_exit
