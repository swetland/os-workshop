// Copyright 2022, Brian Swetland <swetland@frotz.net>
// Licensed under the Apache License, Version 2.0

#include <string.h>
#include <stdint.h>

#include <hw/debug.h>
#include <hw/platform.h>
#include <hw/litex.h>

#define FB_WIDTH 640
#define FB_HEIGHT 480

uint16_t colors[12] = {
	0x1111, 0x2222, 0x3333, 0x4444,
	0x5555, 0x6666, 0x7777, 0x8888,
	0x9999, 0xAAAA, 0xBBBB, 0xCCCC,
};

static inline void plot(unsigned px, unsigned py, unsigned c) {
	((uint16_t volatile*) FRAMEBUFFER_BASE)[px + py * FB_WIDTH] = c;
}

void start(void) {
	xputs("Hello, Mandelbrot!\n");

	memset((void*) FRAMEBUFFER_BASE, 0, 640 * 480 * 2);

	for (int py = 0; py < FB_HEIGHT; py++) {
		int y0 = 1300 - (2600 * py) / FB_HEIGHT;
		for (int px = 0; px < FB_WIDTH; px++) {
			int x0 = -2500 + (3500 * px) / FB_WIDTH;
			int x = 0, y = 0;
			for (int i = 0; i < 1000; i++) {
				int x2 = x * x / 1000;
				int y2 = y * y / 1000;
				if ((x2 + y2) > 4000) {
					plot(px, py, colors[(i > 11) ? 11 : i]);
					goto done;
				}
				y = 2 * x * y / 1000 + y0;
				x = x2 - y2 + x0;
			}
			plot(px, py, 0);
		done:
			;
		}
	}
}
