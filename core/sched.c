#include <proc.h>

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
}
