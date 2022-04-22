# Setting up your environment

You will need a cross compiler (riscv32-elf-gcc), debugger (riscv-elf-gdb), and emulator (qemu-system-riscv32).

If you already have these available and installed, simply edit `local.mk` to reflect how to invoke them:
```
XTOOLCHAIN := riscv32-elf-
QEMU := qemu-system-riscv32
```

Note that the toolchain path is a prefix, omitting `gcc` because the makefile will stick `gdb`, `objdump`, `ld`, etc on the end as needed.

## Building GCC and GDB (if needed)

[Travis](https://github.com/travisg) has a handy set of scripts to checkout and build gcc and gdb

You may need to install some packages (libgmp-dev for sure) for this to succeed.  If it fails, find the missing package and let me know so I can update these instructions.

```
$ git clone git@github.com:travisg/toolchains.git
$ cd toolchains
$ ./doit -f -a riscv32
```

On success you'll end up with `riscv32-elf-11.2.0-Linux-x86_64` with `bin/riscv32-elf-*` inside

I like to keep all my cross compilers in a common place, so I do this:
```
$ mv riscv32-elf-11.2.0-Linux-x86_64/ /toolchain/riscv32-11.2.0
```

## Building Qemu (if needed)

```
wget https://download.qemu.org/qemu-7.0.0.tar.xz
tar axvf qemu-7.0.0.tar.xz 
cd qemu-7.0.0
./configure --prefix=/work/qemu --target-list=riscv32-softmmu
make -j32
make install
```

You can change `--prefix=` to point at where you want to install qemu or remove it to install in /usr/local (which will require running make install as root)

## Alternate Toolchain Build

I haven't tried this, but it's the official-ish RISCV toolchain build system and looks like it should build gcc, gdb, and qemu:
https://github.com/riscv-collab/riscv-gnu-toolchain

Be sure to configure it for 32bit riscv.

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

