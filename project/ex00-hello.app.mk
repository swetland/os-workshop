MOD_NAME := ex00-hello
MOD_SRC := hw/src/start.S hw/src/trap-entry-single-stack.S
MOD_SRC += example/ex00-hello.c
MOD_SRC += hw/src/print-exception.c hw/src/debug-printf.c hw/src/debug-io.c
MOD_LIB := c
include make/app.mk
