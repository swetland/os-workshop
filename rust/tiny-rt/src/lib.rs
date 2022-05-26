#![no_std]
#![allow(non_upper_case_globals)]
#![allow(non_camel_case_types)]

use cty::uint32_t;

#[macro_use]
pub mod uart;
pub mod external;
pub mod fb;
pub mod init;
pub mod platform;
pub mod intrinsics;

pub fn spin() -> ! {
    #[allow(clippy::empty_loop)]
    loop {}
}

/// # Safety
///
/// This is dangerous. Don't read where Donny Dont does.
///
pub unsafe fn io_rd32(addr: uint32_t) -> uint32_t {
    let addr: *mut u32 = addr as *mut u32;
    addr.read_volatile()
}

/// # Safety
///
/// This is dangerous. Don't write where Donny Dont does.
///
pub unsafe fn io_wr32(addr: uint32_t, val: uint32_t) {
    let addr: *mut u32 = addr as *mut u32;
    addr.write_volatile(val);
}

// the following taken from https://docs.rust-embedded.org/embedonomicon/main.html
#[macro_export]
macro_rules! entry_fn {
    ($path:path) => {
        #[export_name = "start"]
        pub unsafe fn __main() -> ! {
            // Quick type check of the path, which in general is something like
            // "client_crate::main"; we want to ensure it's a divergent function with no arguments,
            // then call it.
            let f: fn() -> ! = $path;
            f()
        }
    };
}
