// Copyright 2022, Brian Swetland <swetland@frotz.net>
// Licensed under the Apache License, Version 2.0.

#include <stdio.h>
#include <stdarg.h>

#include <hw/platform.h>
#include <hw/litex.h>
#include <hw/debug.h>

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

int xgetc(void) {
	return -1;
}

