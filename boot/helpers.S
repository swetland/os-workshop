// Copyright 2022, Brian Swetland <swetland@frotz.net>
// Licensed under the Apache License, Version 2.0


// trap-safe memory accesses

.globl rd32safe // (addr)
rd32safe:
	csrr t0, mtvec
	csrr t1, mstatus
	la t2, rd32fail
	mv t3, a0
	li a0, 0
	csrw mtvec, t2
	lw a0, 0(t3)
rd32fail:
	csrw mtvec, t0
	csrw mstatus, t1
	ret

.globl rd8safe // (addr)
rd8safe:
	csrr t0, mtvec
	csrr t1, mstatus
	la t2, rd8fail
	mv t3, a0
	li a0, 0
	csrw mtvec, t2
	lbu a0, 0(t3)
rd8fail:
	csrw mtvec, t0
	csrw mstatus, t1
	ret


.globl console_call_cmd // (fn, a1, a2, a3, a4, a0)
console_call_cmd:
	mv t0, a0
	mv a0, a5
	jr t0
