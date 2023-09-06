.code64
.section .text
.global load_gdt

gdtr:
	.word 0
	.quad 0

load_gdt:
	mov %di, gdtr
	mov %rsi, (gdtr+2)
	lgdt (gdtr)
	movw $0x10, %ax
	movw %ax, %ds
	movw %ax, %es
	movw %ax, %fs
	movw %ax, %gs
	movw %ax, %ss
	push $0x08
	lea .reload, %rax
	push %rax
	retfq
.reload:
	ret
