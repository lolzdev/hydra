#ifndef INTERRUPTS_H
#define INTERRUPTS_H

#include <stdio.h>
#include <stdint.h>
#include <stddef.h>

struct interrupt_frame {
	uintptr_t ip;
	uint64_t cs;
	uint64_t cpu_flags;
	uintptr_t sp;
	uint64_t ss;
} __attribute__((packed));

void int_division_by_zero(struct interrupt_frame *frame);
void int_breakpoint(struct interrupt_frame *frame);
void int_page_fault(struct interrupt_frame *frame, uint64_t error);
void int_gpf(struct interrupt_frame *frame);
void int_systimer(struct interrupt_frame *frame);
void int_keyboard(struct interrupt_frame *frame);
void int_void(struct interrupt_frame *frame);

#endif
