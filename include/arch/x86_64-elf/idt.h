#ifndef IDT_H
#define IDT_H

#include <arch/x86_64-elf/interrupts.h>
#include <stdint.h>

typedef struct {
   uint16_t offset_low;
   uint16_t selector;
   uint8_t  ist;
   uint8_t  attributes;
   uint16_t offset_mid;
   uint32_t offset_high;
   uint32_t zero;
} __attribute__((packed)) idt_entry_t;

typedef struct {
	uint16_t limit;
	uint64_t base;
} __attribute__((packed)) idtr_t;

void encode_idt_entry(idt_entry_t *entry, uint64_t isr, uint16_t selector, uint8_t flags);
void init_idt();

#endif
