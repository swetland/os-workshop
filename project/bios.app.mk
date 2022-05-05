
MOD_NAME := bios
MOD_SRC := hw/src/start.S bios/entry.S bios/bios.c
MOD_SRC += hw/src/debug.c
MOD_LIB := c
MOD_LDSCRIPT := hw/bios.ld
include make/app.mk
