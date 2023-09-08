#include <arch/x86_64-elf/mm.h>

uint64_t PAGE_COUNT;
uint8_t *PAGE_LIST;

void k_add_heap_chunk(uint64_t base, uint64_t size)
{
	uint64_t index = base / PAGE_SIZE;
	for (uint64_t i=index; i < ((base + size) / PAGE_SIZE); i++)
		PAGE_LIST[i] = PAGE_FREE;
}

uint64_t init_mm(struct limine_memmap_response *memmap)
{
	uint64_t base = 0;
	uint64_t last_size = 0;
	uint64_t memory_size = 0;
	if (memmap->entry_count > 0) {
		for (uint64_t i=0; i < memmap->entry_count; i++) {
			struct limine_memmap_entry *entry = memmap->entries[i];
			memory_size += entry->length;
			if (entry->type == LIMINE_MEMMAP_USABLE)
				if (entry->length > last_size) base = entry->base;
		}

	}

	PAGE_COUNT = memory_size / PAGE_SIZE;
	uint16_t pages = (PAGE_COUNT / PAGE_SIZE);
	if (pages < 1) pages = 1;
	PAGE_LIST = (uint8_t *)base;

	memset(PAGE_LIST, PAGE_USED, PAGE_COUNT);

	if (memmap->entry_count > 0) {
		for (uint64_t i=0; i < memmap->entry_count; i++) {
			struct limine_memmap_entry *entry = memmap->entries[i];
			if (entry->type == LIMINE_MEMMAP_USABLE) {
				k_add_heap_chunk(entry->base, entry->length);
			}
		}
	}

	PAGE_LIST[(uint64_t)PAGE_LIST / PAGE_SIZE] = PAGE_USED;

	return memory_size;
}

void *k_page_alloc(uint16_t pages)
{
	int found;

	for (uint16_t i=0; i < PAGE_COUNT; i++) {
		if (PAGE_LIST[i] == PAGE_USED || PAGE_LIST[i] == PAGE_LAST) continue;

		if (PAGE_LIST[i] == PAGE_FREE) {
			found = 1;
			for (uint16_t j=0; j < pages; j++) {
				if (PAGE_LIST[i+j] == PAGE_USED || PAGE_LIST[i+j] == PAGE_LAST)
					found = 0;
			}
		}

		if (found) {
			for (uint64_t j=0; j < pages; j++)
				PAGE_LIST[i+j] = PAGE_USED;
			PAGE_LIST[i + (pages - 1)] = PAGE_LAST;	

			return (void *) ((uint64_t)i * PAGE_SIZE);
		}
	}

	return (void *) 0;
}

void *k_page_zalloc(uint16_t pages)
{
	void *ptr = k_page_alloc(pages);
	memset((uint8_t *)ptr, 0, pages * PAGE_SIZE);
	return ptr;
}

void k_page_free(void *addr)
{
	uint64_t index = (uint64_t) addr / PAGE_SIZE;
	
	while (index < NPAGES) {
		if (PAGE_LIST[index] != PAGE_FREE)
			PAGE_LIST[index] = PAGE_FREE;

		if (PAGE_LIST[index] == PAGE_LAST)
			return;

		index++;
	}
}
