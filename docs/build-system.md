
# Build System Notes

Define projects in `project/NAME.lib.mk` (for libraries) or `project/NAME.app.mk` (for executables)

## Common Module Parameters

`MOD_NAME`: Name of the project. Must be unique.
Libraries must be named such that `libNAME/inc` is their include directory and their generated library will be `out/libNAME.a`

`MOD_SRC`: List of source files (`.c` or `.S`). Paths must be relative to the top of the project.

`MOD_LIB`: List of libraries the module depends on. Bare library name, for example `c` for `libc`, etc.

## Application Parameters

`MOD_LDSCRIPT`: alternate linker script

`MOD_QEMU_FB`: If defined, this when `make run.NAME` is invoked to run this module under Qemu, the framebuffer will be enabled.

## Application Build Outputs

`out/NAME/...`: Intermediate files (`.o`, `.d`, etc)

`out/NAME.elf`: ELF binary with full symbols, debug info, etc

`out/NAME.bin`: Raw binary file for loading directly into memory

`out/NAME.lst`: Disassembly listing (handy for debugging)

## Library Build Outputs

`out/libNAME/...`: Intermediate files

`out/libNAME.a`: Static library

## Sample Project

```make
# project/mandelbrot-fb.app.mk

MOD_NAME := mandelbrot-fb
MOD_SRC := hw/src/start.S misc/mandelbrot-fb.c
MOD_SRC += hw/src/debug-printf.c hw/src/debug-io.c
MOD_LIB := c
MOD_QEMU_FB := yes

include make/app.mk
```
