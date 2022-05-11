// Copyright 2022, Brian Swetland <swetland@frotz.net>
// Licensed under the Apache License, Version 2.0

#pragma once

#include <stdint.h>

// this layout must match the assembly in context-switch.S
typedef struct {
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
} rv32_ctxt_t;

// store the current state into prev
// restore the new state from next
void context_switch(rv32_ctxt_t* prev, rv32_ctxt_t* next);

// bootstrap for a new context
// will call s2(s0, s1)
// and call context_exit() should it return
void context_entry(void);
