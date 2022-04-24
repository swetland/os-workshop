// Copyright 2022, Brian Swetland <swetland@frotz.net>
// Licensed under the Apache License, Version 2.0

.globl _start
_start:
	li sp, 0x81000000
	jal start

.globl exit
exit:
	j .
