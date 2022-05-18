
MOD_NAME := boot
MOD_SRC := hw/src/start.S boot/entry.S boot/boot.c
MOD_SRC += boot/console.c boot/helpers.S
MOD_SRC += hw/src/print-exception.c
MOD_SRC += hw/src/debug-printf.c hw/src/debug-io.c
MOD_LIB := c
MOD_LDSCRIPT := make/boot.ram.ld
include make/app.mk
