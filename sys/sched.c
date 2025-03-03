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

#include <sched.h>
#include <mm/mm.h>
#include <lock.h>
#include <x86_64/inst.h>
#include <log/fb.h>
#include <string.h>

proc_t *KERNEL_PROC;
proc_list_t *CURRENT_PROC;

static proc_list_t *proc_queue = NULL;
static proc_list_t *proc_queue_tail = NULL;

spinlock_t proc_queue_lock = {
	.lock = 0,
	.content = NULL,
};

void int_tmr(struct interrupt_frame *frame)
{
    kprintf("timer\n");
	if (frame->cs == 0x1b) {
		sched_save_regs(&(CURRENT_PROC->proc->regs));
		CURRENT_PROC->proc->regs.rsp = frame->rsp;
		vm_reload();
		lock_acquire(&proc_queue_lock);
		if (proc_queue != NULL) {
			CURRENT_PROC->proc->rip = frame->rip;
			proc_queue_tail->next = CURRENT_PROC;
			proc_queue_tail = CURRENT_PROC;
			if (proc_queue->next != NULL) proc_queue = proc_queue->next;
			CURRENT_PROC = proc_queue;
		}
		lock_release(&proc_queue_lock);
		__outb(0x20, 0x20);
		sched_load_regs(&(CURRENT_PROC->proc->regs));
		sched_switch(CURRENT_PROC->proc->rip, (size_t)(CURRENT_PROC->proc->page_table) & ~0xfff, CURRENT_PROC->proc->regs.rsp);
	}

	__outb(0x20, 0x20);
}

void sched_init(void)
{
	kprintf("Initializing scheduler.\n");
	KERNEL_PROC = mm_alloc_pages(1);
	KERNEL_PROC->page_table = vm_get_ktable();
	CURRENT_PROC = NULL;
}

void sched_start(struct limine_file **modules, size_t count)
{
	kprintf("Starting scheduler.\n");	

	for (size_t i=0; i < count; i++) {
		struct limine_file *module = modules[i];

		if (strcmp(module->path, "/boot/loop") == 0) {
			kprintf("Loading module %s.\n", module->path);
			proc_t *proc = sched_proc_load(module->address, module->size);
			proc_list_t *proc_entry = sched_fire(proc);
			lock_acquire(&proc_queue_lock);
			CURRENT_PROC = proc_entry;
			vm_mmap(proc->page_table, (size_t)CURRENT_PROC, (size_t)vm_get_kphys(CURRENT_PROC) & ~0xfff, PAGE_PRESENT | PAGE_WRITABLE | PAGE_USER);
			vm_mmap(proc->page_table, (size_t)CURRENT_PROC->proc, (size_t)vm_get_kphys(CURRENT_PROC->proc) & ~0xfff, PAGE_PRESENT | PAGE_WRITABLE | PAGE_USER);
			lock_release(&proc_queue_lock);
			sched_switch(CURRENT_PROC->proc->rip, (size_t)vm_get_kphys(CURRENT_PROC->proc->page_table) & ~0xfff, proc->regs.rsp);
		}
	}
}

proc_t *sched_proc_load(void *address, size_t size)
{
	proc_t *proc = mm_alloc_pages(1);
	proc->page_table = vm_init_user();

	elf_header_t *header = mm_alloc(size);
	vm_kmmap((void *)header, (size_t)address & 0x00000000ffffffff, PAGE_PRESENT | PAGE_WRITABLE | PAGE_USER);
	vm_reload();

	if (!ELF_CHECK_MAGIC(&(header->magic)))
		kprintf("Invalid ELF executable!\n");

	elf_program_header_t *p_headers = (elf_program_header_t *) ((size_t)header + (size_t)(header->p_header));
	for (uint32_t i=0; i < header->ph_num; i++) {
		elf_program_header_t p_header = p_headers[i];
		if (p_header.type == ELF_PT_LOAD) {
			void *seg = (void *)((uint64_t) header + p_header.offset);
			void *vaddr = (void *) p_header.vaddr;
			uint64_t seg_size = p_header.file_size;
			uint64_t mem_size = p_header.mem_size;
			size_t pages = mem_size / 0x1000;
			if (pages == 0) pages = 1;
			void *addr = mm_alloc_pages(pages);
			memcpy(addr + ((size_t)vaddr % 0x1000), seg, seg_size);
			uint64_t bss = mem_size - seg_size;
			if (bss > 0) {
				memset((void *)((uint64_t)(addr + ((size_t)vaddr % 0x1000)) + seg_size), 0x0, bss);
			}

			vm_mmap(proc->page_table, (size_t)vaddr & ~0xfff, (size_t)vm_get_kphys(addr) & ~0xfff, PAGE_PRESENT | PAGE_WRITABLE | PAGE_USER);
		}
	}


	vm_mmap(proc->page_table, (size_t)proc, (size_t)vm_get_kphys(proc) & ~0xfff, PAGE_PRESENT | PAGE_WRITABLE | PAGE_USER);

	kprintf("Loaded ELF in memory.\n");

	proc->rip = header->entry;
	return proc;
}

proc_list_t *sched_fire(proc_t *proc)
{
	proc->state = PROC_STATE_RUNNING;
	uint64_t user_stack = (uint64_t) mm_alloc_pages(1);
	vm_mmap(proc->page_table, (size_t)user_stack & ~0xfff, (size_t)vm_get_kphys(user_stack) & ~0xfff, PAGE_PRESENT | PAGE_WRITABLE | PAGE_USER);

	sched_save_regs(&(KERNEL_PROC->regs));
	proc->regs.rsp = (user_stack + 0x1000)-1;
	proc->regs.rbp = proc->regs.rsp;

	vm_mmap(proc->page_table, (size_t)KERNEL_PROC->regs.rsp & ~0xfff, (size_t)vm_get_kphys(KERNEL_PROC->regs.rsp) & ~0xfff, PAGE_PRESENT | PAGE_WRITABLE);
	vm_mmap(proc->page_table, (size_t)KERNEL_PROC->page_table & ~0xfff, (size_t)vm_get_kphys(KERNEL_PROC->page_table) & ~0xfff, PAGE_PRESENT | PAGE_WRITABLE );
	
	lock_acquire(&proc_queue_lock);
	proc_list_t *proc_entry = mm_alloc_pages(1);
	proc_entry->proc = proc;
	if (proc_queue == NULL) {
		proc_queue = proc_entry;
		proc_queue_tail = proc_entry;
	} else {
		proc_entry->next = proc_queue;
		proc_queue = proc_entry;
	}
	lock_release(&proc_queue_lock);

	vm_mmap(proc->page_table, (size_t)proc_entry, (size_t)vm_get_kphys(proc_entry) & ~0xfff, PAGE_PRESENT | PAGE_WRITABLE | PAGE_USER);

	return proc_entry;
}
