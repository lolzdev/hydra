.code64
.section .text
.global enable_page_table

enable_page_table:
	mov %rdi, %rax
	mov %rax, %cr3

	/*
	mov %cr4, %rbx
	orq $1 << 5, %rbx
	mov %rbx, %cr4
	*/

	ret
