#pragma once

#include <stdarg.h>
#include <stddef.h>

int vsprintf(char *str, const char *format, va_list ap);
int vsnprintf(char *str, size_t size, const char *format, va_list ap);

int sprintf(char *str, const char *fmt, ...);
int snprintf(char *str, size_t len, const char *fmt, ...);
