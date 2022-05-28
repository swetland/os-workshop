#![feature(asm_const)]
#![no_std]
#![no_main]

use core::{
    arch::asm,
    sync::atomic::{AtomicU32, Ordering},
};

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

#[allow(non_camel_case_types)]
#[allow(clippy::upper_case_acronyms)]
enum TimerRegs {
    EN,
    EV_PENDING,
    LOAD,
    RELOAD,
    EV_ENABLE,
}

impl TimerRegs {
    pub fn addr(self) -> u32 {
        TIMER0_BASE
            + match self {
                Self::EN => LX_TIMER_EN,
                Self::EV_PENDING => LX_TIMER_EV_PENDING,
                Self::EV_ENABLE => LX_TIMER_EV_ENABLE,
                Self::LOAD => LX_TIMER_LOAD,
                Self::RELOAD => LX_TIMER_RELOAD,
            }
    }
}

fn timer_rd(reg: TimerRegs) -> u32 {
    unsafe { io_rd32(reg.addr()) }
}

fn timer_wr(reg: TimerRegs, val: u32) {
    unsafe {
        io_wr32(reg.addr(), val);
    }
}

static TICKS: AtomicU32 = AtomicU32::new(0);

entry_fn!(start);
fn start() -> ! {
    // set the stvec register to the address of the trap entry defined in trap-entry-single-stack.S
    csr_write!(CSR_STVEC, trap_entry as *mut ());

    // enable timer0 irq
    csr_set!(CSR_S_INTC_ENABLE, TIMER0_IRQb);

    // enable external interrupts
    csr_set!(CSR_SIE, INTb_SVC_EXTERN);

    // enable interrupts
    irq_enable();

    timer_init();

    loop {
        let now = TICKS.load(Ordering::SeqCst);
        print!("{:0>2}:{:0>2}.{}\r", now / 600, (now / 10) % 60, now % 10);
        while now == TICKS.load(Ordering::SeqCst) {}
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
        TICKS.fetch_add(1, Ordering::SeqCst);
    }
}

#[no_mangle]
extern "C" fn exception_handler(ef: *mut eframe) {
    print!("\n\noh shit an exception\n\n");
    print!("\n\n{:?}\n\n", unsafe { *ef });
    spin()
}
