#include <arch/x86_64-elf/idt.h>

__attribute__((aligned(0x10)))
static idt_entry_t IDT[256];
static idtr_t IDTR;

void encode_idt_entry(idt_entry_t *entry, uint64_t isr, uint16_t selector, uint8_t flags)
{
	entry->offset_low = (uint64_t) isr & 0xffff;
	entry->selector = selector;
	entry->ist = 0x0;
	entry->attributes = flags;
	entry->offset_mid = ((uint64_t) isr >> 16) & 0xffff;
	entry->offset_high = ((uint64_t) isr >> 32) & 0xffffffff;
	entry->zero = 0x0;
}

void init_idt()
{
	IDTR.base = (uintptr_t) &IDT[0];
	IDTR.limit = ((uint16_t) sizeof(idt_entry_t) * 256) - 1;

	for (int i=0; i < 32; i++) {
		encode_idt_entry(&IDT[i], (uint64_t) int_void, 0x28, 0x8e);
	}

	encode_idt_entry(&IDT[0], (uint64_t)int_division_by_zero, 0x28, 0x8e);
	encode_idt_entry(&IDT[3], (uint64_t)int_breakpoint, 0x28, 0x8e);
	encode_idt_entry(&IDT[13], (uint64_t)int_gpf, 0x28, 0x8e);
	encode_idt_entry(&IDT[14], (uint64_t)int_page_fault, 0x28, 0x8e);
	encode_idt_entry(&IDT[32], (uint64_t)int_systimer, 0x28, 0x8e);
	encode_idt_entry(&IDT[33], (uint64_t)int_keyboard, 0x28, 0x8e);

	__asm__ volatile ("lidt %0" : : "m"(IDTR));
	__asm__ volatile ("sti");
}
