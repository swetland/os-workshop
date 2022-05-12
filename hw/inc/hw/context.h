// Copyright 2022, Brian Swetland <swetland@frotz.net>
// Licensed under the Apache License, Version 2.0

#pragma once

#include <stdint.h>

// cframe - Context Frame
// contains the 16 callee-save and global registers
typedef struct cframe {
	uint32_t pc;
	uint32_t sp;
	uint32_t gp;
	uint32_t tp;
	uint32_t s0;
	uint32_t s1;
	uint32_t s2;
	uint32_t s3;
	uint32_t s4;
	uint32_t s5;
	uint32_t s6;
	uint32_t s7;
	uint32_t s8;
	uint32_t s9;
	uint32_t s10;
	uint32_t s11;
} cframe_t;

// tframe - Trap Frame
// contains the 16 caller-save and temporary registers
// as well as the pc
typedef struct tframe {
	uint32_t ra;
	uint32_t t0;
	uint32_t t1;
	uint32_t t2;
	uint32_t t3;
	uint32_t t4;
	uint32_t t5;
	uint32_t t6;
	uint32_t a0;
	uint32_t a1;
	uint32_t a2;
	uint32_t a3;
	uint32_t a4;
	uint32_t a5;
	uint32_t a6;
	uint32_t a7;

	uint32_t pc;
	uint32_t _0;
	uint32_t _1;
	uint32_t _2;
} tframe_t;

// eframe - Exception Frame
// contains all general registers
// It stacks a cframe on an iframe
typedef struct eframe {
	uint32_t ra;
	uint32_t t0;
	uint32_t t1;
	uint32_t t2;
	uint32_t t3;
	uint32_t t4;
	uint32_t t5;
	uint32_t t6;
	uint32_t a0;
	uint32_t a1;
	uint32_t a2;
	uint32_t a3;
	uint32_t a4;
	uint32_t a5;
	uint32_t a6;
	uint32_t a7;

	uint32_t pc;
	uint32_t sp;
	uint32_t gp;
	uint32_t tp;
	uint32_t s0;
	uint32_t s1;
	uint32_t s2;
	uint32_t s3;
	uint32_t s4;
	uint32_t s5;
	uint32_t s6;
	uint32_t s7;
	uint32_t s8;
	uint32_t s9;
	uint32_t s10;
	uint32_t s11;
} eframe_t;

// save register state into from and restore it from to
void context_switch(cframe_t* from, cframe_t* to);

// helper function which calls s2(s0, s1)
void context_entry(void);

// trap vector
// saves register on stack in tframe_t or eframe_t
// calls interrupt_handler() or exception_handler()
void trap_entry(void);

// restores tframe_t from stack and return from interrupt
void trap_exit(void);

// restores eframe_t from stack and return from exception
void exception_exit(void);

// debug helpers
void xprint_m_exception(eframe_t* ef);
void xprint_s_exception(eframe_t* ef);

// called from trap_entry() to handle interrupts or exceptions
void interrupt_handler(void);
void exception_handler(eframe_t* ef);
