// Copyright 2022, Brian Swetland <swetland@frotz.net>
// Licensed under the Apache License, Version 2.0

#pragma once
#include <xos/status.h>
#include <xos/types.h>

#define __SYSCALL(n,rtype,name,args) rtype name args;
#include <xos/syscall-tmpl.h>
#undef __SYSCALL
