// Copyright 2022, Brian Swetland <swetland@frotz.net>
// Licensed under the Apache License, Version 2.0

.globl _start
_start:
	li sp, 0x81000000
	li a0, 0
	li a1, 0
	jal main

.globl exit
exit:
	j .
