#pragma once

// offsets into the interrupt workspace
#define IWS_TIMECMP	0x00
#define IWS_TICKINC	0x04
#define IWS_SAVE0	0x08
#define IWS_SAVE1	0x0C
#define IWS_SAVE2	0x10
#define IWS_SAVE3	0x14
#define IWS_SAVE4       0x18
#define IWS_SIZE	0x40

#ifndef __ASSEMBLY__

// entry.S
void mach_exception_entry(void);
void enter_mode_s(uint32_t a0, uint32_t a1, uint32_t pc, uint32_t sp);

#endif

