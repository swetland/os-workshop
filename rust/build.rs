use glob::glob;
use regex::Regex;
use std::{env, fs, path::PathBuf};

fn main() {
    let out_dir = PathBuf::from(env::var_os("OUT_DIR").unwrap());
    let vga_src = "../external/vgafonts.c";
    println!("cargo:rerun-if-changed={}", vga_src);
    println!("cargo:rerun-if-changed=platform_bindings.h");
    println!("cargo:rerun-if-changed=build.rs");
    println!("cargo:rustc-link-arg=-Tmake/app.ram.ld");
    println!("cargo:rustc-link-arg=-melf32lriscv");
    // extend the library search path
    println!("cargo:rustc-link-search={}", out_dir.display());

    // copy and transform the FB VGA data
    let vga_dest = "src/external/vgafonts.rs";
    let contents = fs::read_to_string(vga_src).unwrap();
    let c1 = Regex::new(r"unsigned char ([^\[]*)\[")
        .unwrap()
        .replace_all(&contents, r"pub const $1: [u8; ");
    let c2 = Regex::new(r"\{").unwrap().replace_all(&c1, r"[");
    let c3 = Regex::new(r"\}").unwrap().replace_all(&c2, r"]");

    fs::write(vga_dest, &*c3).unwrap();

    // grab platform headers and make them available to Rust
    let bindings = bindgen::builder()
        .header("platform_bindings.h")
        .rust_target(bindgen::RustTarget::Nightly)
        .rustfmt_bindings(true)
        .clang_arg("-I../hw/inc")
        .clang_arg("--target=riscv32-unknown-none-elf")
        .ctypes_prefix("cty")
        .use_core()
        .generate()
        .expect("Could not generate Rust bindings from platform_bindings.h");

    bindings
        .write_to_file(out_dir.join("platform_bindings.rs"))
        .unwrap_or_else(|_| {
            panic!(
                "Could not write Rust bindings in directory {}",
                out_dir.display()
            )
        });

    // asm setup
    if env::var_os("CC").is_none() {
        let cc_val = "riscv64-unknown-elf-gcc";
        let cc_key = "CC";
        env::set_var(cc_key, cc_val);
    }
    cc::Build::new()
        .file("../hw/src/start.S")
        .includes(["../hw/inc", "../libc/inc"])
        .compile("start");

    // grab linker scripts
    fs::create_dir("make").unwrap_or(());
    for ld in glob("../make/*.ld")
        .expect("could not find .ld files in ../make/")
        .flatten()
    {
        let dst = format!("make/{}", ld.display());
        fs::copy(ld, dst).unwrap();
    }
}
