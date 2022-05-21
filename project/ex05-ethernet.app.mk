MOD_NAME := ex05-ethernet
MOD_SRC := hw/src/start.S hw/src/trap-entry-single-stack.S
MOD_SRC += example/ex05-ethernet.c
MOD_SRC += hw/src/print-exception.c hw/src/debug-printf.c hw/src/debug-io.c
MOD_SRC += net/ipv6.c
MOD_INC := net/inc
MOD_LIB := c
include make/app.mk
