// Copyright 2022, Brian Swetland <swetland@frotz.net>
// Licensed under the Apache License, Version 2.0

#include <hw/platform.h>

.section ".start"

.globl _start
_start:
	/* zero BSS */
	la t0, __bss_start
	la t1, __bss_end
zero_loop:
	beq t0, t1, zero_done
	sw zero, 0(t0)
	add t0, t0, 4
	j zero_loop
zero_done:

	/* setup initial stack */
	la sp, __memory_top

	/* enter C code */
	jal start

	/* infinite loop */
	j .
