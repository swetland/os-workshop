// Copyright 2022, Brian Swetland <swetland@frotz.net>
// Licensed under the Apache License, Version 2.0

#include <stdio.h>
#include <stdarg.h>

#include <gfx/gfx.h>
#include <compiler.h>
#include <hw/platform.h>

#define CH_WIDTH 8
#define CH_HEIGHT 16

void txt_init(txt_surface_t *ts, gfx_surface_t *gs) {
	ts->gs = gs;
	ts->cw = gs->width / CH_WIDTH;
	ts->ch = gs->height / CH_HEIGHT;
	ts->cx = 0;
	ts->cy = 0;
};

void txt_goto(txt_surface_t *ts, uint32_t cx, uint32_t cy) {
	if (cx >= ts->cw) {
		cx = ts->cw - 1;
	}
	if (cy >= ts->ch) {
		cx = ts->ch - 1;
	}
	ts->cx = cx;
	ts->cy = cy;
}

void txt_putc(txt_surface_t *ts, uint32_t ch) {
	if ((ch == '\r') || (ch == '\n')) {
newline:
		ts->cx = 0;
		ts->cy++;
		if (ts->cy >= ts->ch) {
			ts->cy = 0;
		}
		return;
	}
	gfx_putc(ts->gs, ts->cx * CH_WIDTH, ts->cy * CH_HEIGHT, ch);
	ts->cx++;
	if (ts->cx >= ts->cw) {
		goto newline;
	}
}

void txt_puts(txt_surface_t *ts, const char *s) {
	while (*s) {
		txt_putc(ts, *s++);
	}
}

void txt_printf(txt_surface_t *ts, const char *fmt, ...) {
	char msg[128];
	va_list ap;
	va_start(ap, fmt);
	vsnprintf(msg, sizeof(msg), fmt, ap);
	va_end(ap);
	txt_puts(ts, msg);
}
