
APP := mandelbrot
SRC := hw/src/start.S misc/mandelbrot.c
SRC += hw/src/debug.c $(LIBC_SRC)
include make/app.mk
