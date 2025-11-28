OUTPUT_ARCH(riscv)
ENTRY(_start)

BASE_ADDRESS = 0x80400000;

PHDRS
{
	trampoline PT_LOAD FLAGS(5); /* RX */
	text       PT_LOAD FLAGS(5); /* RX */
	data       PT_LOAD FLAGS(6); /* RW */
}

SECTIONS
{
	. = BASE_ADDRESS;

	.trampoline : {
		__trampoline_start = .;
		*(.text.trampoline)
		*(.data.trampoline)
		. = ALIGN(4096);
	} :trampoline

	_kernel_lma_start = .;
	. += 0xFFFFFFFF00000000;
	PROVIDE(__kernel_start = .);

	.text : AT(_kernel_lma_start) {
		_entry = .;
		*(.text .text.*)
	} :text

	. = ALIGN(4096);

	.rodata : AT(ALIGN(LOADADDR(.text) + SIZEOF(.text), 4096)) {
		srodata = .;
		*(.rodata .rodata.*)
		erodata = .;
	} :text

	. = ALIGN(4096);

	.data : AT(ALIGN(LOADADDR(.rodata) + SIZEOF(.rodata), 4096)) {
	sdata = .;
		*(.data .data.*)
		*(.sdata .sdata.*)
		edata = .;
	} :data

	. = ALIGN(4096);

	.bss : AT(ALIGN(LOADADDR(.data) + SIZEOF(.data), 4096)) {
		sbss = .;
		*(.bss .bss.*)
		*(.sbss .sbss.*)
		ebss = .;
	} :data

	. = ALIGN(4096);
	PROVIDE(__memory_start = .);
	PROVIDE(__kernel_end = .);
}
