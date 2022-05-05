
MOD_NAME := info
MOD_SRC := hw/src/start.S misc/info.c
MOD_SRC += hw/src/debug.c $(LIBC_SRC)
include make/app.mk
