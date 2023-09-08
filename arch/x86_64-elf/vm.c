#include <arch/x86_64-elf/vm.h>

static uint64_t *pml4 = NULL;
extern void enable_page_table(uint64_t *pml4);

void init_paging(struct limine_memmap_response *memmap)
{
	pml4 = k_page_zalloc(1); 

	uintptr_t cr3;
	__asm__ volatile ("mov %%cr3, %0" : "=r"(cr3));

	for (uint64_t i=256; i < 512; i++) {
		pml4[i] = ((uint64_t *)cr3)[i];
	}

	for (uint64_t i=0; i < memmap->entry_count; i++) {
		struct limine_memmap_entry *entry = memmap->entries[i];	
		if (entry->type == LIMINE_MEMMAP_USABLE) {
			for (uint64_t i=entry->base; i < entry->base + entry->length; i+=0x1000) {
				kvmalloc(&pml4[0], (void *)i, (void *)i, 0x3);
			}
		}
	}

	enable_page_table(&pml4[0]);
}

void kvmmap(uint64_t *pml4, void *phys, void *virt, uint64_t flags)
{
	uint16_t pml4_index = ((uint64_t)virt >> 39) & 0x1ff;
	uint16_t pml3_index = ((uint64_t)virt >> 30) & 0x1ff;
	uint16_t pml2_index = ((uint64_t)virt >> 21) & 0x1ff;
	uint16_t pml1_index = ((uint64_t)virt >> 12) & 0x1ff;

	uint64_t *pml3 = (uint64_t *)(pml4[pml4_index] & ~0xf800000000000fff);
	uint64_t *pml2 = (uint64_t *)(pml3[pml3_index] & ~0xf800000000000fff);
	uint64_t *pml1 = (uint64_t *)(pml2[pml2_index] & ~0xf800000000000fff);

	pml1[pml1_index] = ((uint64_t)phys) | flags;
}

void kvmalloc(uint64_t *pml4, void *phys, void *virt, uint64_t flags)
{
	uint16_t pml4_index = ((uint64_t)virt >> 39) & 0x1ff;
	uint16_t pml3_index = ((uint64_t)virt >> 30) & 0x1ff;
	uint16_t pml2_index = ((uint64_t)virt >> 21) & 0x1ff;
	uint16_t pml1_index = ((uint64_t)virt >> 12) & 0x1ff;

	if (!pml4[pml4_index])
		pml4[pml4_index] = (uint64_t)k_page_zalloc(1) | 0x7;
	uint64_t *pml3 = (uint64_t *)(pml4[pml4_index] & ~0xf800000000000fff);

	if (!pml3[pml3_index])
		pml3[pml3_index] = (uint64_t)k_page_zalloc(1) | 0x7;
	uint64_t *pml2 = (uint64_t *)(pml3[pml3_index] & ~0xf800000000000fff);

	if (!pml2[pml2_index])
		pml2[pml2_index] = (uint64_t)k_page_zalloc(1) | 0x7;
	uint64_t *pml1 = (uint64_t *)(pml2[pml2_index] & ~0xf800000000000fff);
	
	pml1[pml1_index] = ((uint64_t)phys) | flags;
}
