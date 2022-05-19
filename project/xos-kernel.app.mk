
MOD_NAME := xos-kernel
MOD_SRC := xos/src/start.S xos/src/entry.S
MOD_SRC += xos/src/kernel.c xos/src/vmm.c
MOD_SRC += hw/src/context-switch.S
MOD_SRC += hw/src/debug-printf.c hw/src/debug-io.c hw/src/print-exception.c
MOD_LIB := c
MOD_INC := xos/inc
MOD_EXT := out/xos-mandelbrot.bin
MOD_LDSCRIPT := make/kernel.ram.ld
MOD_QEMU_FB := 1
include make/app.mk
