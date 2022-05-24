use core::panic::PanicInfo;

/// # Safety
///
/// "Life is pain, Highness. Anyone who says differently is selling something."
///
/// Likewise, reality is dangerous, and we're talking directly to semi-sentient rock with tiny lightning.
///
#[no_mangle]
pub unsafe extern "C" fn __mane() -> ! {
    extern "Rust" {
        fn start() -> !;
    }
    start()
}

#[panic_handler]
fn panic(_panic: &PanicInfo<'_>) -> ! {
    loop {}
}
