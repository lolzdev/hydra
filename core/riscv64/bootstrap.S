.section .text
.globl _entry
_entry:
	mv tp, a0
	la sp, boot_stack_top
	call kmain
loop:
	wfi
	j loop

.globl _smp_entry
_smp_entry:
	mv sp, a1
	mv tp, a0
	call kmain_smp

	j loop
.section .bss.stack
.align 12
.globl boot_stack
boot_stack:
	.space 4096 * 4 * 4
.globl boot_stack_top
boot_stack_top:
