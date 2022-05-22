#![no_main]
#![no_std]

use tinyos::fb::FB;
use tinyos::{entry_fn, spin, print, println};

const COLORS: [u16; 12] = [
    0x1111, 0x2222, 0x3333, 0x4444, 0x5555, 0x6666, 0x7777, 0x8888, 0x9999, 0xAAAA, 0xBBBB, 0xCCCC,
];

entry_fn!(start);

fn start() -> ! {
    let mut fb = FB::new();

    fb.clear();

    let maxiter: usize = 1000;

    let mut py = 0;
    while py < fb.height {
        let y0 = 1300 - (2600 * py as isize) / (fb.height as isize);
        if py == 18 {
            fb.setbg(0);
            fb.setfg(0xffff);
            fb.text(0, fb.height - 17, "Hello, Mandelbrot!");
        }
        let mut px = 0;
        while px < fb.width {
            let x0 = -2500 + (3500 * px as isize) / (fb.width as isize);
            fb.setfg(0);
            let mut i: usize = 0;
            let mut x: isize = 0;
            let mut y: isize = 0;
            while i < maxiter {
                let x2 = x * x / 1000;
                let y2 = y * y / 1000;
                if (x2 + y2) > 4000 {
                    fb.setfg(COLORS[if i > 11 { 11 } else { i }]);
                    break;
                }
                y = 2 * x * y / 1000 + y0;
                x = x2 - y2 + x0;
                i += 1;
            }
            fb.point(px, py);
            px += 1;
        }
        py += 1;
    }

    println!();

    println!("Hello from Mandelbrot!");
    spin()
}
