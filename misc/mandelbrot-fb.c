// Copyright 2022, Brian Swetland <swetland@frotz.net>
// Licensed under the Apache License, Version 2.0

#include <string.h>
#include <stdint.h>

#include <hw/debug.h>
#include <hw/platform.h>
#include <hw/litex.h>
#include <gfx/gfx.h>

#define FB_WIDTH 640
#define FB_HEIGHT 480

uint16_t colors[12] = {
	0x1111, 0x2222, 0x3333, 0x4444,
	0x5555, 0x6666, 0x7777, 0x8888,
	0x9999, 0xAAAA, 0xBBBB, 0xCCCC,
};

void start(void) {
	xputs("Hello, Mandelbrot!\n");

	gfx_surface_t gs;
	gfx_init_display(&gs);

	memset((void*) gs.pixels, 0, gs.width * gs.height * 2);

	gfx_puts(&gs, 0, gs.height - 17, "Hello, Mandelbrot!");

	for (int py = 0; py < FB_HEIGHT; py++) {
		int y0 = 1300 - (2600 * py) / FB_HEIGHT;
		for (int px = 0; px < FB_WIDTH; px++) {
			int x0 = -2500 + (3500 * px) / FB_WIDTH;
			int x = 0, y = 0;
			for (int i = 0; i < 1000; i++) {
				int x2 = x * x / 1000;
				int y2 = y * y / 1000;
				if ((x2 + y2) > 4000) {
					gs.fgcolor = colors[(i > 11) ? 11 : i];
					gfx_plot(&gs, px, py);
					goto done;
				}
				y = 2 * x * y / 1000 + y0;
				x = x2 - y2 + x0;
			}
			gs.fgcolor = 0;
			gfx_plot(&gs, px, py);
		done:
			;
		}
		gs.fgcolor = 0xFFFF;
	}

	gfx_puts(&gs, 0, gs.height - 16, "Hello, Mandelbrot!");
}
