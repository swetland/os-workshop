all commands assume the rust/ directory is current

## Build

`cargo build --release`

## Run the build in QEMU

`${QEMU}/build/qemu-system-riscv32 -machine micro -bios ../out/boot.elf -nographic -monitor none -serial stdio -kernel target/riscv32i-unknown-none-elf/release/tinyos`

## Generate a binary for the target device

`cargo objcopy --release -- -O binary  ../out/tinyos.bin`

## Run on the device

`sconsole -t -b../out/boot.bin@0x40000000 -b../out/tinyos.bin@0x40008000 /dev/ttyACM0 1000000`