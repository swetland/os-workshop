
MOD_NAME := boot.net
MOD_SRC := hw/src/start.S boot/entry.S boot/netboot.c
MOD_SRC += net/ipv6.c
MOD_SRC += hw/src/print-exception.c
MOD_SRC += hw/src/debug-printf.c hw/src/debug-io.c
MOD_LIB := c
MOD_INC := tools net/inc
MOD_LDSCRIPT := make/boot.ram.ld
include make/app.mk
