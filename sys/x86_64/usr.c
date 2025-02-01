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

#include <elf.h>
#include <string.h>
#include <x86_64/usr.h>
#include <x86_64/inst.h>
#include <log/fb.h>
#include <vm/vm.h>
#include <mm/mm.h>

static proc_t KERNEL_PROC;
static proc_list_t proc_list;

void usr_load_module(void *address, size_t size)
{
	/* Set the process list as empty */
	proc_list.next = NULL;
	proc_list.prev = NULL;
	proc_list.proc = NULL;

	elf_header_t *header = mm_alloc(size);
	vm_kmmap((void *)header, (size_t)address & 0x00000000ffffffff, PAGE_PRESENT | PAGE_WRITABLE | PAGE_USER);
	vm_reload();
	if (!ELF_CHECK_MAGIC(&(header->magic)))
		kprintf("Invalid ELF executable!\n");
	void (*entry)(void) = header->entry;

	KERNEL_PROC.page_table = vm_get_ktable();

	proc_t *proc = mm_alloc_pages(1);
	proc->page_table = vm_init_user();
	proc->state = PROC_STATE_RUNNING;
	if (proc_list.proc == NULL) {
		proc_list.proc = proc;
		proc_list.prev = &proc_list;
		proc_list.next = proc_list.prev;
	} else {
		proc_list_t *entry = mm_alloc_pages(1);
		entry->prev = proc_list.prev;
		entry->next = NULL;
		entry->proc = proc;
		proc_list.prev->next = entry;
		proc_list.prev = entry;
	}

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
			vm_mmap(proc->page_table, vaddr, (size_t)addr & 0x00000000ffffffff, PAGE_PRESENT | PAGE_WRITABLE | PAGE_USER);
			vm_set_pml4(proc->page_table);
			__wr8(0xcafebabe);
			memcpy(vaddr, seg, seg_size);
			__wr9(0xbabecafe);
			uint64_t bss = mem_size - seg_size;
			if (bss > 0) {
				memset((void *)((uint64_t)vaddr + seg_size), 0x0, bss);
			}
		}
	}

	kprintf("Loaded ELF in memory.\n");
	proc_snapshot(&KERNEL_PROC.regs);
	proc_snapshot(&(proc->regs));
	ctx_switch(entry);
}

void usr_init(struct limine_file **modules, size_t count)
{
	for (size_t i=0; i < count; i++) {
		struct limine_file *module = modules[i];

		kprintf("Loading module %s.\n", module->path);
		usr_load_module(module->address, module->size);
	}
}
