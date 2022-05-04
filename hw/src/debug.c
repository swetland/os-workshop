// Copyright 2022, Brian Swetland <swetland@frotz.net>
// Licensed under the Apache License, Version 2.0.

#include <stdio.h>
#include <stdarg.h>

#include <hw/platform.h>
#include <hw/litex.h>

#define uart_rd(a) io_rd32(UART0_BASE + LX_UART_ ## a)
#define uart_wr(a,v) io_wr32(UART0_BASE + LX_UART_ ## a, v)

void xputc(unsigned c) {
	if (c == '\n') {
		while (uart_rd(TXFULL)) ;
		uart_wr(TX, '\r');
	}
	while (uart_rd(TXFULL)) ;
	uart_wr(TX, c);
}

void xputs(const char* s) {
	while (*s != 0) {
		xputc(*s++);
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

