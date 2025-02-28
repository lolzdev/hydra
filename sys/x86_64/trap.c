	/*-
	 * Copyright 2024 Lorenzo Torres
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

#include <x86_64/trap.h>
#include <log/fb.h>

void printRfl(uint64_t rflags){
	if (rflags & 0x00000001)
		kprintf("CF ");
	if (rflags & 0x00000004)
		kprintf("PF ");
	if (rflags & 0x00000010)
		kprintf("AF ");
	if (rflags & 0x00000040)
		kprintf("ZF ");
	if (rflags & 0x00000080)
		kprintf("SF ");
	if (rflags & 0x00000100)
		kprintf("TF ");
	if (rflags & 0x00000200)
		kprintf("IF ");
	if (rflags & 0x00000400)
		kprintf("DF ");
	if (rflags & 0x00000800)
		kprintf("OF ");
	if (rflags & 0x00010000)
		kprintf("RF ");
	if (rflags & 0x00020000)
		kprintf("VM ");
	if (rflags & 0x00040000)
		kprintf("AC ");
	if (rflags & 0x00080000)
		kprintf("VIF ");
	if (rflags & 0x00100000)
		kprintf("VIP ");
	if (rflags & 0x00200000)
		kprintf("ID ");
	if (rflags & 0x80000000)
		kprintf("AI ");
	kprintf("\n");
}
void printRegs(struct interrupt_frame* regs){
	uintptr_t cr2;
	__asm__ volatile ("mov %%cr2, %0" : "=r"(cr2));
	kprintf("\tv=0x%x e=0x%x\n", regs->interrupt_number, regs->error_code);
	kprintf("RAX=0x%x RBX=0x%x RCX=0x%x RDX=0x%x\n", regs->rax, regs->rbx, regs->rcx, regs->rdx);
	kprintf("RSI=0x%x RDI=0x%x RBP=0x%x RSP=0x%x\n", regs->rsi, regs->rdi, regs->rbp, regs->rsp);
	kprintf("R8 =0x%x R9 =0x%x R10=0x%x R11=0x%x\n", regs->r8, regs->r9, regs->r10, regs->r11);
	kprintf("R12=0x%x R13=0x%x R14=0x%x R15=0x%x\n", regs->r12, regs->r13, regs->r14, regs->r15);
	kprintf("RIP=0x%x RFL=", regs->rip); printRfl(regs->rflags);
	kprintf("ES =0x%02.2llx\n", regs->es);
	kprintf("CS =0x%02.2llx\n", regs->cs);
	kprintf("SS =0x%02.2llx\n", regs->ss);
	kprintf("DS =0x%02.2llx\n", regs->ds);
	kprintf("FS =0x%02.2llx\n", regs->fs);
	kprintf("GS =0x%02.2llx\n", regs->gs);
	kprintf("CR2=0x%x ORIGINAL_RSIP=0x%x\n", cr2, regs->orig_rsp);
}

void int_division_by_zero(struct interrupt_frame *frame)
{
	kprintf("panic: division by 0 at: 0x%x\n", frame->rip);
	printRegs(frame);
}


void int_breakpoint(struct interrupt_frame *frame)
{
	kprintf("panic: breakpoint at: 0x%x\n", frame->rip);
	printRegs(frame);
}

typedef struct __attribute__((packed)) PageFaultError{
	uint8_t PPV   : 1; // 1=PPV   0=NP
	uint8_t write : 1; // 1=Write 0=Read
	uint8_t user  : 1; // 1=CPL3  0=CPL0
	uint8_t rsvw  : 1; // Reserved field write
	uint8_t insF  : 1; // Instruction fetch
	uint8_t PKV   : 1; // Protection key violation
	uint8_t SS    : 1; // Shadow stack
	uint8_t reserved0;
	uint8_t SGX   : 1; // Software guard extension
	uint16_t reserved1;
} PageFaultError;

void int_page_fault(struct interrupt_frame *frame)
{
	kprintf("panic: page fault at: 0x%x\n", frame->rip);
	uintptr_t cr2;
	__asm__ volatile ("mov %%cr2, %0" : "=r"(cr2));
	kprintf("error code: 0x%x, address: 0x%x\n", frame->error_code, cr2);
	printRegs(frame);
	while(1);
}

void int_gpf(struct interrupt_frame *frame)
{
	kprintf("panic: general protection fault at: 0x%x\n", frame->rip);
	printRegs(frame);
	while(1);
}

void int_overflow(struct interrupt_frame *frame)
{
	kprintf("panic: overflow at: 0x%x\n", frame->rip);
	printRegs(frame);
	while(1);
}

void int_bad_opcode(struct interrupt_frame *frame)
{
	kprintf("panic: invalid opcode at: 0x%x\n", frame->rip);
	printRegs(frame);
	while(1);
}

void int_double_fault(struct interrupt_frame *frame)
{
	kprintf("panic: double fault at: 0x%x\n", frame->rip);
	printRegs(frame);
	while(1);
}

void int_invalid_tss(struct interrupt_frame *frame)
{
	kprintf("panic: double fault at: 0x%x\n", frame->rip);
	printRegs(frame);
	while(1);
}

void int_no_segment(struct interrupt_frame *frame)
{
	kprintf("panic: segment not present at: 0x%x\n", frame->rip);
	printRegs(frame);
	while(1);
}

void int_stack_segment(struct interrupt_frame *frame)
{
	kprintf("panic: stack segment fault at: 0x%x\n", frame->rip);
	printRegs(frame);
	while(1);
}

void int_virtualization(struct interrupt_frame *frame)
{
	kprintf("panic: virtualization exception at: 0x%x\n", frame->rip);
	printRegs(frame);
	while(1);
}

void int_security(struct interrupt_frame *frame)
{
	kprintf("panic: security exception at: 0x%x\n", frame->rip);
	printRegs(frame);
	while(1);
}

void int_systimer(struct interrupt_frame *frame)
{
	(void)frame;
	kprintf("apic timer\n");
}

void int_keyboard(struct interrupt_frame *frame)
{
	(void)frame;

}

void int_void(struct interrupt_frame *frame)
{
	(void)frame;

}
