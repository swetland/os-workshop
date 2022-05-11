
MOD_NAME := info
MOD_SRC := hw/src/start.S misc/info.c
MOD_SRC += hw/src/debug-printf.c hw/src/debug-io.c
MOD_LIB := c
include make/app.mk
