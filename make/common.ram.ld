
ENTRY(_start)

SECTIONS {
	/* RX code */
	.text : {
		__text_start = .;
		KEEP(*(.start))
		*(.text .text*)
		*(.gnu.linkonce.t.*)
	} > RAM

	/* RO data */
	.rodata : {
		. = ALIGN(4096);
		__rodata_start = .;
		*(.rodata .rodata.* .gnu.linkonce.r.*)
		. = ALIGN(4);
		__extra_start = .;
		KEEP(*(.extra .extra.*))
		. = ALIGN(4);
		__extra_end = .;
		__rodata_end = .;
	} > RAM

	/* collect any extra sections here */
	. = .;

	/* RW data */
	.data : {
		. = ALIGN(4096);
		__data_start = .;
		*(.data .data.* .gnu.linkonce.d.*)
		*(.got*)
		*(.dynamic)
		*(.sdata .sdata.*)
		. = ALIGN(4);
		__data_end = .;
	} > RAM

	.bss : {
		__bss_start = .;
		*(.sbss .sbss.*)
		*(.bss .bss.*)
		*(.gnu.linkonce.b.*)
		. = ALIGN(4);
		__bss_end = .;
	} > RAM

	. = ALIGN(4096);
	__image_end = .;

	/* initialize stack to top of memory */
	__memory_top = ORIGIN(RAM) + LENGTH(RAM);
}
