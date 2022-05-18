use core::arch::global_asm;

use core::panic::PanicInfo;

global_asm!(
    r#"
.section ".start"

.globl _start
_start:
    /* zero BSS */
    la t0, __bss_start
    la t1, __bss_end
zero_loop:
    beq t0, t1, zero_done
    sw zero, 0(t0)
    add t0, t0, 4
    j zero_loop
zero_done:

    /* setup initial stack */
    la sp, __stack_top

    /* enter C code */
    jal start

    /* infinite loop */
    j .
"#
);

/// # Safety
///
/// "Life is pain, Highness. Anyone who says differently is selling something."
///
/// Likewise, reality is dangerous, and we're talking directly to semi-sentient rock with tiny lightning.
///
#[no_mangle]
#[link_section = ".start"]
pub unsafe extern "C" fn __start() -> ! {
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
