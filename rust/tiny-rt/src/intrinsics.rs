use core::{arch::asm, u32};
//use tiny_rt_macros::*;

pub fn csr_read(csr: u32) -> u32 {
    let mut x = u32::MAX;
    unsafe {
        // asm!("csrr {0}, {1}", out(reg) x, in(reg) csr);
        //asm!("csrr {0}, {csr}", out(reg) x, csr=in(reg) &csr);
    }
    x
}
