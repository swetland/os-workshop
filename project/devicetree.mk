
APP := devicetree
SRC := hw/src/start.S misc/devicetree.c
SRC += hw/src/debug.c $(LIBC_SRC)
include make/app.mk
