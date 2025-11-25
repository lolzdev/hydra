#include <drivers/uart.h>
#include <proc.h>
#include <riscv64/vm/vm.h>

extern struct process_node *current_proc;

void syscall_handle(struct frame *frame)
{
	if (frame->a0 == 0xfe) {
		current_proc->proc.frame.a0 = process_fork(&current_proc->proc);
	} else if (frame->a0 == 0x1) {
		/* FIXME: overlapping pages */
		uint64_t string = vm_get_phys(current_proc->proc.page_table, frame->a1);
		string = string | HH_MASK + (frame->a1 & VM_PAGE_SIZE - 1);
		uart_printf("%s", string);
	}
}
