.section .text
.globl _entry
_entry:
	la sp, boot_stack_top
	call kmain
loop:
	j loop

.section .bss.stack
.align 12
.globl boot_stack
boot_stack:
	.space 4096 * 4 * 4
.globl boot_stack_top
boot_stack_top:
