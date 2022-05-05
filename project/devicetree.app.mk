
MOD_NAME := devicetree
MOD_SRC := hw/src/start.S misc/devicetree.c
MOD_SRC += hw/src/debug.c
MOD_LIB := c
include make/app.mk
