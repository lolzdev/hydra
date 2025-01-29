.section .text
.global _start
_start:
	mov $0x1, %rax
	mov $str, %rdi
	syscall
loop:
	jmp loop
str:
	.ascii "hello from userspace!\n\0"
