const UART0_BASE: *mut u8 = 0xF0002800 as *mut u8;
const TXFULL: isize = 0x4;

use core::fmt;
use core::fmt::Write;

pub struct Uart {}

fn putc(addr: *mut u8, v: u8) {
	if v == b'\n' {
		putc(addr, b'\r')
	}
	unsafe {
		while addr.offset(TXFULL).read_volatile() > 0 {}
		addr.write_volatile(v);
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

pub fn print(arg: fmt::Arguments) {
	let mut u = Uart {};
	u.write_fmt(arg).unwrap();
}

#[macro_export]
macro_rules! print {
	($($arg:tt)*) => ($crate::uart::print(format_args!($($arg)*)))
}

#[macro_export]
macro_rules! println {
	() => (print!("\n"));
	($arg:expr) => (print!(concat!($arg, "\n")));
	($fmt:expr, $($arg:tt)*) => (print!(concat($fmt, "\n"), $($arg)*))
}
