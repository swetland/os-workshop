// Copyright 2022, Brian Swetland <swetland@frotz.net>
// Licensed under the Apache License, Version 2.0.

#include <stdio.h>
#include <stdarg.h>

#include <hw/debug.h>

void xprintf(const char* fmt, ...) {
	char msg[128];
	va_list ap;
	va_start(ap, fmt);
	vsnprintf(msg, sizeof(msg), fmt, ap);
	va_end(ap);
	xputs(msg);
}

void vxprintf(const char* fmt, va_list ap) {
	char msg[128];
	vsnprintf(msg, sizeof(msg), fmt, ap);
	xputs(msg);
}

