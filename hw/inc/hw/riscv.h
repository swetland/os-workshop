// Copyright 2022, Brian Swetland <swetland@frotz.net>
// Licensed under the Apache License, Version 2.0

#pragma once

#define CSR_FFLAGS     0x001 // FP Exceptions
#define CSR_FRM        0x002 // FP Dynamic Rounding Mode
#define CSR_FCSR       0x003 // FP Control + Status

#define CSR_CYCLE      0xC00 // Cycle counter
#define CSR_TIME       0xC01 // Timer
#define CSR_INSTRET    0xC02 // Instructions-retired Counter

#define CSR_SSTATUS    0x100 // Supervisor status
#define CSR_SIE        0x104 // Supervisor interrupt enable
#define CSR_STVEC      0x105 // Supervisor trap handler base addr
#define CSR_SCOUNTEREN 0x106 // Supervisor Counter Enable

#define CSR_SENVCFG    0x10A // Supervisor Environment Config
#define CSR_SSCRATCH   0x140 // Scratch register for trap handlers
#define CSR_SEPC       0x141 // Supervisor Exception Program Counter
#define CSR_SCAUSE     0x142 // Supervisor Trap Cause
#define CSR_STVAL      0x143 // Supervisor Bad Address or Instruction
#define CSR_SIP        0x144 // Supervisor Interrupt Pending

#define CSR_SATP       0x180 // Supervisor Addr Translation and Protection

#define CSR_MVENDORID  0xF11 // Vendor ID
#define CSR_MARCHID    0xF12 // Architecture ID
#define CSR_MIMPID     0xF13 // Implementation ID
#define CSR_MHARTIT    0xF14 // Hardware Thread ID
#define CSR_MCONFIGPTR 0xF15 // Pointer to Config Data Structure

#define CSR_MSTATUS    0x300 // Machine Status
#define CSR_MISA       0x301 // Machine ISA and Extensions
#define CSR_MEDELEG    0x302 // Machine Exception Delegation
#define CSR_MIDELEG    0x303 // Machine Interrupt Delegation
#define CSR_MIE        0x304 // Machine Interrupt Enable
#define CSR_MTVEC      0x305 // Machine Trap Handler Base Address
#define CSR_MCOUNTEREN 0x307 // Machine Counter Enable
#define CSR_MSTATUSH   0x310 // Machine status high

#define CSR_MSCRATCH   0x340 // Scratch register for trap handlers
#define CSR_MEPC       0x341 // Machine Exception Program Counter
#define CSR_MCAUSE     0x342 // Machine Trap Cause
#define CSR_MTVAL      0x343 // Machine Bad Address or Instruction
#define CSR_MIP        0x344 // Machine Interrupt Pending
#define CSR_MTINST     0x34A // Machine Trap Instruction
#define CSR_MTVAL2     0x34B

#define CSR_MENVCFG    0x30A // Machine Environment Config

#define CSR_MCYCLE     0xB00 // Machine Cycle Counter
#define CSR_MINSTRET   0xB02 // Machine Instructions Retired


// inline assembly helpers for CSR access, etc
#ifndef __ASSEMBLER__
#include <stdint.h>

#define S(x) #x

#define csr_read(csr) ({ \
	uint32_t v; asm volatile ("csrr %0, " S(csr) : "=r"(v)); v; })

#define csr_write(csr, val) ({ \
	asm volatile ("csrw " S(csr) ", %0" :: "rK"(val)) });

#define csr_swap(csr, val) ({ \
	uint32_t v; asm volatile ("csrrw %0, " S(csr) ", %1" : "=r"(v) : "rK"(val)); v; })

#define csr_set(csr, bit) ({ \
	uint32_t v; asm volatile ("csrrs %0, " S(csr) ", %1" : "=r"(v) : "rK"(bit)); v; })

#define csr_clr(csr, bit) ({ \
	uint32_t v; asm volatile ("csrrc %0, " S(csr) ", %1" : "=r"(v) : "rK"(bit)); v; })

#endif
