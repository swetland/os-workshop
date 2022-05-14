
ENTRY(_start)

SECTIONS {
	/* RX code */
	.text : {
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
		. = ALIGN(4);
		__data_end = .;
	} > RAM

	.bss : {
		__bss_start = .;
		*(.bss .bss.*)
		*(.gnu.linkonce.b.*)
		. = ALIGN(4);
		__bss_end = .;
	} > RAM
}
