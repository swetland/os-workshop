# syntax=docker/dockerfile:1.0.0-experimental
FROM ubuntu:focal

WORKDIR /root

# prep for the docker build process
ENV DEBIAN_FRONTEND noninteractive
RUN echo 'APT { Get { Assume-Yes "true"; }; };' > /etc/apt/apt.conf.d/99assume-yes
RUN mkdir $HOME/.ssh && echo "StrictHostKeyChecking no" >> $HOME/.ssh/config \
  && chmod 600 $HOME/.ssh && chmod 600 $HOME/.ssh/config

# install the packages we need to build out our toolchain
RUN apt update \
  && apt-get install \
     build-essential \
     gcc-riscv64-unknown-elf \
     gdb-multiarch \
     git \
     texinfo \
     wget \
     less \
     man \
     manpages \
     libexpat-dev \
     libgmp-dev \
     python \
     python3 \
     ninja-build \
     libglib2.0-dev \
     libpixman-1-dev \
  && apt-get clean autoclean && apt-get autoremove --yes && rm -rf /var/lib/apt/lists/*
RUN echo y | unminimize
RUN echo "quiet=on" > $HOME/.wgetrc
ENV PAGER /usr/bin/less

RUN mkdir /toolchains
RUN --mount=type=ssh git clone git@github.com:travisg/toolchains.git
RUN cd toolchains ; ./doit -f -a riscv32  > riscv32-build.log
# glob to make things cross platform.  Note 11.2.0 --> 11.3.0 (diff from instructions)
RUN cd toolchains ; cp -r riscv32-elf-11.3.0-Linux-* /toolchains/riscv32-11.3.0 

# qemu build and set up
RUN --mount=type=ssh git clone https://github.com/swetland/qemu.git
RUN cd qemu ; git checkout -b workshop origin/workshop 
RUN cd qemu ; ./configure --prefix=/toolchain/qemu --target-list=riscv32-softmmu 
RUN cd qemu ; make -j32  

# install dev tooling
RUN apt update \
  && apt-get install \
     vim \
     emacs \
     emacs-el \
  && apt-get clean autoclean && apt-get autoremove --yes && rm -rf /var/lib/apt/lists/*

COPY emacs.el $HOME/.emacs

# write config to /root/local.mk
RUN echo "XTOOLCHAIN := /usr/bin/riscv64-unknown-elf-" >> /toolchains/local.mk \  && echo "QEMU := /usr/bin/qemu-system-riscv32" >> /toolchains/local.mk 
