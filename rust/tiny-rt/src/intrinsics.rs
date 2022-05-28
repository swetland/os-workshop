use core::arch::asm;
pub use tiny_rt_macros::*;

use crate::platform::SSTATUS_SIE;

#[inline(always)]
pub fn irq_enable() {
    unsafe {
        asm!("csrrs x0, sstatus, {sie}", sie = const SSTATUS_SIE);
    }
}
