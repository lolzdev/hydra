#include <sched.h>
#include <proc.h>
#include <riscv64/isa.h>
#include <drivers/uart.h>
#include <riscv64/vm/vm.h>

extern void user_jump(struct frame *proc_frame, uint64_t *page_table);
extern struct process_node *process_list;
struct process_node *current_proc;

void sched_init(void)
{
	current_proc = process_list;
}

void sched_tick(void)
{
	if (current_proc->next) {
		current_proc = current_proc->next;
	} else {
		current_proc = process_list;
	}

	user_jump(&current_proc->proc.frame, (uint64_t *)RISCV_MAKE_SATP((uint64_t)current_proc->proc.page_table - HH_MASK, RISCV_SATP_SV48));
}
