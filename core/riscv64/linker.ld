OUTPUT_ARCH(riscv)
ENTRY(_entry)

BASE_ADDRESS = 0x80400000;

SECTIONS
{
    /* Load the kernel at this address: "." means the current address */
    . = BASE_ADDRESS;
    . = ALIGN(4K);

    .trampoline : {
	    *(.text.trampoline)
    }

    . = ALIGN(4K);

    _vma_base = .; 
    _lma_base = LOADADDR(.trampoline) + SIZEOF(.trampoline);

    . += 0xFFFFFFFF00000000;

    .text : AT(_lma_base) {
        *(.text .text.*)
    }

    . = ALIGN(4K);
    rodata_start = .;
    .rodata : AT(LOADADDR(.text) + SIZEOF(.text)) {
        srodata = .;
        *(.rodata .rodata.*)
        erodata = .;
    }

    . = ALIGN(4K);
    data_start = .;
    .data : AT(LOADADDR(.rodata) + SIZEOF(.rodata)) {
        sdata = .;
        *(.data .data.*)
        edata = .;
    }

    . = ALIGN(4K);
    bss_start = .;
    .bss : AT(LOADADDR(.data) + SIZEOF(.data)) {
        *(.bss.stack)
        sbss_clear = .;
        *(.sbss .bss .bss.*)
        ebss_clear = .;
    }

    . = ALIGN(4K);
    PROVIDE(__memory_start = LOADADDR(.bss) + SIZEOF(.bss));
}
