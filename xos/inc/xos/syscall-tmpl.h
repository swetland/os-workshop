// Copyright 2022, Brian Swetland <swetland@frotz.net>
// Licensed under the Apache License, Version 2.0

__SYSCALL(0,void,exit,(int n))
__SYSCALL(1,void,xputc,(unsigned ch))
__SYSCALL(2,status_t,thread_create,(handle_t *out, void *entry, void *arg))

#define SYSCALL_COUNT 3
