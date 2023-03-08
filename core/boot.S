.set ALIGN,         1 << 0
.set MEMINFO,       1 << 1
// multiboot header data
.set MAGIC,         0xe85250d6
.set ARCHITECTURE,  0
.set HEADER_LENGHT, 16
.set CHECKSUM,      0x100000000 - (MAGIC + ARCHITECTURE + HEADER_LENGHT)

.section .multiboot
.align 4
.long MAGIC
.long ARCHITECTURE
.long HEADER_LENGHT
.long CHECKSUM

.section .bss
.align 16
stack_bottom:
.skip 16384 # 16 KiB
stack_top:

.section .text
.align 16
.global _start
.type _start, @function
_start:
    mov $stack_top, %esp
    call kmain

    cli
1:  hlt
    jmp 1b

.size _start, . - _start
