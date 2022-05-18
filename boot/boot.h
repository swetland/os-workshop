#pragma once

#ifndef __ASSEMBLY__

#include <stdint.h>

// entry.S
void mach_exception_entry(void);
void exit_mode_m(uint32_t a0, uint32_t a1, uint32_t pc, uint32_t sp);

// console.c
void console_line(char *line);

// helpers.S
uint32_t rd8safe(uint32_t addr);
uint32_t rd16safe(uint32_t addr);
uint32_t rd32safe(uint32_t addr);
void console_call_cmd(void *cmdfn, uintptr_t a1, uintptr_t a2, uintptr_t a3, uintptr_t a4, uint32_t argc);

#endif

