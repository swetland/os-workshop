
MOD_NAME := ex25-user
MOD_SRC := hw/src/start.S
MOD_SRC += example/ex25-user.c example/ex25-syscalls.S
MOD_SRC += hw/src/debug-printf.c
MOD_LIB := c
MOD_LDSCRIPT := make/user.ram.ld
include make/app.mk
