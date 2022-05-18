#![no_main]
#![no_std]

use tinyos::{entry_fn, print, println};

entry_fn!(main);

fn main() -> ! {
    println!("Hello from Rust!");
    loop {}
}
