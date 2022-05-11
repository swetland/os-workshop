// Copyright 2022, Brian Swetland <swetland@frotz.net>
// Licensed under the Apache License, Version 2.0

#pragma once

#include <stdint.h>

// debug-io.c -- can be replaced with alternate impls
void xputc(unsigned c);
void xputs(const char* s);
int  xgetc(void);

// debug-printf.c -- calls xputs()
void xprintf(const char* fmt, ...);

// print-exception.c -- calls xprintf()
void xprint_exception(uint32_t regs[32]);

