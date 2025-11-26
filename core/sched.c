#include <sched.h>
#include <riscv64/isa.h>
#include <drivers/uart.h>
#include <riscv64/vm/vm.h>
#include <mm/buddy.h>

extern void user_jump(struct frame *proc_frame, uint64_t *page_table);
struct process_node **process_list;
struct process_node **current_proc;
uint16_t *process_counts;
uint16_t LAST_PID = 0;
static uint16_t CPU_COUNT = 0;
static uint64_t a = 0;
static struct spinlock lock;

void sched_init(uint16_t cpu_count)
{
	lock.locked = 0;
	CPU_COUNT = cpu_count;
	process_counts = mm_alloc(sizeof(uint16_t) * cpu_count);
	process_list = mm_alloc(sizeof(struct process_node *) * cpu_count);
	current_proc = mm_alloc(sizeof(struct process_node *) * cpu_count);
	for (int i=0; i < cpu_count; i++) {
		process_counts[i] = 0;
		current_proc[i] = NULL;
		process_list[i] = NULL;
	}
}

void sched_add(struct process_node *proc)
{
	spinlock_aquire(&lock);
	uint16_t best_hart = 0;
	for (uint16_t i=0; i < CPU_COUNT; i++) {
		if (process_counts[i] < process_counts[best_hart]) {
			best_hart = i;
		}
	}

	process_counts[best_hart] += 1;

	proc->next = process_list[best_hart];
	process_list[best_hart] = proc;
	proc->proc.frame.hartid = best_hart;
	LAST_PID += 1;

	if (!current_proc[best_hart]) {
		current_proc[best_hart] = process_list[best_hart];
	}
	spinlock_release(&lock);
}

void sched_kill(uint16_t pid)
{
	spinlock_aquire(&lock);
	for (uint16_t i=0; i < CPU_COUNT; i++) {
		struct process_node *node = process_list[i];
		struct process_node *prev = NULL;
		while (node != NULL) {
			if (node->proc.id == pid) {
				if (prev) {
					prev->next = node->next;
				} else {
					process_list[i] = node->next;
				}

				mm_free(node->proc.page_table, sizeof(uint64_t) * 512);
				mm_free(node, sizeof(struct process_node));
				goto found;
			}
			
			prev = node;
			node = node->next;
		}
	}
found:
	spinlock_release(&lock);
}

void sched_tick(void)
{
	spinlock_aquire(&lock);
	uint64_t tp = 0;
	__asm__ volatile("mv %0, tp" : "=r"(tp) :);

	if (!current_proc[tp]) {
		spinlock_release(&lock);
		return;
	}

	if (current_proc[tp]->next) {
		current_proc[tp] = current_proc[tp]->next;
	} else {
		current_proc[tp] = process_list[tp];
	}
	//uart_printf("tp: %d\n", tp);

	spinlock_release(&lock);

	user_jump(&current_proc[tp]->proc.frame, (uint64_t *)current_proc[tp]->proc.satp);
}
