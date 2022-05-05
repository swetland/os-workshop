
MOD_NAME := traps
MOD_SRC := hw/src/start.S misc/traps-entry.S misc/traps.c
MOD_SRC += hw/src/debug.c $(LIBC_SRC)
include make/app.mk
