all commands assume the rust/ directory is current

Some commands assume you've run `cargo install cargo-binutils`

## Build

`./cargo.sh build --release`

## Run the build in QEMU

`${QEMU}/build/qemu-system-riscv32 -machine micro -bios ../out/boot.elf -nographic -monitor none -serial stdio -kernel target/riscv32ima-unknown-none-elf/release/mandelbrot`

## Generate a binary for the target device

`riscv64-unknown-elf-objcopy target/riscv32ima-unknown-none-elf/release/mandelbrot -O binary  target/mandelbrot.bin`

## Run on the device

`sconsole -t -b../out/boot.bin@0x40000000 -btarget/mandelbrot.bin@0x40008000 /dev/ttyACM0 1000000`
