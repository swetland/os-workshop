#![no_main]
#![no_std]

use tinyos::{print, println};

#[no_mangle]
pub unsafe extern "C" fn start() -> ! {
    println!("Hello from Rust!");
    loop {}
}
