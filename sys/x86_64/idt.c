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

#include <x86_64/idt.h>
#include <x86_64/trap.h>
#include <sched.h>
#include <x86_64/inst.h>
#include "gdt.h"

__attribute__((aligned(0x10)))
static idt_entry_t IDT[256];
static idtr_t IDTR;
typedef void (*idt_function)(struct interrupt_frame*);
idt_function functions[256];

extern idt_function isrHandler0(struct interrupt_frame*);
extern idt_function isrHandler1(struct interrupt_frame*);
extern idt_function isrHandler2(struct interrupt_frame*);
extern idt_function isrHandler3(struct interrupt_frame*);
extern idt_function isrHandler4(struct interrupt_frame*);
extern idt_function isrHandler5(struct interrupt_frame*);
extern idt_function isrHandler6(struct interrupt_frame*);
extern idt_function isrHandler7(struct interrupt_frame*);
extern idt_function isrHandler8(struct interrupt_frame*);
extern idt_function isrHandler9(struct interrupt_frame*);
extern idt_function isrHandler10(struct interrupt_frame*);
extern idt_function isrHandler11(struct interrupt_frame*);
extern idt_function isrHandler12(struct interrupt_frame*);
extern idt_function isrHandler13(struct interrupt_frame*);
extern idt_function isrHandler14(struct interrupt_frame*);
extern idt_function isrHandler15(struct interrupt_frame*);
extern idt_function isrHandler16(struct interrupt_frame*);
extern idt_function isrHandler17(struct interrupt_frame*);
extern idt_function isrHandler18(struct interrupt_frame*);
extern idt_function isrHandler19(struct interrupt_frame*);
extern idt_function isrHandler20(struct interrupt_frame*);
extern idt_function isrHandler21(struct interrupt_frame*);
extern idt_function isrHandler22(struct interrupt_frame*);
extern idt_function isrHandler23(struct interrupt_frame*);
extern idt_function isrHandler24(struct interrupt_frame*);
extern idt_function isrHandler25(struct interrupt_frame*);
extern idt_function isrHandler26(struct interrupt_frame*);
extern idt_function isrHandler27(struct interrupt_frame*);
extern idt_function isrHandler28(struct interrupt_frame*);
extern idt_function isrHandler29(struct interrupt_frame*);
extern idt_function isrHandler30(struct interrupt_frame*);
extern idt_function isrHandler31(struct interrupt_frame*);
extern idt_function isrHandler32(struct interrupt_frame*);
extern idt_function isrHandler33(struct interrupt_frame*);
extern idt_function isrHandler39(struct interrupt_frame*);

idt_entry_t idt_encode_entry(uint64_t isr, uint16_t selector, uint8_t flags)
{
	idt_entry_t entry;
	entry.offset_low = (uint64_t) isr & 0xffff;
	entry.selector = selector;
	entry.ist = 0x0;
	entry.attributes = flags | 1;
	entry.offset_mid = ((uint64_t) isr >> 16) & 0xffff;
	entry.offset_high = ((uint64_t) isr >> 32) & 0xffffffff;
	entry.zero = 0x0;
	return entry;
}

