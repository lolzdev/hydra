OUTPUT_ARCH("riscv")
ENTRY(_start)

SECTIONS
{
	. = 0x1000;

	.text : {
		*(.text.entry) 
		*(.text .text.*)
	}

	. = ALIGN(16);
	.rodata : {
		*(.rodata .rodata.*)
	}

	. = ALIGN(4096); 

	.data : {
		*(.data .data.*)
	}

	. = ALIGN(16);
	.sdata : {
		__global_pointer$ = . + 0x800;
		*(.sdata .sdata.*)
	}

	. = ALIGN(16);
	.bss : {
		__bss_start = .;
		*(.bss .bss.*)
		*(.sbss .sbss.*)
		__bss_end = .;
	}

	_end = .;
}
