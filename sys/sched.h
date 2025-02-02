	/*-
	 * Copyright 2025 Lorenzo Torres
 	 *
	 * Redistribution and use in source and binary forms, with or without
	 * modification, are permitted provided that the following conditions are met:
	 * 1. Redistributions of source code must retain the above copyright
	 *    notice, this list of conditions and the following disclaimer.
	 * 2. Redistributions in binary form must reproduce the above copyright
	 *    notice, this list of conditions and the following disclaimer in the
	 *    documentation and/or other materials provided with the distribution.
	 * 3. Neither the name of the copyright holder nor
	 *    the names of its contributors may be used to endorse or promote products
	 *    derived from this software without specific prior written permission.
	 *
	 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS “AS IS” AND ANY
	 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
	 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
	 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY
	 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
	 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
	 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
	 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
	 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
	 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
	 */

#ifndef SCHED_H
#define SCHED_H

#include <vm/vm.h>
#include <stdint.h>
#include <stddef.h>
#include <elf.h>
#include <limine.h>
#include <x86_64/trap.h>

#define PROC_STATE_RUNNING 0x1
#define PROC_STATE_HALTED 0x2
#define PROC_STATE_STOPPED 0x3

struct proc_registers {
	uint64_t rax;
	uint64_t rbx;
	uint64_t rcx;
	uint64_t rdx;
	uint64_t rsi;
	uint64_t rdi;
	uint64_t rsp;
	uint64_t rbp;
	uint64_t r8;
	uint64_t r9;
	uint64_t r10;
	uint64_t r11;
	uint64_t r12;
	uint64_t r13;
	uint64_t r14;
	uint64_t r15;
};

typedef struct proc {
	struct proc_registers regs;
	pml4_t page_table;
	uint8_t state;
	uint64_t rip;
	uint16_t id;
} proc_t;

typedef struct proc_list {
	proc_t *proc;
	struct proc_list *next;
} __attribute__((packed)) proc_list_t;

extern void sched_save_regs(struct proc_registers *registers);
extern void sched_load_regs(struct proc_registers *registers);
extern void sched_switch(uint64_t address, uint64_t pml4, uint64_t stack);
void int_tmr(struct interrupt_frame *frame);
proc_list_t *sched_fire(proc_t *proc);
proc_t *sched_proc_load(void *address, size_t size);
void sched_init(void);
void sched_start(struct limine_file **modules, size_t count);

#endif