static void initEntries(){
	IDT[0] = idt_encode_entry((uint64_t)isrHandler0, 0x08, 0x8e);
	IDT[1] = idt_encode_entry((uint64_t)isrHandler1, 0x08, 0x8e);
	IDT[2] = idt_encode_entry((uint64_t)isrHandler2, 0x08, 0x8e);
	IDT[3] = idt_encode_entry((uint64_t)isrHandler3, 0x08, 0x8e);
	IDT[4] = idt_encode_entry((uint64_t)isrHandler4, 0x08, 0x8e);
	IDT[5] = idt_encode_entry((uint64_t)isrHandler5, 0x08, 0x8e);
	IDT[6] = idt_encode_entry((uint64_t)isrHandler6, 0x08, 0x8e);
	IDT[7] = idt_encode_entry((uint64_t)isrHandler7, 0x08, 0x8e);
	IDT[8] = idt_encode_entry((uint64_t)isrHandler8, 0x08, 0x8e);
	IDT[9] = idt_encode_entry((uint64_t)isrHandler9, 0x08, 0x8e);
	IDT[10] = idt_encode_entry((uint64_t)isrHandler10, 0x08, 0x8e);
	IDT[11] = idt_encode_entry((uint64_t)isrHandler11, 0x08, 0x8e);
	IDT[12] = idt_encode_entry((uint64_t)isrHandler12, 0x08, 0x8e);
	IDT[13] = idt_encode_entry((uint64_t)isrHandler13, 0x08, 0x8e);
	IDT[14] = idt_encode_entry((uint64_t)isrHandler14, 0x08, 0x8e);
	IDT[15] = idt_encode_entry((uint64_t)isrHandler15, 0x08, 0x8e);
	IDT[16] = idt_encode_entry((uint64_t)isrHandler16, 0x08, 0x8e);
	IDT[17] = idt_encode_entry((uint64_t)isrHandler17, 0x08, 0x8e);
	IDT[18] = idt_encode_entry((uint64_t)isrHandler18, 0x08, 0x8e);
	IDT[19] = idt_encode_entry((uint64_t)isrHandler19, 0x08, 0x8e);
	IDT[20] = idt_encode_entry((uint64_t)isrHandler20, 0x08, 0x8e);
	IDT[21] = idt_encode_entry((uint64_t)isrHandler21, 0x08, 0x8e);
	IDT[22] = idt_encode_entry((uint64_t)isrHandler22, 0x08, 0x8e);
	IDT[23] = idt_encode_entry((uint64_t)isrHandler23, 0x08, 0x8e);
	IDT[24] = idt_encode_entry((uint64_t)isrHandler24, 0x08, 0x8e);
	IDT[25] = idt_encode_entry((uint64_t)isrHandler25, 0x08, 0x8e);
	IDT[26] = idt_encode_entry((uint64_t)isrHandler26, 0x08, 0x8e);
	IDT[27] = idt_encode_entry((uint64_t)isrHandler27, 0x08, 0x8e);
	IDT[28] = idt_encode_entry((uint64_t)isrHandler28, 0x08, 0x8e);
	IDT[29] = idt_encode_entry((uint64_t)isrHandler29, 0x08, 0x8e);
	IDT[30] = idt_encode_entry((uint64_t)isrHandler30, 0x08, 0x8e);
	IDT[31] = idt_encode_entry((uint64_t)isrHandler31, 0x08, 0x8e);
	IDT[32] = idt_encode_entry((uint64_t)isrHandler32, 0x08, 0x8e);
	IDT[33] = idt_encode_entry((uint64_t)isrHandler33, 0x08, 0x8e);
	IDT[39] = idt_encode_entry((uint64_t)isrHandler39, 0x08, 0x8e);
}

static void pic_remap() {
    __outb(PIC1_COMMAND, 0x11);
    __outb(PIC2_COMMAND, 0x11);
    
    __outb(PIC1_DATA, 0x20);
    __outb(PIC2_DATA, 0x28);
    
    __outb(PIC1_DATA, 0x04);
    __outb(PIC2_DATA, 0x02);

    __outb(PIC1_DATA, 0x01);
    __outb(PIC2_DATA, 0x01);

    __outb(PIC1_DATA, 0xFE);
    __outb(PIC2_DATA, 0xFF);
}

void idt_init(void)
{
	
	IDTR.base = (uintptr_t) IDT;
	IDTR.limit = ((uint16_t) sizeof(idt_entry_t) * 256) - 1;
	__asm__ volatile ("lidt %0" : : "m"(IDTR));
	pic_remap();
	initEntries();
	functions[0] = int_division_by_zero;
	functions[3] = int_breakpoint;
	functions[4] = int_overflow;
	functions[6] = int_bad_opcode;
	functions[8] = int_double_fault;
	functions[10] = int_invalid_tss;
	functions[11] = int_no_segment;
	functions[12] = int_stack_segment;
	functions[13] = int_gpf;
	functions[14] = int_page_fault;
	functions[32] = int_void;
	functions[33] = int_keyboard;
	functions[39] = int_systimer;
}
