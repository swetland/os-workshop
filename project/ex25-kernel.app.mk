
MOD_NAME := ex25-kernel
MOD_SRC := hw/src/start.mmu.S hw/src/trap-entry-dual-stack.S
MOD_SRC += example/ex25-kernel.c
MOD_SRC += hw/src/context-switch.S
MOD_SRC += hw/src/debug-printf.c hw/src/debug-io.c hw/src/print-exception.c
MOD_LIB := c
MOD_EXT := out/ex25-user.bin
MOD_LDSCRIPT := make/kernel.ram.ld
include make/app.mk
