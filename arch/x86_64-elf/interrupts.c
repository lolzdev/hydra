#include <arch/x86_64-elf/interrupts.h>

__attribute__((interrupt))
void int_division_by_zero(struct interrupt_frame *frame)
{
	printf("panic: division by 0 at: 0x%x\n", frame->ip);
}


__attribute__((interrupt))
void int_breakpoint(struct interrupt_frame *frame)
{
	printf("panic: breakpoint at: 0x%x\n", frame->ip);
}

__attribute__((interrupt))
void int_page_fault(struct interrupt_frame *frame)
{
	printf("panic: page fault at: 0x%x\n", frame->ip);
}

__attribute__((interrupt))
void int_gpf(struct interrupt_frame *frame)
{
	printf("panic: general protection fault at: 0x%x\n", frame->ip);
}

__attribute__((interrupt))
void int_systimer(struct interrupt_frame *frame)
{

}

__attribute__((interrupt))
void int_keyboard(struct interrupt_frame *frame)
{

}

__attribute__((interrupt))
void int_void(struct interrupt_frame *frame)
{

}
