
MOD_NAME := xos-mandelbrot
MOD_SRC := hw/src/start.S xos/user/mandelbrot.c
MOD_LIB := gfx xos c
MOD_LDSCRIPT := make/user.ram.ld

# TODO: fix this
MOD_INC := xos/inc

include make/app.mk
