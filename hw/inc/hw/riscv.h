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
#define CSR_MHARTID    0xF14 // Hardware Thread ID
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

#define CSR_PMPCFG(n)  (0x3A0+(n)) // 0-15
#define CSR_PMPADDR(n) (0x3B0+(n)) // 0-63

// CSR_MSTATUS bits
#define MSTATUS_TSR  0x00400000U // Trap SRET
#define MSTATUS_TW   0x00200000U // Timeout Wait (trap on WFI)
#define MSTATUS_TVM  0x00100000U // Trap Virtual Memory
#define MSTATUS_MPRV 0x00020000U // Modify PRiVilege
#define MSTATUS_MPP  0x00001800U // Mode M Prev Priv Mode
#define MSTATUS_MPIE 0x00000080U // Mode M Prev IRQ Enable
#define MSTATUS_MIE  0x00000008U // Mode M IRQ Enable

#define MSTATUS_MPP_SHIFT 11

// CSR_STATUS (and CSR_MSTATUS) bits
#define SSTATUS_SD   0x80000000U // Some Dirty (XS or FS are nonzero)
#define SSTATUS_MXR  0x00080000U // Make eXecutable Readable (MMU)
#define SSTATUS_SUM  0x00040000U // Supervisor User Memory access permitted
#define SSTATUS_XS   0x00018000U // Extension State
#define SSTATUS_FS   0x00006000U // FP State
#define SSTATUS_SPP  0x00000100U // Mode S Prev Priv Mode
#define SSTATUS_UBE  0x00000040U // User Big Endian
#define SSTATUS_SPIE 0x00000020U // Mode S Prev IRQ Enable
#define SSTATUS_SIE  0x00000002U // Mode S IRQ Enable

#define SSTATUS_XS_SHIFT 15
#define SSTATUS_FS_SHIFT 13

// Privilege Levels
#define PRIV_U 0 // User
#define PRIV_S 1 // Supervisor
#define PRIV_M 3 // Machine

// Interrupt numbers (xCAUSE) or bits (xIE/xIP/MIDELEG)
#define INTn_SVC_SW       1
#define INTn_MACH_SW      3
#define INTn_SVC_TIMER    5
#define INTn_MACH_TIMER   7
#define INTn_SVC_EXTERN   9
#define INTn_MACH_EXTERN  11

#define INTb_SVC_SW       (1U << 1)
#define INTb_MACH_SW      (1U << 3)
#define INTb_SVC_TIMER    (1U << 5)
#define INTb_MACH_TIMER   (1U << 7)
#define INTb_SVC_EXTERN   (1U << 9)
#define INTb_MACH_EXTERN  (1U << 11)


// Exception numbers (xCAUSE) or bits (MEDELEG)
#define EXCn_INSTR_ADDR_MISALIGN   0
#define EXCn_INSTR_ACCES_FAULT     1
#define EXCn_ILLEGAL_INSTR         2
#define EXCn_BREAKPOINT            3
#define EXCn_LOAD_ADDR_MISALIGN    4
#define EXCn_LOAD_ADDR_FAULT       5
#define EXCn_STORE_ADDR_MISALIGN   6
#define EXCn_STORE_ADDR_FAULT      7
#define EXCn_ECALL_UMODE           8
#define EXCn_ECALL_SMODE           9
#define EXCn_ECALL_MMODE           11
#define EXCn_INSTR_PAGE_FAULT      12
#define EXCn_LOAD_PAGE_FAULT       13
#define EXCn_STORE_PAGE_FAULT      15

#define EXCb_INSTR_ADDR_MISALIGN   (1U << 0)
#define EXCb_INSTR_ACCES_FAULT     (1U << 1)
#define EXCb_ILLEGAL_INSTR         (1U << 2)
#define EXCb_BREAKPOINT            (1U << 3)
#define EXCb_LOAD_ADDR_MISALIGN    (1U << 4)
#define EXCb_LOAD_ADDR_FAULT       (1U << 5)
#define EXCb_STORE_ADDR_MISALIGN   (1U << 6)
#define EXCb_STORE_ADDR_FAULT      (1U << 7)
#define EXCb_ECALL_UMODE           (1U << 8)
#define EXCb_ECALL_SMODE           (1U <<  9)
#define EXCb_ECALL_MMODE           (1U << 11)
#define EXCb_INSTR_PAGE_FAULT      (1U << 12)
#define EXCb_LOAD_PAGE_FAULT       (1U << 13)
#define EXCb_STORE_PAGE_FAULT      (1U << 15)


#define PMP_CFG_LOCK    (1U << 7) // disallow further rights until reset
#define PMP_CFG_A_OFF   (0U << 3) // disable this entry
#define PMP_CFG_A_TOR   (1U << 3) // match top of range (prev entry is bottom)
#define PMP_CFG_A_NA4   (2U << 3) // match naturally aligned 4byte
#define PMP_CFG_A_NAPOT (3U << 3) // match naturally aligned power of two
#define PMP_CFG_X       (1U << 2) // execute
#define PMP_CFG_W       (1U << 1) // write
#define PMP_CFG_R       (1U << 0) // read

// inline assembly helpers for CSR access, etc
#ifndef __ASSEMBLER__
#include <stdint.h>

#define __S(x) #x

#define csr_read(csr) ({ \
	uint32_t v; asm volatile ("csrr %0, " __S(csr) : "=r"(v)); v; })

#define csr_write(csr, val) ({ \
	asm volatile ("csrw " __S(csr) ", %0" :: "rK"(val)); });

#define csr_swap(csr, val) ({ \
	uint32_t v; asm volatile ("csrrw %0, " __S(csr) ", %1" : "=r"(v) : "rK"(val)); v; })

#define csr_set(csr, bit) ({ \
	uint32_t v; asm volatile ("csrrs %0, " __S(csr) ", %1" : "=r"(v) : "rK"(bit)); v; })

#define csr_clr(csr, bit) ({ \
	uint32_t v; asm volatile ("csrrc %0, " __S(csr) ", %1" : "=r"(v) : "rK"(bit)); v; })

#endif
