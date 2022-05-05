
MOD_NAME := devicetree
MOD_SRC := hw/src/start.S misc/devicetree.c
MOD_SRC += hw/src/debug.c $(LIBC_SRC)
include make/app.mk
