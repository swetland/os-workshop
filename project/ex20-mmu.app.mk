
MOD_NAME := ex20-mmu
MOD_SRC := hw/src/start.S hw/src/trap-entry-single-stack.S
MOD_SRC += example/ex20-mmu.c
MOD_SRC += hw/src/context-switch.S
MOD_SRC += hw/src/debug-printf.c hw/src/debug-io.c hw/src/print-exception.c
MOD_LIB := c
include make/app.mk
