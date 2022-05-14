#![no_main]
#![no_std]

use core::panic::PanicInfo;

#[no_mangle]
#[link_section = "start"]
pub unsafe extern "C" fn _start() -> ! {
    let _x = 42;

    let addr = 0xf0002800 as *mut u32;
    *addr = 0x33;

    // can't return so we go into an infinite loop here
    loop {}
}

#[panic_handler]
fn panic(_panic: &PanicInfo<'_>) -> ! {
    loop {}
}
