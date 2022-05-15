
#include "ex25-syscalls.h"

.globl exit
exit:
	li t0, SYS_EXIT
	ecall
	ret

.globl xputc
xputc:
	li t0, SYS_XPUTC
	ecall 
	ret
