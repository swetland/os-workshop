
MOD_NAME := mandelbrot-fb
MOD_SRC := hw/src/start.S misc/mandelbrot-fb.c
MOD_SRC += hw/src/debug.c
MOD_QEMU_FB := 1
MOD_LIB := c
include make/app.mk
