// Copyright 2022, Brian Swetland <swetland@frotz.net>
// Licensed under the Apache License, Version 2.0

#define __SYSCALL(n,rtype,name,args) .globl name; name:; addi t0, x0, n; ecall; ret

#include <xos/syscall-tmpl.h>
