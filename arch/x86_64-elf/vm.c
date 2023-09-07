#include <arch/x86_64-elf/vm.h>

uint64_t pml4[1] __attribute__((aligned(0x1000)));
static uint64_t pdpt[1] __attribute__((aligned(0x1000))); 
static uint64_t page_directory[512] __attribute__((aligned(0x1000)));
static uint64_t page_table[512] __attribute__((aligned(0x1000)));

extern void enable_page_table(uint64_t *pml4);

void init_paging()
{
	pml4[0] = ((uint64_t)&pdpt[0]) | 0b11;
	pdpt[0] = ((uint64_t)&page_directory[0]) | 0b11;
	page_directory[0] = ((uint64_t)&page_table[0]) | 0b11;

	for (int i=0; i<0x1000000; i+=0x1000) {
		kvmmap(&pml4[0], (void *)i, (void *)i, 3);
	}

	enable_page_table(&pml4[0]);
}

void kvmmap(uint64_t *pml4, void *phys, void *virt, uint64_t flags)
{
	uint16_t pml4_index = ((uint64_t)virt >> 39) & 0x1ff;
	uint16_t pdpt_index = ((uint64_t)virt >> 30) & 0x1ff;
	uint16_t page_directory_index = ((uint64_t)virt >> 21) & 0x1ff;
	uint16_t page_table_index = ((uint64_t)virt >> 12) & 0x1ff;

	uint64_t *pd_pointer = (uint64_t *)(pml4[pml4_index] & ~0xfff);
	uint64_t *pd = (uint64_t *)(pd_pointer[pdpt_index] & ~0xfff);
	uint64_t *pt = (uint64_t *)(pd[page_directory_index] & ~0xfff);

	pt[page_table_index] = ((uint64_t)phys) | flags;
	
}
