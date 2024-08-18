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

__attribute__((aligned(0x10)))
static idt_entry_t IDT[256];
static idtr_t IDTR;

void idt_encode_entry(idt_entry_t *entry, uint64_t isr, uint16_t selector, uint8_t flags)
{
	entry->offset_low = (uint64_t) isr & 0xffff;
	entry->selector = selector;
	entry->ist = 0x0;
	entry->attributes = flags;
	entry->offset_mid = ((uint64_t) isr >> 16) & 0xffff;
	entry->offset_high = ((uint64_t) isr >> 32) & 0xffffffff;
	entry->zero = 0x0;
}

void idt_init(void)
{
	IDTR.base = (uintptr_t) &IDT[0];
	IDTR.limit = ((uint16_t) sizeof(idt_entry_t) * 256) - 1;

	for (int i=0; i < 32; i++) {
		idt_encode_entry(&IDT[i], (uint64_t) int_void, 0x08, 0x8e);
	}

	idt_encode_entry(&IDT[0], (uint64_t)int_division_by_zero, 0x08, 0x8e);
	idt_encode_entry(&IDT[3], (uint64_t)int_breakpoint, 0x08, 0x8e);
	idt_encode_entry(&IDT[13], (uint64_t)int_gpf, 0x08, 0x8e);
	idt_encode_entry(&IDT[14], (uint64_t)int_page_fault, 0x08, 0x8e);
	idt_encode_entry(&IDT[32], (uint64_t)int_systimer, 0x08, 0x8e);
	idt_encode_entry(&IDT[33], (uint64_t)int_keyboard, 0x08, 0x8e);

	__asm__ volatile ("lidt %0" : : "m"(IDTR));
	__asm__ volatile ("sti");
}
