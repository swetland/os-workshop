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

    // TODO: initialize bss to 0. The BSS segment is currently length zero so ...
    // asm!("la sp, __stack_top", "j start",);
    start()
}

#[panic_handler]
fn panic(_panic: &PanicInfo<'_>) -> ! {
    loop {}
}
