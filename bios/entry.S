// Copyright 2022, Brian Swetland <swetland@frotz.net>
// Licensed under the Apache License, Version 2.0

#include <hw/riscv.h>
#include "bios.h"

.globl mach_exception_entry
mach_exception_entry:
	j mach_exception
	j .
	j .
	j .
	j .
	j .
	j .
mach_timer_entry:
	// swap active sp with value in MSCRATCH (exception stack)
	// interupts have been disabled on exception entry
	csrrw sp, CSR_MSCRATCH, sp

	// use the workspace above the exception stack to save
	// registers and get necessary data
	sw t0, IWS_SAVE0(sp)
	sw t1, IWS_SAVE1(sp)
	sw t2, IWS_SAVE2(sp)
	sw t3, IWS_SAVE3(sp)
	sw t4, IWS_SAVE4(sp)

	lw t0, IWS_TIMECMP(sp)
	lw t1, IWS_TICKINC(sp)

	// read last match lo/hi
	lw t2, 0(t0)
	lw t3, 4(t0)

	add t1, t2, t1
	// t4 is carry bit
	sltu t4, t1, t2
	add t3, t4, t3

	// avoid spurious irq by ensuring we can't roll back
	li t4, -1
	sw t4, 0(t0)

	sw t3, 4(t0)
	sw t1, 0(t0)

	// set S MODE SW INT pending
	csrsi CSR_SIP, INTb_SVC_SW

#if 0
	li t0, 0x10000000
	li t1, '!'
	sb t1, 0(t0)
#endif

	// restore registers
	lw t0, IWS_SAVE0(sp)
	lw t1, IWS_SAVE1(sp)
	lw t2, IWS_SAVE2(sp)
	lw t3, IWS_SAVE3(sp)
	lw t4, IWS_SAVE4(sp)

	// swap back to previous sp
	csrrw sp, CSR_MSCRATCH, sp
	mret

mach_exception:
	// swap active sp with value in MSCRATCH (exception stack)
	// interupts have been disabled on exception entry
	csrrw sp, CSR_MSCRATCH, sp

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
	mv a0, sp

	// save previous pc (hw stashed it in MEPC)
	csrr t0, CSR_MEPC
	sw t0, 0x00(sp)

	// save previous sp (we stashed it in MSCRATCH)
	csrr t0, CSR_MSCRATCH
	sw t0, 0x08(sp)

	jal mach_exception_handler

	// save machine sp back into SSCRATCH
	addi t0, sp, (32 * 4)
	csrw CSR_MSCRATCH, t0

	// return pc goes into MEPC for mret
	lw t0, 0x00(sp)
	csrw CSR_MEPC, t0

	// restore remaining registers
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

	lw sp, 0x08(sp)

	mret

.globl enter_mode_s
enter_mode_s: // (a0, a1, svc_pc, svc_sp)
	csrw CSR_MEPC, a2
	mv sp, a3
	mret

