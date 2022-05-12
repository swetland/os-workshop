// Copyright 2022, Brian Swetland <swetland@frotz.net>
// Licensed under the Apache License, Version 2.0

// this code must match the structure rv32_ctxt_t in context-switch.h

// context_switch(rv32_ctxt_t* prev, rv32_ctxt_t* next)
.globl context_switch
context_switch:
	// save old context to prev
	sw ra, 0x00(a0)
	sw sp, 0x04(a0)
	sw gp, 0x08(a0)
	sw tp, 0x0C(a0)
	sw s0, 0x10(a0)
	sw s1, 0x14(a0)
	sw s2, 0x18(a0)
	sw s3, 0x1C(a0)
	sw s4, 0x20(a0)
	sw s5, 0x24(a0)
	sw s6, 0x28(a0)
	sw s7, 0x2C(a0)
	sw s8, 0x30(a0)
	sw s9, 0x34(a0)
	sw s10, 0x38(a0)
	sw s11, 0x3C(a0)

        // load new context from next
	lw ra, 0x00(a1)
	lw sp, 0x04(a1)
	lw gp, 0x08(a1)
	lw tp, 0x0C(a1)
	lw s0, 0x10(a1)
	lw s1, 0x14(a1)
	lw s2, 0x18(a1)
	lw s3, 0x1C(a1)
	lw s4, 0x20(a1)
	lw s5, 0x24(a1)
	lw s6, 0x28(a1)
	lw s7, 0x2C(a1)
	lw s8, 0x30(a1)
	lw s9, 0x34(a1)
	lw s10, 0x38(a1)
	lw s11, 0x3C(a1)

	// return to new context
	ret

// startup helper
// calls routine in s2 with s0, s1 as arguments
// then calls thread_exit() if that routine returns
.globl context_entry
context_entry:
	mv a0, s0
	mv a1, s1
	jalr s2
	j thread_exit

