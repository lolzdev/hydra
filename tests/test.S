.section .text
.global _start
_start:
	mov $0x1, %rax
	mov $str, %rdi
	syscall
	mov $0x1, %rax
	mov $msg, %rdi
	syscall
loop:
	jmp loop
str:
	.ascii "hello from userspace!\n\0"
msg:
	.ascii "hello again from userspace!\n\0"
