#include <proc.h>
#include <elf.h>
#include <stdint.h>
#include <stddef.h>
#include <mm/buddy.h>
#include <riscv64/vm/vm.h>
#include <drivers/uart.h>
#include <mem.h>

#define ALIGNUP(data, align) (((data) + (align) - 1) & ~((align) - 1))



struct process_node *process_list;

void process_create(uint64_t address, size_t size)
{
	struct process_node *node = mm_alloc(sizeof(struct process_node));
	node->proc.page_table = vm_create_page_table();

	elf_header_t *header = (elf_header_t *) address;

	if (!ELF_CHECK_MAGIC(&(header->magic)))
		uart_puts("Invalid ELF executable!\n");

	elf_program_header_t *p_headers = (elf_program_header_t *)((char *)header + header->p_header);

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

				uint64_t current_phys = vm_get_phys(kernel_pt, (uint64_t)kernel_backing) + (p * VM_PAGE_SIZE);

				vm_mmap(node->proc.page_table, current_vaddr, current_phys, flags); 
			}
		}
	}

	node->next = process_list;
	process_list = node;
}

void process_kill(uint16_t id)
{
	struct process_node *node = process_list;
	struct process_node *prev = NULL;
	while (node != NULL) {
		if (node->proc.id == id) {
			if (prev) {
				prev->next = node->next;
			} else {
				process_list = node->next;
			}

			mm_free(node->proc.page_table, sizeof(uint64_t) * 512);
			mm_free(node, sizeof(struct process_node));
			break;
		}
		
		prev = node;
		node = node->next;
	}
}
