
APP_NAME := mandelbrot
APP_SRC := hw/src/start.S misc/mandelbrot.c
APP_SRC += hw/src/debug.c $(LIBC_SRC)
include make/app.mk
