use core::arch::asm;

use core::panic::PanicInfo;

#[no_mangle]
#[link_section = ".start"]
pub unsafe extern "C" fn _start() -> ! {
    // TODO: initialize bss to 0. The BSS segment is currently length zero so ...
    asm!("la sp, __stack_top", "j start",);
    loop {}
}

#[panic_handler]
fn panic(_panic: &PanicInfo<'_>) -> ! {
    loop {}
}
