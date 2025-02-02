	/*-
	 * Copyright 2025 Lorenzo Torres
 	 *
	 * Redistribution and use in source and binary forms, with or without
	 * modification, are permitted provided that the following conditions are met:
	 * 1. Redistributions of source code must retain the above copyright
	 *    notice, this list of conditions and the following disclaimer.
	 * 2. Redistributions in binary form must reproduce the above copyright
	 *    notice, this list of conditions and the following disclaimer in the
	 *    documentation and/or other materials provided with the distribution.
	 * 3. Neither the name of the copyright holder nor
	 *    the names of its contributors may be used to endorse or promote products
	 *    derived from this software without specific prior written permission.
	 *
	 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS “AS IS” AND ANY
	 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
	 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
	 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY
	 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
	 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
	 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
	 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
	 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
	 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
	 */

.section .text
.extern k_page_table
.extern syscall_handler
.extern CURRENT_PROC 
.extern KERNEL_PROC 
.global syscall_entry
syscall_entry:
	mov k_page_table, %r9
	mov $0xffffffff, %r8
	and %r8, %r9
	mov %r9, %cr3

	movq KERNEL_PROC, %r9
	movq 0x40(%r9), %rbp
	movq 0x30(%r9), %rsp
	swapgs

	pushq %rcx
	pushq %r11
	call syscall_handler
	popq %r11
	popq %rcx
	
	movq CURRENT_PROC, %r9
	movq 0x38(%r9), %rbp
	movq 0x30(%r9), %rsp
	movq 0x80(%r9), %r8
	mov $0xffffffff, %r9
	and %r9, %r8
	mov %r8, %cr3

	swapgs
	sysretq

.global hpet_int
hpet_int:
	push %rbp
	mov %rsp,%rbp
	push %r11
	push %r10
	push %r9
	push %r8
	push %rdi
	push %rsi
	push %rdx
	push %rcx
	push %rax

	mov $0xcafebab, %r8
	mov $0x20, %rax
	outb $0x20

	pop %rax
	pop %rcx
	pop %rdx
	pop %rsi
	pop %rdi
	pop %r8
	pop %r9
	pop %r10
	pop %r11
	pop %rbp

	iretq
