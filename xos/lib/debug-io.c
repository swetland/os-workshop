// Copyright 2022, Brian Swetland <swetland@frotz.net>
// Licensed under the Apache License, Version 2.0.

#include <stdio.h>
#include <stdarg.h>

#include <hw/debug.h>

void xputs(const char* s) {
	while (*s != 0) {
		xputc(*s++);
	}
}

int xgetc(void) {
	return -1;
}

void xprintf(const char* fmt, ...) {
	char msg[128];
	va_list ap;
	va_start(ap, fmt);
	vsnprintf(msg, sizeof(msg), fmt, ap);
	va_end(ap);
	xputs(msg);
}

