use glob::glob;
use std::{env, fs, path::PathBuf};

fn main() {
    let out_dir = PathBuf::from(env::var_os("OUT_DIR").unwrap());
    println!("cargo:rerun-if-changed=build.rs");
    println!("cargo:rustc-link-arg=-Tmake/app.ram.ld");
    println!("cargo:rustc-link-arg=-melf32lriscv");
    // extend the library search path
    println!("cargo:rustc-link-search={}", out_dir.display());

    // asm setup
    if env::var_os("CC").is_none() {
        let cc_val = "riscv64-unknown-elf-gcc";
        let cc_key = "CC";
        env::set_var(cc_key, cc_val);
    }
    cc::Build::new()
        .files(["../../hw/src/trap-entry-single-stack.S"])
        .compile("start");

    // grab linker scripts
    let ld_script_dir = out_dir.join("make");
    fs::create_dir_all(&ld_script_dir).unwrap();
    for ld in glob("../../make/*.ld")
        .expect("could not find .ld files in ../../make/")
        .flatten()
    {
        let dst = &ld_script_dir.join(ld.file_name().unwrap());
        fs::copy(ld, dst).unwrap();
    }
}
