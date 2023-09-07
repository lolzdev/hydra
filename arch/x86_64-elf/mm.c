#include <arch/x86_64-elf/mm.h>

uint8_t page_list[NPAGES];

void *k_page_alloc(uint16_t pages)
{
	int found;

	for (uint16_t i=0; i < NPAGES; i++) {
		if (page_list[i] == PAGE_USED || page_list[i] == PAGE_LAST) continue;

		if (page_list[i] == PAGE_FREE) {
			found = 1;
			for (uint16_t j=0; j < pages; j++) {
				if (page_list[i+j] == PAGE_USED || page_list[i+j] == PAGE_LAST)
					found = 0;
			}
		}

		if (found) {
			for (uint64_t j=0; j < pages; j++)
				page_list[i+j] = PAGE_USED;
			page_list[i + (pages - 1)] = PAGE_LAST;	

			return (void *) ((uint64_t) endkernel + (i * PAGE_SIZE));
		}
	}

	return (void *) 0;
}

void *k_page_zalloc(uint16_t pages)
{
	void *ptr = k_page_alloc(pages);
	//memset((uint8_t *)ptr, 0, pages * PAGE_SIZE);
	return ptr;
}

void k_page_free(void *addr)
{
	uint64_t index = ((uint64_t) addr - (uint64_t)endkernel) / PAGE_SIZE;
	
	while (index < NPAGES) {
		if (page_list[index] != PAGE_FREE)
			page_list[index] = PAGE_FREE;

		if (page_list[index] == PAGE_LAST)
			return;

		index++;
	}
}
