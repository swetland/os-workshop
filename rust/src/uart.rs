const UART0_BASE: *mut u8 = 0xF0002800 as *mut u8;
const TXFULL: isize = 0x4;

use core::fmt;

use core::ptr::{read_volatile, write_volatile};

pub struct Uart {}

fn putc(addr: *mut u8, v: u8) {
    unsafe {
	while read_volatile(addr.offset(TXFULL)) > 0 {}
	write_volatile(addr, v);
    }
}

impl fmt::Write for Uart {
    fn write_str(&mut self, s: &str) -> fmt::Result {
	for ch in s.bytes() {
	    putc(UART0_BASE, ch);
	}
	Ok(())
    }
}
