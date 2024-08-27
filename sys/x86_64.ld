OUTPUT_FORMAT(elf64-x86-64)

ENTRY(_start)

PHDRS
{
	hydra_tags 	PT_LOAD;
    text       	PT_LOAD;
    rodata  	PT_LOAD;
    data    	PT_LOAD;
    dynamic 	PT_DYNAMIC;
}

SECTIONS
{
    . = 2M;

    .text : {
        *(.text .text.*)
    } :text

    . = ALIGN(CONSTANT(MAXPAGESIZE));

    .rodata : {
        *(.rodata .rodata.*)
    } :rodata

    . = ALIGN(CONSTANT(MAXPAGESIZE));

	.hydra_tags : {
		*(.hydra_tags)
	} :hydra_tags

    .data : {
        *(.data .data.*)
    } :data

    .bss : {
        *(.bss .bss.*)
        *(COMMON)
    } :data

    /DISCARD/ : {
        *(.eh_frame*)
        *(.note .note.*)
    }

	. = ALIGN(CONSTANT(MAXPAGESIZE));
}
