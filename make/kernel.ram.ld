
MEMORY {
	RAM (rwx) : ORIGIN = 0xC0008000, LENGTH = (0x02000000 - 0x8000)
}

INCLUDE "make/common.ram.ld"
