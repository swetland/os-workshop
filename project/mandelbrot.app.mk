
MOD_NAME := mandelbrot
MOD_SRC := hw/src/start.S misc/mandelbrot.c
MOD_SRC += hw/src/debug.c
MOD_LIB := c
include make/app.mk
