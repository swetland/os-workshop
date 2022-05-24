#![no_main]
#![no_std]

use tiny_rt::{entry_fn, print, println, spin};

entry_fn!(start);

fn start() -> ! {
    let top: i32 = 1000;
    let bottom: i32 = -1000;
    let ystep: i32 = 50;
    let left: i32 = -2500;
    let right: i32 = 1000;
    let xstep: i32 = 30;
    let maxiter: u32 = 1000;

    let mut y0 = top;
    while y0 > bottom {
        let mut x0 = left;
        while x0 < right {
            let mut i = 0;
            let mut x = 0;
            let mut y = 0;
            let mut ch: char = ' ';
            while i < maxiter {
                let x2 = x * x / 1000;
                let y2 = y * y / 1000;
                if (x2 + y2) > 4000 {
                    ch = char::from_digit(i, 10).unwrap_or('@');
                    break;
                }
                y = 2 * x * y / 1000 + y0;
                x = x2 - y2 + x0;
                i += 1;
            }
            print!("{}", ch);
            x0 += xstep;
        }
        println!();
        y0 -= ystep;
    }

    println!();

    println!("Hello from Mandelbrot!");
    spin()
}
