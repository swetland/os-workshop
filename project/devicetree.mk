
APP_NAME := devicetree
APP_SRC := hw/src/start.S misc/devicetree.c
APP_SRC += hw/src/debug.c $(LIBC_SRC)
include make/app.mk
