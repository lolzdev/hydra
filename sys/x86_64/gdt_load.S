.section .data
gdtr:
	.word 0
	.quad 0

.section .text

.global gdt_load
gdt_load:
	mov %di, gdtr
	mov %rsi, (gdtr+2)
	lgdt (gdtr)
	movw $0x20, %ax
	movw %ax, %ds
	movw %ax, %es
	movw %ax, %fs
	movw %ax, %gs
	movw %ax, %ss
	push $0x8
	lea .reload, %rax
	push %rax
	lretq
.reload:
	mov $0x28, %ax
	ltr %ax
	ret
