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
static uint32_t color0(gfx_surface_t *gs, uint32_t c) { return 0; }
static void plot0(gfx_surface_t *gs, uint32_t x, uint32_t y, uint32_t pxl) {}
static void hline0(gfx_surface_t *gs, uint32_t x0, uint32_t y, uint32_t x1, uint32_t pxl) {}
static void putc0(gfx_surface_t *gs, uint32_t x, uint32_t y, uint32_t c) {}

// drawing routines for 16bpp pixfmt
static uint32_t color16(gfx_surface_t *gs, uint32_t c) {
	return ((c >> 3) & 0x1F) |
		(((c >> 10) & 0x3F) << 5) |
		(((c >> 19) & 0x1F) << 11);
}

static void plot16(gfx_surface_t *gs, uint32_t x, uint32_t y, uint32_t pxl) {
	if (unlikely((x >= gs->width) || (y >= gs->height))) {
		return;
	}	
	((uint16_t*) (gs->pixels))[x + y * gs->stride] = pxl;
}
 
static void hline16(gfx_surface_t *gs, uint32_t x0, uint32_t y, uint32_t x1, uint32_t pxl) {
	if (unlikely((x0 >= gs->width) || (y >= gs->height) || (x1 <= x0))) {
		return;
	}
	if (unlikely((x1 > gs->width))) {
		x1 = gs->width;
	}
	uint16_t *pixels = ((uint16_t*)gs->pixels) + x0 + y * gs->stride;
	uint16_t *end = pixels + x1 - x0;
	while (pixels < end) {
		*pixels++ = pxl;
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
		gs->color = color16;
		break;
	default:
		gs->plot = plot0;
		gs->hline = hline0;
		gs->putc = putc0;
		gs->color = color0;
		break;
	}
}

void gfx_init_display(gfx_surface_t *gs) {
	gs->width = 640;
	gs->height = 480;
	gs->stride = 640;
	gs->pixels = (void*) FRAMEBUFFER_BASE;
	gs->pixfmt = PIXFMT_RGB565;
	gfx_init(gs);
	gs->fgcolor = gs->color(gs, C_WHITE);
	gs->bgcolor = gs->color(gs, C_BLACK);
}

void gfx_setcolor(gfx_surface_t* gs, uint32_t fg_argb8888, uint32_t bg_argb8888) {
	gs->fgcolor = gs->color(gs, fg_argb8888);
	gs->bgcolor = gs->color(gs, bg_argb8888);
}

void gfx_fill(gfx_surface_t* gs, uint32_t x0, uint32_t y0, uint32_t x1, uint32_t y1, uint32_t pxl) {
	while (y0 < y1) {
		gs->hline(gs, x0, y0, x1, pxl);
		y0++;
	}
}

void gfx_clear(gfx_surface_t* gs, uint32_t pxl) {
	gfx_fill(gs, 0, 0, gs->width, gs->height, pxl);
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

