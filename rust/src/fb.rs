use core::ptr::write_volatile;

pub struct FB {
    pub width: isize,
    pub height: isize,
}
const FB_BASE: *mut u16 = 0x40C00000 as *mut u16;

impl FB {
    pub fn plot(&mut self, x: isize, y: isize, rgba: u16) {
        if x > self.width {
            // panic
            return;
        }
        if y > self.height {
            // panic
            return;
        }
        unsafe {
            write_volatile(FB_BASE.offset(self.width * y + x), rgba);
        }
    }

    pub fn clear(&mut self) {
        unsafe {
            FB_BASE.write_bytes(0, (self.width * self.height) as usize);
        }
    }
}
