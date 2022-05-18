#![no_main]
#![no_std]

use tinyos::{entry_fn, print, println};

entry_fn!(safe_start);

fn safe_start() -> ! {
    println!("Hello from Rust!");
    loop {}
}
