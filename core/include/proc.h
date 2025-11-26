#ifndef PROC_H
#define PROC_H

#include <stdint.h>
#include <stddef.h>

struct frame {
	uint64_t ra;
	uint64_t sp;
	uint64_t gp;
	uint64_t tp;

	uint64_t t0;
	uint64_t t1;
	uint64_t t2;
	uint64_t t3;
	uint64_t t4;
	uint64_t t5;
	uint64_t t6;

	uint64_t s0;
	uint64_t s1;
	uint64_t s2;
	uint64_t s3;
	uint64_t s4;
	uint64_t s5;
	uint64_t s6;
	uint64_t s7;
	uint64_t s8;
	uint64_t s9;
	uint64_t s10;
	uint64_t s11;

	uint64_t a0;
	uint64_t a1;
	uint64_t a2;
	uint64_t a3;
	uint64_t a4;
	uint64_t a5;
	uint64_t a6;
	uint64_t a7;

	uint64_t pc;
	uint64_t kernel_sp;
	uint64_t sstatus;
	uint64_t hartid;
};

struct process {
	struct frame frame;
	uint64_t *page_table;
	uint64_t satp;
	uint16_t id;
	uint16_t uid;
} __attribute__((packed));

struct process_node {
	struct process proc;
	struct process_node *next;
} __attribute__((packed));

/* Create a new process given the ELF file address */
void process_create(uint64_t address);
void process_kill(uint16_t id);
uint16_t process_fork(struct process *proc);

#endif
