.section .text
.global proc_snapshot
proc_snapshot:
	movq %rax, (%rdi)
	movq %rbx, 0x8(%rdi)
	movq %rcx, 0x10(%rdi)
	movq %rdx, 0x18(%rdi)
	movq %rsi, 0x20(%rdi)
	movq %rdi, 0x28(%rdi)
	movq %rsp, 0x30(%rdi)
	movq %rbp, 0x38(%rdi)
	movq %r8, 0x40(%rdi)
	movq %r9, 0x48(%rdi)
	movq %r10, 0x50(%rdi)
	movq %r11, 0x58(%rdi)
	movq %r12, 0x60(%rdi)
	movq %r13, 0x68(%rdi)
	movq %r14, 0x70(%rdi)
	movq %r15, 0x78(%rdi)
	ret
