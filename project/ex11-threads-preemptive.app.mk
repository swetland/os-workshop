
MOD_NAME := ex11-threads-preemptive
MOD_SRC := hw/src/start.S hw/src/trap-entry-single-stack.S
MOD_SRC += example/ex11-threads-preemptive.c
MOD_SRC += hw/src/context-switch.S
MOD_SRC += hw/src/debug-printf.c hw/src/debug-io.c hw/src/print-exception.c
MOD_LIB := c gfx
MOD_QEMU_FB := 1
include make/app.mk
