// Copyright 2022, Brian Swetland <swetland@frotz.net>
// Licensed under the Apache License, Version 2.0

#include <hw/platform.h>

.globl _start
_start:
	li sp, (DRAM_BASE + DRAM_SIZE)
	jal start

.globl exit
exit:
	j .
