// Copyright 2022, Brian Swetland <swetland@frotz.net>
// Licensed under the Apache License, Version 2.0

#include <stdio.h>
#include <stdarg.h>

#include <gfx/gfx.h>
#include <compiler.h>
#include <hw/platform.h>

extern uint8_t vga_rom_16[256 * 16];

#define CH_ROM     vga_rom_16
#define CH_WIDTH   8
#define CH_HEIGHT  16

// drawing routines for unknown pixfmt
static void plot0(gfx_surface_t *gs, uint32_t x, uint32_t y) {}
static void hline0(gfx_surface_t *gs, uint32_t x0, uint32_t y, uint32_t x1) {}
static void putc0(gfx_surface_t *gs, uint32_t x, uint32_t y, uint32_t c) {}

// drawing routines for 16bpp pixfmt
static void plot16(gfx_surface_t *gs, uint32_t x, uint32_t y) {
	if (unlikely((x >= gs->width) || (y >= gs->height))) {
		return;
	}	
	((uint16_t*) (gs->pixels))[x + y * gs->stride] = gs->fgcolor;
}
 
static void hline16(gfx_surface_t *gs, uint32_t x0, uint32_t y, uint32_t x1) {
	if (unlikely((x0 >= gs->width) || (y >= gs->height) || (x1 <= x0))) {
		return;
	}
	if (unlikely((x1 > gs->width))) {
		x1 = gs->width;
	}
	uint16_t *pixels = gs->pixels + x0 + y * gs->stride;
	uint16_t *end = pixels + x1 - x0;
	uint32_t c = gs->fgcolor;
	while (pixels < end) {
		*pixels++ = c;
	}
}

static void putc16(gfx_surface_t *gs, uint32_t x, uint32_t y, uint32_t ch) {
	if (unlikely((x >= gs->width) || ((x + CH_WIDTH) > gs->width) ||
		(y >= gs->height) || ((y + CH_HEIGHT) > gs->height))) {
		return;
	}

	ch = (ch & 0xFF) * CH_HEIGHT;
	uint32_t fg = gs->fgcolor;
	uint32_t bg = gs->bgcolor;
	uint16_t *pixels = ((uint16_t*)gs->pixels) + x + y * gs->stride;
	uint32_t stride = gs->stride - CH_WIDTH;
	for (y = 0; y < CH_HEIGHT; y++) {
		uint32_t bits = CH_ROM[ch++];
		for (x = 0; x < CH_WIDTH; x++) {
			*pixels++ = (bits & 0x80) ? fg : bg;
			bits <<= 1;
		}
		pixels += stride;
	}
}

void gfx_init(gfx_surface_t *gs) {
	switch (gs->pixfmt) {
	case PIXFMT_RGB565:
		gs->plot = plot16;
		gs->hline = hline16;
		gs->putc = putc16;
		break;
	default:
		gs->plot = plot0;
		gs->hline = hline0;
		gs->putc = putc0;
		break;
	}
}

void gfx_init_display(gfx_surface_t *gs) {
	gs->width = 640;
	gs->height = 480;
	gs->stride = 640;
	gs->pixels = (void*) FRAMEBUFFER_BASE;
	gs->pixfmt = PIXFMT_RGB565;
	gs->fgcolor = 0xFFFF;
	gs->bgcolor = 0x0000;
	gfx_init(gs);
}

void gfx_puts(gfx_surface_t *gs, uint32_t x, uint32_t y, const char* s) {
	while (*s) {
		gs->putc(gs, x, y, *s++);
		x += CH_WIDTH;
	}
}

void gfx_printf(gfx_surface_t* gs, uint32_t x, uint32_t y, const char* fmt, ...) {
	char msg[128];
	va_list ap;
	va_start(ap, fmt);
	vsnprintf(msg, sizeof(msg), fmt, ap);
	va_end(ap);
	gfx_puts(gs, x, y, msg);
}

