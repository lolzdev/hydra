#include <riscv64/vm/vm.h>
#include <mm/buddy.h>
#include <drivers/uart.h>
#include <riscv64/isa.h>
#include <mem.h>

struct kernel_pt kernel_pt;

static void zero(void *ptr, size_t size)
{
	for (size_t i = 0; i < size; i++) {
		((uint8_t *)ptr)[i] = 0x0;
	}
}

void vm_init(void)
{
	kernel_pt.page_table = vm_create_page_table();

	/*
	 * Map the physical addresses from 0x80000000
	 * to 0xC0000000 in the respective higher half.
	 */
	for (size_t i=0x80000000; i < 0xC0000000; i+=0x1000) {
		vm_mmap(kernel_pt.page_table, 0xffffffff00000000 + i, i, VM_PTE_VALID | VM_PTE_READ | VM_PTE_WRITE | VM_PTE_EXEC);
	}

	/* Identity map the UART MMIO register */
	vm_mmap(kernel_pt.page_table, 0x10000000, 0x10000000, VM_PTE_VALID | VM_PTE_READ | VM_PTE_WRITE | VM_PTE_EXEC);

	uart_printf("Virtual memory initialized with page table 0x%x.\n", kernel_pt.page_table);
}

void vm_load_page_table(uint64_t *pt)
{
	/* The satp CSR requires the page table address to be physical */
	size_t table = vm_get_phys(kernel_pt.page_table, (size_t)pt);
	riscv_set_satp(RISCV_MAKE_SATP(table, RISCV_SATP_SV48));
}

uint64_t *vm_create_page_table(void)
{
	uint64_t *page_table = mm_alloc(sizeof(uint64_t) * 512);
	zero((void *)page_table, sizeof(uint64_t) * 512);
	return page_table;
}

void vm_mmap(uint64_t *table, size_t virtual, size_t physical, uint64_t flags)
{
	/* Split the virtual address in the 3 needed indices. */
	uint32_t vpn0 = (virtual >> 12) & 0x1ff;
	uint32_t vpn1 = (virtual >> 21) & 0x1ff;
	uint32_t vpn2 = (virtual >> 30) & 0x1ff;
	uint32_t vpn3 = (virtual >> 39) & 0x1ff;

	/* If a table level doesn't exist yet, allocate it. */
	if (!table[vpn3]) {
		uint64_t address = (uint64_t)mm_alloc(sizeof(uint64_t) * 512);
		table[vpn3] = (((address - HH_MASK) >> 12) << 10) | VM_PTE_VALID;
		zero((void *)address, sizeof(uint64_t) * 512);
	}
	uint64_t *layer2 = (uint64_t *) ((((table[vpn3] >> 10) << 12)) | HH_MASK);
	if (!layer2[vpn2]) {
		uint64_t address = (uint64_t)mm_alloc(sizeof(uint64_t) * 512);
		layer2[vpn2] = (((address - HH_MASK) >> 12) << 10) | VM_PTE_VALID;
		zero((void *)address, sizeof(uint64_t) * 512);
	}
	uint64_t *layer1 = (uint64_t *) (((layer2[vpn2] >> 10) << 12) | HH_MASK);
	if (!layer1[vpn1]) {
		uint64_t address = (uint64_t)mm_alloc(sizeof(uint64_t) * 512);
		layer1[vpn1] = (((address - HH_MASK) >> 12) << 10) | VM_PTE_VALID;
		zero((void *)address, sizeof(uint64_t) * 512);
	}
	uint64_t *layer0 = (uint64_t *) (((layer1[vpn1] >> 10) << 12) | HH_MASK);

	/* Create the entry containing the physical address mapping */
	uint64_t pte = ((physical >> 12) << 10) | (flags & 0xff);

	layer0[vpn0] = pte;
}

void vm_copy(uint64_t *dest, uint64_t *source)
{
	for (size_t vpn3 = 0; vpn3 < 512; vpn3++) {
		/* If a table level is present, copy it. */
		if (source[vpn3]) {
			uint64_t address = (uint64_t)mm_alloc(sizeof(uint64_t) * 512);
			dest[vpn3] = (((address - HH_MASK) >> 12) << 10) | VM_PTE_VALID;
			zero((void *)address, sizeof(uint64_t) * 512);

			uint64_t *layer2 = (uint64_t *) ((((source[vpn3] >> 10) << 12)) | HH_MASK);
			uint64_t *layer2_dest = (uint64_t *) ((((dest[vpn3] >> 10) << 12)) | HH_MASK);
			for (size_t vpn2 = 0; vpn2 < 512; vpn2++) {
				if (layer2[vpn2]) {
					uint64_t address = (uint64_t)mm_alloc(sizeof(uint64_t) * 512);
					layer2_dest[vpn2] = (((address - HH_MASK) >> 12) << 10) | VM_PTE_VALID;
					zero((void *)address, sizeof(uint64_t) * 512);
					
					uint64_t *layer1 = (uint64_t *) (((layer2[vpn2] >> 10) << 12) | HH_MASK);
					uint64_t *layer1_dest = (uint64_t *) (((layer2_dest[vpn2] >> 10) << 12) | HH_MASK);
					for (size_t vpn1 = 0; vpn1 < 512; vpn1++) {
						if (layer1[vpn1]) {
							uint64_t address = (uint64_t)mm_alloc(sizeof(uint64_t) * 512);
							layer1_dest[vpn1] = (((address - HH_MASK) >> 12) << 10) | VM_PTE_VALID;
							zero((void *)address, sizeof(uint64_t) * 512);

							uint64_t *layer0 = (uint64_t *) (((layer1[vpn1] >> 10) << 12) | HH_MASK);
							uint64_t *layer0_dest = (uint64_t *) (((layer1_dest[vpn1] >> 10) << 12) | HH_MASK);
							for (size_t vpn0 = 0; vpn0 < 512; vpn0++) {
								uint64_t pte = layer0[vpn0];
								if (pte & VM_PTE_WRITE && vpn3 < 256) {
									uint64_t kernel_address = ((pte >> 10) << 12);
									kernel_address |= HH_MASK;
									uint64_t allocated_page = (uint64_t)mm_alloc_pages(1);
									layer0_dest[vpn0] = (((vm_get_phys(kernel_pt.page_table, allocated_page)) >> 12) << 10) | (pte & 0xff);
									memcpy((void *)allocated_page, (void *)kernel_address, VM_PAGE_SIZE);
								} else {
									layer0_dest[vpn0] = pte;
								}
							}
						}
					}
				}
			}
		}
	}
}

uint64_t vm_get_phys(uint64_t *table, size_t virtual)
{
	uint32_t vpn0 = (virtual >> 12) & 0x1ff;
	uint32_t vpn1 = (virtual >> 21) & 0x1ff;
	uint32_t vpn2 = (virtual >> 30) & 0x1ff;
	uint32_t vpn3 = (virtual >> 39) & 0x1ff;

	uint64_t *layer2 = (uint64_t *) ((((table[vpn3] >> 10) << 12)) | HH_MASK);
	uint64_t *layer1 = (uint64_t *) (((layer2[vpn2] >> 10) << 12) | HH_MASK);
	uint64_t *layer0 = (uint64_t *) (((layer1[vpn1] >> 10) << 12) | HH_MASK);

	return (layer0[vpn0] >> 10) << 12;
}

void vm_reload(void)
{
	__asm__ volatile("sfence.vma");
}
