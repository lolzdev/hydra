#ifndef SCHED_H
#define SCHED_H

#include <stdint.h>
#include <proc.h>

void sched_init(uint16_t cpu_count);
void sched_add(struct process_node *proc);
void sched_kill(uint16_t pid);
void sched_tick(void);

#endif
