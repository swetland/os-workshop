// Copyright 2022, Brian Swetland <swetland@frotz.net>
// Licensed under the Apache License, Version 2.0

#include <hw/debug.h>
#include <hw/riscv.h>
#include <hw/devicetree.h>

#include <string.h>

static inline uint32_t swap(uint32_t n) {
	return (n >> 24) | (n << 24) |
		((n >> 8) & 0x0000FF00) |
		((n << 8) & 0x00FF0000);
}

int isstring(const char* s) {
	switch (s[0]) {
	case 'b':
		if (!strcmp(s, "bootargs")) return 1;
	case 'c':
		if (!strcmp(s, "compatible")) return 1;
		break;
	case 'd':
		if (!strcmp(s, "device_type")) return 1;
		break;
	case 'm':
		if (!strcmp(s, "model")) return 1;
		if (!strcmp(s, "mmu-type")) return 1;
		break;
	case 'r':
		if (!strcmp(s, "riscv.isa")) return 1;
		break;
	}
	return 0;
}

void xindent(unsigned count) {
	while (count-- > 0) {
		xputs("  ");
	}
}

void fdt_walk(fdt_header_t* fdt) {
	uint32_t* data = (void*) (((uintptr_t) fdt) + swap(fdt->off_dt_struct));
	char* strtab = (char*) (((uintptr_t) fdt) + swap(fdt->off_dt_strings));
	unsigned indent = 0;
	for (;;) {
		switch(swap(*data++)) {
		case FDT_PADDING:
		case FDT_NOP:
			continue;
		case FDT_BEGIN_NODE: {
			char *name = (void*) data;
			uint32_t len = strlen(name) + 1;	
			xindent(indent);
			xprintf("NODE '%s'\n", name);
			indent++;
			data += ((len + 3) >> 2);
			break;
		}
		case FDT_END_NODE:
			indent--;
			break;
		case FDT_PROP: {
			uint32_t len = swap(*data++);
			uint32_t name = swap(*data++);
			xindent(indent);
			xprintf("PROP '%s' %u:", strtab + name, len);
			if (isstring(strtab + name)) {
				xprintf(" '%s'\n", (char*) data);
			} else {
				for (unsigned n = 0; n < len; n++) {
					xprintf(" %02x", ((uint8_t*)data)[n]);
				}
				xprintf("\n");
			}
			data += ((len + 3) >> 2);
			break;
		}
		default:
			xprintf("HUH %08x\n", swap(data[-1]));
			return;
		case FDT_END:
			xprintf("END\n");
			return;
		}
	}
}

void start(unsigned _, fdt_header_t* fdt) {
	xprintf("fdt         %p\n", fdt);
	xprintf("fdt.magic   %08x\n", swap(fdt->magic));
	xprintf("fdt.version %08x\n", swap(fdt->version));
	xprintf("fdt.struct  %08x %08x\n",
		swap(fdt->off_dt_struct), swap(fdt->size_dt_struct));
	xprintf("fdt.strings %08x %08x\n",
		swap(fdt->off_dt_strings), swap(fdt->size_dt_strings));

	fdt_walk(fdt);

	for (unsigned n = 0x10001000; n < 0x10009000; n+= 0x1000) {
		unsigned *x = (void*) n;
		xprintf("%08x: %08x %08x %08x %08x\n",
			n, x[0], x[1], x[2], x[3]);
	}
}
