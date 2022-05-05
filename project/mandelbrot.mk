
MOD_NAME := mandelbrot
MOD_SRC := hw/src/start.S misc/mandelbrot.c
MOD_SRC += hw/src/debug.c $(LIBC_SRC)
include make/app.mk
