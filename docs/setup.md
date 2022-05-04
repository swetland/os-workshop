# Setting up your environment

You will need a cross compiler (riscv32-elf-gcc), debugger (riscv-elf-gdb), and emulator (qemu-system-riscv32).

See "Installing GCC and GDB" or "Building GCC and GDB" below for instructions.

Qemu is a bit more complicated -- we need a custom version that has support for peripherals compatible with the FPGA SoC.

See "Building Qemu" below.

Once you have the compiler, debugger, and emulator installed, you can tell the build system
where to find them by editing 'local.mk' at the root of this project:
``` makefile
XTOOLCHAIN := /path/to/bin/riscv32-elf-
QEMU := /path/to/bin/qemu-system-riscv32
```

Note that the toolchain path is a prefix, omitting `gcc` because the makefile will stick `gdb`, `objdump`, `ld`, etc on the end as needed.

Once you're all set up, proceed to "Checking out and building" below.


## Installing GCC and GDB Ubuntu
This should work on Ubuntu 20.04.4LTS or newer.  Not sure about older versions.
```
sudo apt-get install gcc-riscv64-unknown-elf gdb-multiarch
```
That should get you `riscv64-unknown-elf-gcc` and `gdb-multiarch`, and you
can add the following to your `local.mk`:

``` makefile
XTOOLCHAIN := /usr/bin/riscv64-unknown-elf-
QEMU := /usr/bin/qemu-system-riscv32
```

Note that the toolchain is 64-bit by default, and the qemu system is 32-bit; the compiler arguments in the main
Makefile specify that a 32-bit binary should be built.

## Building GCC and GDB (if needed)

[Travis](https://github.com/travisg) has a handy set of scripts to checkout and build gcc and gdb

You may need to install some packages (libgmp-dev for sure) for this to succeed.  If it fails, find the missing package and let me know so I can update these instructions.

```
$ git clone git@github.com:travisg/toolchains.git
$ cd toolchains
$ ./doit -f -a riscv32
```

On success you'll end up with `riscv32-elf-11.2.0-Linux-x86_64` with `bin/riscv32-elf-*` inside.

I like to keep all my cross compilers in a common place, so I do this:
```
$ mv riscv32-elf-11.2.0-Linux-x86_64/ /toolchain/riscv32-11.2.0
```

## Alternate Toolchain Build

I haven't tried this, but it's the official-ish RISCV toolchain build system and looks like it should build gcc, gdb, and qemu:
https://github.com/riscv-collab/riscv-gnu-toolchain

Be sure to configure it for 32bit riscv.

## Building Qemu

You need to check out the modified qemu from github and select the "workshop" branch to build:

```
git clone https://github.com/swetland/qemu.git
cd qemu
git checkout -b workshop origin/workshop
./configure --prefix=/toolchain/qemu --target-list=riscv32-softmmu
make -j32
```

You could use `make install` to install qemu at the specified prefix, but since you may have to update it from time to time
it's easiest to just configure things to point to the binary in the build itself.  Modify your `local.mk` to include:
``` makefile
QEMU := /path/to/qemu/build/qemu-system-riscv32
```

## Updating Qemu

You may have to refresh your build from time to time...
```
git remote update
git rebase origin/workshop
make clean
make -j32
make install
```

## Checking out and building

```
$ git clone git@github.com:swetland/os-workshop.git
$ cd os-workshop
$ make
```

There are some simple projects, defined in `project/*.mk`

Build results appear in `out/<projectname>/...` (objects, etc), `out/<projectname>.elf` (the binary), and `out/<projectname>.lst` (disasembly of the binary, handy for debugging).

You can run a project under qemu with `make run.<projectname>` or under qemu, ready for gdb connection with `make debug.<projectname>`

To exit qemu: `CTRL-A` `X`

To bring up the qemu console: `CTRL-A` `C`

## Using GDB with QEMU

In one terminal:
```
$ make debug.mandelbrot
```

In another:
```
$ riscv32-elf-gdb out/mandelbrot.elf
...
Reading symbols from out/mandelbrot.elf...
(gdb) target remote :7777
0x00001000 in ?? ()
(gdb) break main
Breakpoint 1 at 0x8000002c: file misc/mandelbrot.c, line 7.
(gdb) cont
Continuing.

Breakpoint 1, main (argc=0, argv=0x0) at misc/mandelbrot.c:7
7               int top = 1000, bottom = -1000, ystep = 50;
(gdb)
```
