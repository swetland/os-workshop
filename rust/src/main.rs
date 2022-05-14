#![no_main]
#![no_std]

use core::panic::PanicInfo;


/*
const DRAM_BASE: usize = 0x4000000;
const DRAM_SIZE: usize = 32 * 1024 * 1024;
const SP: usize = DRAM_BASE + DRAM_SIZE;
 */

use core::arch::asm;

use tinyos::uart;
use core::fmt::Write;

#[no_mangle]
#[link_section = ".start"]
pub unsafe extern "C" fn _start() -> ! {
    // TODO: initialize bss to 0. The BSS segment is currently length zero so ...
    asm!(
	"lui sp, 0x42000",
    );
    start();
}

#[no_mangle]
pub unsafe extern "C" fn start() -> ! {
    let mut u = uart::Uart{};
    u.write_str("Hello from Rust!\r\n").unwrap();
    loop {}
}

#[panic_handler]
fn panic(_panic: &PanicInfo<'_>) -> ! {
    loop {}
}
