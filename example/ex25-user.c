// Copyright 2022, Brian Swetland <swetland@frotz.net>
// Licensed under the Apache License, Version 2.0

#include <hw/debug.h>

void xputs(const char* s) {
	while (*s) {
		xputc(*s++);
	}
}

void start(void) {
	xprintf("Hello, User Mode!\n");
	exit(42);
}
