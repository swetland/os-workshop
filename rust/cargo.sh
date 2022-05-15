#!/bin/sh
cargo +nightly -Z build-std=core "$@" --target riscv32ima-unknown-none-elf.json
