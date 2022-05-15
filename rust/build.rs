use regex::Regex;
use std::fs;

// (
//  echo '// GENERATED FILE, DO NOT EDIT' ;
//  sed \
//      -e 's@unsigned char \([^[]*\)\[@pub const \1: [u8; @' \
//      -e 's@{@[@' \
//      -e 's@}@]@' \
//      ../external/vgafonts.c
// ) > src/external/vgafonts.rs

fn main() {
    let src = "../external/vgafonts.c";
    let dest = "src/external/vgafonts.rs";
    println!("cargo:rerun-if-changed={}", src);
    println!("cargo:rerun-if-changed=build.rs");
    println!("cargo:rustc-link-arg=-Tapp.ram.ld");
    println!("cargo:rustc-link-arg=-melf32lriscv");

    let contents = fs::read_to_string(src).unwrap();
    let c1 = Regex::new(r"unsigned char ([^\[]*)\[")
        .unwrap()
        .replace_all(&contents, r"pub const $1: [u8; ");
    let c2 = Regex::new(r"\{").unwrap().replace_all(&c1, r"[");
    let c3 = Regex::new(r"\}").unwrap().replace_all(&c2, r"]");

    fs::write(dest, &*c3).unwrap();
}
