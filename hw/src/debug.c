// Copyright 2022, Brian Swetland <swetland@frotz.net>
// Licensed under the Apache License, Version 2.0.

#include <stdio.h>
#include <stdarg.h>

#define UART_BASE 0x10000000

#define wr8(v,a) (*((volatile unsigned char*) (a)) = v)

void xputc(unsigned c) {
	wr8(c, UART_BASE);
}

void xputs(const char* s) {
	while (*s != 0) {
		wr8(*s++, UART_BASE);
	}
}

void xprintf(const char* fmt, ...) {
	char msg[128];
	va_list ap;
	va_start(ap, fmt);
	vsnprintf(msg, sizeof(msg), fmt, ap);
	va_end(ap);
	xputs(msg);
}

int xgetc(void) {
	return -1;
}

