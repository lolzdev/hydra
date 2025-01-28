.section .text

.global pt_load
pt_load:
	mov $0x000ffffffffff000, %rax
    and %rdi, %rax
    mov %rdi, %cr3
    ret
