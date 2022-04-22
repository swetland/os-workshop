
APP := info
SRC := hw/src/start.S misc/info.c
SRC += hw/src/debug.c $(LIBC_SRC)
include make/app.mk
