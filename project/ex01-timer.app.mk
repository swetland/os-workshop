MOD_NAME := ex01-timer
MOD_SRC := hw/src/start.S hw/src/trap-entry-single-stack.S
MOD_SRC += example/ex01-timer.c
MOD_SRC += hw/src/print-exception.c hw/src/debug-printf.c hw/src/debug-io.c
MOD_LIB := c
include make/app.mk
