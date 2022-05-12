#pragma once

#ifndef __ASSEMBLY__

// entry.S
void mach_exception_entry(void);
void exit_mode_m(uint32_t a0, uint32_t a1, uint32_t pc, uint32_t sp);

#endif

