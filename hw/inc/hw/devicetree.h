#pragma once

#include <stdint.h>

typedef struct {
	uint32_t magic;
	uint32_t totalsize;
	uint32_t off_dt_struct;      // byte offset from header to struct
	uint32_t off_dt_strings;     // byte offset from header to strings
	uint32_t off_mem_rsvmap;     // byte offset from header to rsvmap
	uint32_t version;
	uint32_t last_comp_version;
	uint32_t boot_cpuid_phys;
	uint32_t size_dt_strings;    // byte len
	uint32_t size_dt_struct;     // byte len
} fdt_header_t;

#define FDT_MAGIC      0xd00dfeed

#define FDT_PADDING    0x00000000
#define FDT_BEGIN_NODE 0x00000001
#define FDT_END_NODE   0x00000002
#define FDT_PROP       0x00000003
#define FDT_NOP        0x00000004
#define FDT_END        0x00000009

typedef struct {
	uint32_t len;        // value len in bytes
	uint32_t nameoff;    // name offset from string table start
} fdt_prop;
