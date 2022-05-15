## Copyright 2022, Brian Swetland <swetland@frotz.net>
## Licensed under the Apache License, Version 2.0

all:: build-all

-include local.mk

V := @

# defaults for Ubuntu, can override with local.mk
XTOOLCHAIN ?= /usr/bin/riscv64-unknown-elf-
QEMU ?= /usr/bin/qemu-system-riscv32
QEMUBIOS := out/boot.elf

QFLAGS := -machine micro -bios $(QEMUBIOS)
QFLAGS.GDB := $(QFLAGS) -gdb tcp::7777 -S
QFLAGS.FB := -serial stdio
QFLAGS.TTY := -nographic -monitor none -serial stdio

ifeq ($(wildcard $(XTOOLCHAIN)gcc),)
$(warning Cannot find toolchain $(XTOOLCHAIN))
$(error Please set XTOOLCHAIN in local.mk)
endif

ifeq ($(wildcard $(QEMU)),)
$(warning Cannot find qemu-system-riscv32)
$(error Please set QEMU in local.mk)
endif

XGCC := $(XTOOLCHAIN)gcc
XOBJDUMP := $(XTOOLCHAIN)objdump
XOBJCOPY := $(XTOOLCHAIN)objcopy
XAR := $(XTOOLCHAIN)ar

ARCHFLAGS := -march=rv32ima -mabi=ilp32 -mcmodel=medany
ARCHFLAGS += -static -nostdlib -nostartfiles -ffreestanding
ARCHFLAGS += -ffunction-sections -fdata-sections
ARCHFLAGS += -fno-builtin -fno-strict-aliasing

LDSCRIPT := hw/app.ram.ld

BUILD := out

CFLAGS := -g -Wall -Ihw/inc
CFLAGS += -O2

ALL :=

LIBC_SRC := libc/src/printf.c $(wildcard libc/src/string/*.c)

include $(wildcard project/*.lib.mk)
include $(wildcard project/*.app.mk)

build-all: $(ALL)

clean::
	rm -rf $(BUILD)
