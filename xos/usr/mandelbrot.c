// Copyright 2022, Brian Swetland <swetland@frotz.net>
// Licensed under the Apache License, Version 2.0

#include <xos/syscalls.h>
#include <hw/debug.h>
#include <gfx/gfx.h>
#include <string.h>

uint16_t colors[12] = {
	0x1111, 0x2222, 0x3333, 0x4444, 0x5555, 0x6666,
	0x7777, 0x8888, 0x9999, 0xAAAA, 0xBBBB, 0xCCCC,
};

int render(gfx_surface_t* _gs,
	uint32_t px0, uint32_t py0,
	uint32_t px1, uint32_t py1) {
	gfx_surface_t gs;
	memcpy(&gs, _gs, sizeof(gs));

	for (int py = py0; py < py1; py++) {
		int y0 = 650 - (1300 * py) / gs.height;
		for (int px = px0; px < px1; px++) {
			int x0 = -1250 + (1750 * px) / gs.width;
			int x = 0, y = 0;
			for (int i = 0; i < 500; i++) {
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
	}

	return 2;
}

#if 0
int t1(void* arg) {
	gfx_surface_t *gs = arg;
	gfx_fill(gs, 0, 0, 320, 240, gfx_color(gs, C_WHITE));
	gfx_puts(gs, 0, 240-16, "Thread One");
	return render(gs, 0, 0, 320, 240);
}
int t2(void* arg) {
	gfx_surface_t *gs = arg;
	gfx_fill(gs, 320, 240, 640, 480, gfx_color(gs, C_GRAY75));
	gfx_puts(gs, 320, 480-16, "Thread Two");
	return render(gs, 320, 240, 640, 480);
}
int t3(void* arg) {
	gfx_surface_t *gs = arg;
	gfx_fill(gs, 320, 0, 640, 240, gfx_color(gs, C_GRAY25));
	gfx_puts(gs, 320, 240-16, "Thread Three");
	return render(gs, 320, 0, 640, 240);
}
int t4(void* arg) {
	gfx_surface_t *gs = arg;
	gfx_fill(gs, 0, 240, 320, 480, gfx_color(gs, C_GRAY50));
	gfx_puts(gs, 0, 480-16, "Thread Four");
	return render(gs, 0, 240, 320, 480);
}
#endif

static gfx_surface_t screen;

void start(void) {
	xprintf("X/OS Example - Mandelbrot\n\n");

	screen.width = 640;
	screen.height = 480;
	screen.stride = 640;
	screen.pixels = (void*) 0x20000000;
	screen.pixfmt = PIXFMT_RGB565;
	screen.fgcolor = C_WHITE;
	screen.bgcolor = C_BLUE;
	gfx_init(&screen);

	gfx_clear(&screen, C_BLUE);
	render(&screen, 0, 0, 640, 480);

	exit(0);
}

