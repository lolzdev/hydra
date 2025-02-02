.section .text
.global _start
_start:
	pushq $0x1
	popq %rax
	mov $str, %rdi
	syscall
	mov $0x1, %rax
	mov $msg, %rdi
	syscall
loop:
	jmp loop
str:
	.ascii "hello from userspace! 2\n\0"
msg:
	.ascii "hello again from userspace! 2\n\0"
