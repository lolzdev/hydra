.section .data
gdtr:
	.word 0
	.quad 0

.section .text

.global load_gdt
load_gdt:
	mov %di, gdtr
	mov %rsi, (gdtr+2)
	lgdt (gdtr)
	movw $0x30, %ax
	movw %ax, %ds
	movw %ax, %es
	movw %ax, %fs
	movw %ax, %gs
	movw %ax, %ss
	push $0x28
	lea .reload, %rax
	push %rax
	retfq
.reload:
	ret
