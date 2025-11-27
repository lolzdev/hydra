#include <proc.h>
#include <elf.h>
#include <stdint.h>
#include <stddef.h>
#include <mm/buddy.h>
#include <riscv64/vm/vm.h>
#include <riscv64/isa.h>
#include <drivers/uart.h>
#include <mem.h>
#include <sched.h>

#define ALIGNUP(data, align) (((data) + (align) - 1) & ~((align) - 1))

extern uint16_t LAST_PID;

void process_create(uint64_t address)
{
	struct process_node *node = mm_alloc(sizeof(struct process_node));
	/* Assign process ID. */
	node->proc.id = LAST_PID + 1;

	/* Create a new page table for the process. */
	node->proc.page_table = vm_create_page_table();

	/* Allocate a stack for the kernel when an interrupt occurs. */
	void *kernel_stack = mm_alloc_pages(1);
	node->proc.frame.kernel_sp = (uint64_t)kernel_stack + VM_PAGE_SIZE;

	/* Prepare the value that the satp register assumes when jumping to this process. */
	size_t table = vm_get_phys(kernel_pt.page_table, (size_t)node->proc.page_table);
	node->proc.satp = RISCV_MAKE_SATP(table, RISCV_SATP_SV48);

	/* Map the kernel code in the higher half of the address space inside the process page table. */
	for (size_t i=0x80000000; i < 0xC0000000; i+=0x1000) {
		vm_mmap(node->proc.page_table, 0xffffffff00000000 + i, i, VM_PTE_VALID | VM_PTE_READ | VM_PTE_WRITE | VM_PTE_EXEC);
	}

	/*
	 * This part of the code parses the ELF file, allocating space in memory
	 * for the process code and data, mapping all of it in the lower half of
	 * the address space. Refer to https://en.wikipedia.org/wiki/Executable_and_Linkable_Format
	 * for the ELF specification and core/include/elf.h
	 */
	elf_header_t *header = (elf_header_t *) address;
	if (!ELF_CHECK_MAGIC(&(header->magic)))
		uart_puts("Invalid ELF executable!\n");
	elf_program_header_t *p_headers = (elf_program_header_t *)((char *)header + header->p_header);

	node->proc.frame.pc = header->entry;
	for (uint32_t i = 0; i < header->ph_num; i++) {
		elf_program_header_t *ph = &p_headers[i];

		if (ph->type == ELF_PT_LOAD) {
			uint64_t vaddr = ph->vaddr;
			uint64_t mem_size = ph->mem_size;
			uint64_t file_size = ph->file_size;
			uint64_t offset = ph->offset;

			uint64_t vaddr_page = vaddr & ~0xFFF; 
			uint64_t offset_in_page = vaddr & 0xFFF;

			uint64_t total_size = mem_size + offset_in_page;
			size_t pages = (total_size + VM_PAGE_SIZE - 1) / VM_PAGE_SIZE;

			void *kernel_backing = mm_alloc_pages(pages);

			void *src = (void *)((char *)header + offset);
			void *dst = (char *)kernel_backing + offset_in_page;

			memset(kernel_backing, 0, pages * VM_PAGE_SIZE);
			memcpy(dst, src, file_size);

			uint64_t flags = VM_PTE_USER | VM_PTE_VALID;
			if (ph->flags & ELF_PF_W) flags |= VM_PTE_WRITE;
			if (ph->flags & ELF_PF_R) flags |= VM_PTE_READ;
			if (ph->flags & ELF_PF_X) flags |= VM_PTE_EXEC;

			for (size_t p = 0; p < pages; p++) {
				uint64_t current_vaddr = vaddr_page + (p * VM_PAGE_SIZE);

				uint64_t current_phys = vm_get_phys(kernel_pt.page_table, (uint64_t)kernel_backing) + (p * VM_PAGE_SIZE);

				vm_mmap(node->proc.page_table, current_vaddr, current_phys, flags); 
			}
		}
	}

	/* Create a stack for the process. */
	size_t stack_pages = 4;
	void *stack_backing = mm_alloc_pages(stack_pages);
	uint64_t user_stack_top = 0x40000000;
	uint64_t user_stack_base = user_stack_top - (stack_pages * VM_PAGE_SIZE);
	for (size_t p = 0; p < stack_pages; p++) {
		uint64_t va = user_stack_base + (p * VM_PAGE_SIZE);
		uint64_t pa = vm_get_phys(kernel_pt.page_table, (uint64_t)stack_backing) + (p * VM_PAGE_SIZE);
		vm_mmap(node->proc.page_table, va, pa, VM_PTE_VALID | VM_PTE_READ | VM_PTE_WRITE | VM_PTE_USER);
	}
	node->proc.frame.sp = user_stack_top;

	/* Set the status flags. */
	node->proc.frame.sstatus = (1 << 5) | (1 << 18);

	/* Add the process to the linked list of running processes. */
	sched_add(node);
}

void process_kill(uint16_t pid)
{
	/* TODO: free process resources when killing it. */
	sched_kill(pid);
}

struct process_node *process_fork(struct process *proc)
{
	struct process_node *node = mm_alloc(sizeof(struct process_node));
	memcpy(&node->proc, proc, sizeof(struct frame));

	/* Assign process ID. */
	node->proc.id = LAST_PID + 1;

	/* Create a new page table for the process. */
	node->proc.page_table = vm_create_page_table();

	/* Allocate a stack for the kernel when an interrupt occurs. */
	void *kernel_stack = mm_alloc_pages(1);
	node->proc.frame.kernel_sp = (uint64_t)kernel_stack + VM_PAGE_SIZE;

	/* Prepare the value that the satp register assumes when jumping to this process. */
	size_t table = vm_get_phys(kernel_pt.page_table, (size_t)node->proc.page_table);
	node->proc.satp = RISCV_MAKE_SATP(table, RISCV_SATP_SV48);

	uint64_t *parent_pt = proc->page_table;
	vm_copy(node->proc.page_table, parent_pt);

	node->proc.frame.a0 = 0;
	node->proc.frame.pc += 4;

	/* Create a stack for the process. */
	size_t stack_pages = 4;
	void *stack_backing = mm_alloc_pages(stack_pages);
	uint64_t user_stack_top = 0x40000000;
	uint64_t user_stack_base = user_stack_top - (stack_pages * VM_PAGE_SIZE);
	uint64_t proc_stack_base = vm_get_phys(proc->page_table, user_stack_base) | HH_MASK;
	for (size_t p = 0; p < stack_pages; p++) {
		uint64_t va = user_stack_base + (p * VM_PAGE_SIZE);
		uint64_t pa = vm_get_phys(kernel_pt.page_table, (uint64_t)stack_backing) + (p * VM_PAGE_SIZE);
		vm_mmap(node->proc.page_table, va, pa, VM_PTE_VALID | VM_PTE_READ | VM_PTE_WRITE | VM_PTE_USER);
		memcpy((void *)((uint64_t)stack_backing + (p * VM_PAGE_SIZE)), (void *)(proc_stack_base + (p * VM_PAGE_SIZE)), VM_PAGE_SIZE);
	}


	/* Add the process to the linked list of running processes. */
	sched_add(node);

	return node;
}
