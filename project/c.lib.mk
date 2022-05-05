
MOD_NAME := c
MOD_SRC := libc/src/printf.c $(wildcard libc/src/string/*.c)
include make/lib.mk
