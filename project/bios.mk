
APP_NAME := bios
APP_SRC := hw/src/start.S bios/entry.S bios/bios.c
APP_SRC += hw/src/debug.c $(LIBC_SRC)
APP_LDSCRIPT := hw/bios.ld
include make/app.mk
