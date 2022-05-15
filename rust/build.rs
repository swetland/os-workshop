use cc;

fn main() {
    println!("cargo:rerun-if-changed=../external/vgafonts.c");
    cc::Build::new()
        .file("../external/vgafonts.c")
        .compile("vgafonts");
}
