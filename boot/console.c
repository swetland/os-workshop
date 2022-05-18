// Copyright 2022, Brian Swetland <swetland@frotz.net>
// Licensed under the Apache License, Version 2.0

#include <hw/riscv.h>
#include <hw/intrinsics.h>
#include <hw/context.h>
#include <hw/debug.h>
#include "boot.h"
#include <string.h>

int atoi(const char *s) {
	unsigned c;
	int n = 0;
	while ((c = *s++)) {
		if ((c >= '0') && (c <= '9')) {
			n = n * 10 + (c - '0');
		} else {
			break;
		}
	}
	return n;
}

uint32_t atox(const char *s) {
	uint32_t n = 0;
	uint32_t c;
	while ((c = *s++)) {
		switch (c) {
		case 'a': case 'b': case 'c': case 'd': case 'e': case 'f':
			c = c - 'a' + 10;
			break;
		case 'A': case 'B': case 'C': case 'D': case 'E': case 'F':
			c = c - 'A' + 10;
			break;
		case '0': case '1': case '2': case '3': case '4':
		case '5': case '6': case '7': case '8': case '9':
			c = c - '0';
			break;
		default:
			return n;
		}
		n = (n << 4) | c;
	}
	return n;
}

extern eframe_t* EF;
extern volatile int STOP;


// args: x hex number
//       i integer number
//       s string
typedef struct {
	const char* name;
	const char* args;
	const char* help;
	void *fn;
} console_cmd_t;

void do_regs(void) {
	eframe_t* ef = EF;
	xprintf("pc %08x ra %08x sp %08x gp %08x  sstatus  %08x\n",
		ef->pc, ef->ra, ef->sp, ef->gp, csr_read(CSR_SSTATUS));
	xprintf("tp %08x t0 %08x t1 %08x t2 %08x  scause   %08x\n",
		ef->tp, ef->t0, ef->t1, ef->t2, csr_read(CSR_SCAUSE));
	xprintf("fp %08x s1 %08x a0 %08x a1 %08x  stval    %08x\n",
		ef->s0, ef->s1, ef->a0, ef->a1, csr_read(CSR_STVAL));
	xprintf("a2 %08x a3 %08x a4 %08x a5 %08x  stvec    %08x\n",
		ef->a2, ef->a3, ef->a4, ef->a5, csr_read(CSR_STVEC));
	xprintf("a6 %08x a7 %08x s2 %08x s3 %08x  sepc     %08x\n",
		ef->a6, ef->a7, ef->s2, ef->s3, csr_read(CSR_SEPC));
	xprintf("s4 %08x s5 %08x s6 %08x s7 %08x  sscratch %08x\n",
		ef->s4, ef->s5, ef->s6, ef->s7, csr_read(CSR_SSCRATCH));
	xprintf("s8 %08x s9 %08x 10 %08x 11 %08x  satp     %08x\n",
		ef->s8, ef->s9, ef->s10, ef->s11, csr_read(CSR_SATP));
	xprintf("t3 %08x t4 %08x t5 %08x t6 %08x  sip      %08x\n",
		ef->t3, ef->t4, ef->t5, ef->t6, csr_read(CSR_SIP));
}

void do_help(void);

void do_dw(unsigned n, unsigned addr, unsigned count) {
	if (n < 2) count = 1;
	if (count > 1024) count = 1024;

	n = 0;
	while (n < count) {
		if ((n & 3) == 0) {
			xprintf("\n%08x:", addr);
		}
		xprintf(" %08x", rd32safe(addr));
		addr += 4;
		n++;
	}
	if ((n & 3) == 0) {
		xprintf("\n");
	}
}

void do_db(unsigned n, unsigned addr, int count) {
	char txt[17];
	if (count < 0) {
		return;
	}
	txt[16] = 0;
	while (count > 0) {
		xprintf("%08x:", addr);
		for (int i = 0; i < 16; i++) {
			uint32_t c;
			if (i < count) {
				c = rd8safe(addr + i) & 0xFF;
				xprintf(" %02x", c);
				if ((c < ' ') || (c > 126)) c = '.';
			} else {
				xprintf("   ");
				c = ' ';
			}
			txt[i] = c;
		}
		xprintf("  %s\n", txt);
		addr += 16;
		count -= 16;
	}
}

void do_stop(void) { STOP = 1; }
void do_cont(void) { STOP = 0; };

console_cmd_t CMDS[] = {
	{ "help", "", "help", do_help },
	{ "regs", "", "dump registers", do_regs },
	{ "dw", "xx", "dump words <addr> <count>", do_dw },
	{ "db", "xx", "dump bytes <addr> <count>", do_db },
	{ "stop", "", "stop execution", do_stop },
	{ "cont", "", "continue execution", do_cont },
};

void do_help(void) {
	for (unsigned n = 0; n < (sizeof(CMDS)/sizeof(CMDS[0])); n++) {
		xprintf("%-16s %s\n", CMDS[n].name, CMDS[n].help);
	}
}

void console_cmd(console_cmd_t *cmd, char *args[], unsigned argc) {
	uintptr_t argx[4];
	unsigned max = strlen(cmd->args);

	if ((argc > max) || (argc > 4)) {
		xprintf("error: too many arguments\n");
		return;
	}

	memset(argx, 0, sizeof(argx));
	for (unsigned n = 0; n < max; n++) {
		switch (cmd->args[n]) {
		case 'x':
			argx[n] = (n < argc) ? atox(args[n]) : 0;
			break;
		case 'i':
			argx[n] = (n < argc) ? atoi(args[n]) : 0;
			break;
		case 's':
			argx[n] = (n < argc) ? (uintptr_t) args[n] : (uintptr_t) "";
			break;
		default:
			xprintf("error: internal: bad args '%s'\n", cmd->args);
			return;
		}
	}

	console_call_cmd(cmd->fn, argx[0], argx[1], argx[2], argx[3], argc);
}

#define MAXTOK 16

void console_line(char* line) {
	char *token[MAXTOK];
	unsigned toklen = 0;

	while (toklen < MAXTOK) {
		while(*line == ' ') {
			if (*line == 0) {
				goto done;
			}
			*line++ = 0;
		}
		token[toklen] = line;
		while (*line != ' ') {
			if (*line == 0) {
				if (token[toklen] != line) {
					*line = 0;
					toklen++;
				}
				goto done;
			}
			line++;
		}
		toklen++;
	}
done:
	if (toklen) {
		for (unsigned n = 0; n < (sizeof(CMDS)/sizeof(CMDS[0])); n++) {
			if (!strcmp(CMDS[n].name, token[0])) {
				console_cmd(CMDS + n, token + 1, toklen - 1);
				goto prompt;
			}
		}
		xprintf("unknown command '%s'\n", token[0]);
	}
prompt:
	xputs("monitor> ");
}



