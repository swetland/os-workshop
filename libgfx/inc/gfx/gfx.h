// Copyright 2022, Brian Swetland <swetland@frotz.net>
// Licensed under the Apache License, Version 2.0

#pragma once

#include <stdint.h>

typedef struct gfx_surface gfx_surface_t;

struct gfx_surface {
	uint32_t width;
	uint32_t height;
	uint32_t stride;
	void *pixels;

	uint32_t pixfmt;
	uint32_t fgcolor;
	uint32_t bgcolor;
	uint32_t reserved;

	void (*plot)(gfx_surface_t* gs, uint32_t x, uint32_t y);
	void (*hline)(gfx_surface_t* gs, uint32_t x0, uint32_t y, uint32_t x1);
	void (*putc)(gfx_surface_t* gs, uint32_t x, uint32_t y, uint32_t ch);
};

#define PIXFMT_RGB565 0x03000565

void gfx_init_display(gfx_surface_t *gs);
void gfx_init(gfx_surface_t* gs);

static inline void gfx_plot(gfx_surface_t* gs, uint32_t x, uint32_t y) {
	gs->plot(gs, x, y);
}

static inline void gfx_hline(gfx_surface_t* gs, uint32_t x0, uint32_t y, uint32_t x1) {
	gs->hline(gs, x0, y, x1);
}

static inline void gfx_putc(gfx_surface_t* gs, uint32_t x, uint32_t y, uint32_t ch) {
	gs->putc(gs, x, y, ch);
}

void gfx_puts(gfx_surface_t* gs, uint32_t x, uint32_t y, const char* s);
void gfx_printf(gfx_surface_t* gs, uint32_t x, uint32_t y, const char* fmt, ...);
