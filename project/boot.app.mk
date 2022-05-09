
MOD_NAME := boot
MOD_SRC := hw/src/start.S boot/entry.S boot/boot.c
MOD_SRC += hw/src/debug.c
MOD_LIB := c
MOD_LDSCRIPT := hw/boot.ram.ld
include make/app.mk
