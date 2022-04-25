
APP_NAME := traps
APP_SRC := hw/src/start.S misc/traps-entry.S misc/traps.c
APP_SRC += hw/src/debug.c $(LIBC_SRC)
include make/app.mk
