use crate::external::vgafonts::vga_rom_16;

pub struct FB {
    ptr: *mut u16,
    pub width: usize,
    pub height: usize,
    fg: u16,
    bg: u16,
}
const FB_BASE: *mut u16 = 0x40C00000 as *mut u16;
const CH_WIDTH: usize = 8;
const CH_HEIGHT: usize = 16;

impl Default for FB {
    fn default() -> Self {
        Self::new()
    }
}

impl FB {
    pub fn new() -> FB {
        FB {
            ptr: FB_BASE,
            width: 640,
            height: 480,
            fg: 0xffff,
            bg: 0,
        }
    }
    pub fn point(&mut self, x: usize, y: usize) {
        if x > self.width {
            // panic
            return;
        }
        if y > self.height {
            // panic
            return;
        }
        unsafe {
            self.ptr.add(self.width * y + x).write_volatile(self.fg);
        }
    }

    pub fn setfg(&mut self, color: u16) {
        self.fg = color;
    }

    pub fn setbg(&mut self, color: u16) {
        self.bg = color;
    }

    pub fn text(&mut self, x0: usize, y0: usize, t: &str) {
        if x0 >= self.width
            || x0 + CH_WIDTH >= self.width
            || y0 >= self.height
            || y0 + CH_HEIGHT >= self.height
        {
            return;
        }
        let fg = self.fg;
        let bg = self.bg;
        let mut x1 = x0;
        for ch in t.bytes() {
            if x1 + CH_WIDTH >= self.width {
                break;
            }
            for y in 0..CH_HEIGHT {
                unsafe {
                    let mut ptr = self.ptr.add(self.width * y + x1);
                    let mut bits = vga_rom_16[(ch as usize * CH_WIDTH * 2 + y) as usize] as u32;
                    for _x in 0..CH_WIDTH {
                        let color = if bits & 0x80 != 0 { fg } else { bg };
                        bits <<= 1;
                        *ptr = color;
                        ptr = ptr.offset(1);
                    }
                }
            }
            x1 += CH_WIDTH;
        }
    }

    pub fn clear(&mut self) {
        unsafe {
            self.ptr.write_bytes(0, (self.width * self.height) as usize);
        }
    }
}
