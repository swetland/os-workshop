#![feature(asm_const)]
#![no_std]
#![no_main]

use core::arch::asm;

use tiny_rt::{
    entry_fn,
    intrinsics::{csr_set, csr_write, irq_enable},
    io_rd32, io_wr32,
    platform::{
        eframe, trap_entry, INTb_SVC_EXTERN, LX_TIMER_EVb_ZERO, TIMER0_IRQb, CSR_SIE, CSR_STVEC,
        CSR_S_INTC_ENABLE, LX_TIMER_EN, LX_TIMER_EV_ENABLE, LX_TIMER_EV_PENDING, LX_TIMER_LOAD,
        LX_TIMER_RELOAD, TIMER0_BASE,
    },
    print, spin,
};

static mut TICKS: u32 = 0;

#[allow(non_camel_case_types)]
#[allow(clippy::upper_case_acronyms)]
enum TimerRegs {
    EN,
    EV_PENDING,
    LOAD,
    RELOAD,
    EV_ENABLE,
}

fn timer_rd(reg: TimerRegs) -> u32 {
    use TimerRegs::*;
    let addr = TIMER0_BASE
        + match reg {
            EN => LX_TIMER_EN,
            EV_PENDING => LX_TIMER_EV_PENDING,
            EV_ENABLE => LX_TIMER_EV_ENABLE,
            LOAD => LX_TIMER_LOAD,
            RELOAD => LX_TIMER_RELOAD,
        };

    unsafe { io_rd32(addr) }
}

fn timer_wr(reg: TimerRegs, val: u32) {
    use TimerRegs::*;
    let addr = TIMER0_BASE
        + match reg {
            EN => LX_TIMER_EN,
            EV_PENDING => LX_TIMER_EV_PENDING,
            EV_ENABLE => LX_TIMER_EV_ENABLE,
            LOAD => LX_TIMER_LOAD,
            RELOAD => LX_TIMER_RELOAD,
        };

    unsafe {
        io_wr32(addr, val);
    }
}

entry_fn!(start);
fn start() -> ! {
    let t_e = trap_entry as *mut ();
    csr_write!(CSR_STVEC, t_e);

    // enable timer0 irq
    csr_set!(CSR_S_INTC_ENABLE, TIMER0_IRQb);

    // enable external interrupts
    csr_set!(CSR_SIE, INTb_SVC_EXTERN);

    // enable interrupts
    irq_enable();

    timer_init();

    loop {
        let now = unsafe { TICKS };
        // xprintf("%02u:%02u.%1u\r", now/600, (now/10) % 60, now % 10);
        print!("{:0>2}:{:0>2}.{}\r", now / 600, (now / 10) % 60, now % 10);
        while now == unsafe { TICKS } {}
    }
}

fn timer_init() {
    use TimerRegs::*;

    // disable, clear pending irqs
    timer_wr(EN, 0);
    timer_wr(EV_PENDING, LX_TIMER_EVb_ZERO);

    // set for repeating every 100ms
    timer_wr(LOAD, 0);
    timer_wr(RELOAD, 50000000 / 10);

    // enable timer and timer wrap irq
    timer_wr(EN, 1);
    timer_wr(EV_ENABLE, LX_TIMER_EVb_ZERO);
}

#[no_mangle]
extern "C" fn interrupt_handler() {
    use TimerRegs::*;
    if timer_rd(EV_PENDING) != 0 {
        timer_wr(EV_PENDING, LX_TIMER_EVb_ZERO);
        unsafe {
            TICKS += 1;
        }
    }
}

#[no_mangle]
extern "C" fn exception_handler(_ef: *mut eframe) {
    print!("\n\noh shit an exception\n\n");
    spin()
}

// Copyright 2022, Brian Swetland <swetland@frotz.net>
// Licensed under the Apache License, Version 2.0

/*
#include <hw/riscv.h>
#include <hw/context.h>
#include <hw/debug.h>
#include <hw/intrinsics.h>

#include <hw/platform.h>
#include <hw/litex.h>

void start(void) {
    xprintf("Example 01 - Timer\n\n");

    // set trap vector to trap_entry() in trap-entry-single.S
    // it will call exception_handler() or interrupt_handler()
    csr_write(CSR_STVEC, (uintptr_t) trap_entry);

    // enable timer0 irq
    csr_set(CSR_S_INTC_ENABLE, TIMER0_IRQb);

    // enable external interrupts
    csr_set(CSR_SIE, INTb_SVC_EXTERN);

    // enable interrupts
    irq_enable();

    timer_init();

    while (1) {
        uint32_t now = ticks;
        xprintf("%02u:%02u.%1u\r", now/600, (now/10) % 60, now % 10);

        // wait for ticks to change
        while (now == ticks) ;
    }
}

// if an exception occurs, dump register state and halt
void exception_handler(eframe_t *ef) {
    xprintf("\n** SUPERVISOR EXCEPTION **\n");
    xprint_s_exception(ef);
    xprintf("\nHALT\n");
    for (;;) ;
}

*/
