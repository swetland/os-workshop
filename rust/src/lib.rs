#![no_std]

#[macro_use]
pub mod uart;
pub mod external;
pub mod fb;
pub mod init;

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
