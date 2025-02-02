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
.extern CURRENT_PROC 
.extern syscall_entry
.extern tss_set_rsp0
.global sched_switch
sched_switch:
	pushq %rdi
	pushq %rdx
	mov %rsp, %rdi
	mov %rsp, %r15
	call tss_set_rsp0


	/* Enable syscall instruction by setting SCE bit
	 * in the EFER register.
	 */
	mov $0xc0000080, %rcx
	rdmsr
	or $0x1, %eax
	wrmsr

	/* Set the LSTAR register to `syscall_entry`
	 * address so that when a syscall happens
	 * the %rip register is set to the handler.
	 */
	mov $0xC0000082, %rcx
	mov $syscall_entry, %rax
	mov %rax, %rdx
	shr $32, %rdx
	mov $0xffffffff, %r9
	and %r9, %rdx
	wrmsr

	/* Load kernel's code sector (0x10) in the
	 * STAR register's bits 63:48. */
	mov $0xC0000081, %rcx
	rdmsr
	mov $0x8, %edx
	shl $0x10, %edx
	wrmsr

	popq %rdx
	popq %rdi

	/* Set the %rcx register to the entry point
	 * for user code (first parameter). `sysret`
	 * will then use %rcx to set the %rip
	 * register once in userspace.
	 */
	mov %rdi, %rcx

	/* Setup the stack for the userspace process. */
	movq %rdx, %rbp
	movq %rdx, %rsp

	/* Loade the user process page table before
	 * jumping to userspace.
	 */
	mov $0xffffffff, %r9
	and %r9, %rsi
	mov %rsi, %cr3
	
	/* Enable interrupts */
	mov $0x202, %r11
	sysretq
