// Copyright 2022, Brian Swetland <swetland@frotz.net>
// Licensed under the Apache License, Version 2.0


// trap-safe memory accesses

.globl rd32safe // (addr)
rd32safe:
	la t0, rd32fail
	mv t1, a0
	li a0, 0
	csrw mtvec, t0
	lw a0, 0(t1)
rd32fail:
	ret

.globl rd8safe // (addr)
rd8safe:
	la t0, rd8fail
	mv t1, a0
	li a0, 0
	csrw mtvec, t0
	lbu a0, 0(t1)
rd8fail:
	ret


.globl console_call_cmd // (fn, a1, a2, a3, a4, a0)
console_call_cmd:
	mv t0, a0
	mv a0, a5
	jr t0
