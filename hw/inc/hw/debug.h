// Copyright 2022, Brian Swetland <swetland@frotz.net>
// Licensed under the Apache License, Version 2.0

#pragma once

#include <stdint.h>
#include <stdarg.h>

// debug-io.c -- can be replaced with alternate impls
void xputc(unsigned c);
void xputs(const char* s);
int  xgetc(void);

// debug-printf.c -- calls xputs()
void xprintf(const char* fmt, ...);
void vxprintf(const char* fmt, va_list ap);
