#include <riscv64/vm/vm.h>
#include <mm/buddy.h>
#include <drivers/uart.h>
#include <riscv64/isa.h>

uint64_t *kernel_pt;

static void zero(void *ptr, size_t size)
{
	for (size_t i = 0; i < size; i++) {
		((uint8_t *)ptr)[i] = 0x0;
	}
}

void vm_init(void)
{
	kernel_pt = vm_create_page_table();

	for (size_t i=0x80000000; i < 0x80600000; i+=0x1000) {
		vm_mmap(kernel_pt, 0xffffffff00000000 + i, i, VM_PTE_VALID | VM_PTE_READ | VM_PTE_WRITE | VM_PTE_EXEC);
	}

	vm_mmap(kernel_pt, 0x10000000, 0x10000000, VM_PTE_VALID | VM_PTE_READ | VM_PTE_WRITE | VM_PTE_EXEC);

	uart_puts("Virtual memory initialized.\n");
}

void vm_load_page_table(uint64_t *pt)
{
	riscv_set_satp(RISCV_MAKE_SATP(pt, RISCV_SATP_SV48));
	vm_reload();
}

uint64_t *vm_create_page_table(void)
{
	uint64_t *page_table = mm_alloc(sizeof(uint64_t) * 512);
	zero((void *)page_table, sizeof(uint64_t) * 512);
	return page_table;
}

void vm_mmap(uint64_t *table, size_t virtual, size_t physical, uint64_t flags)
{
	uint32_t vpn0 = (virtual >> 12) & 0x1ff;
	uint32_t vpn1 = (virtual >> 21) & 0x1ff;
	uint32_t vpn2 = (virtual >> 30) & 0x1ff;
	uint32_t vpn3 = (virtual >> 39) & 0x1ff;

	if (!table[vpn3]) {
		uint64_t address = (uint64_t)mm_alloc(sizeof(uint64_t) * 512);
		table[vpn3] = ((address >> 12) << 10) | VM_PTE_VALID;
		zero((void *)address, sizeof(uint64_t) * 512);
	}
	uint64_t *layer2 = (uint64_t *) ((table[vpn3] >> 10) << 12);
	if (!layer2[vpn2]) {
		uint64_t address = (uint64_t)mm_alloc(sizeof(uint64_t) * 512);
		layer2[vpn2] = ((address >> 12) << 10) | VM_PTE_VALID;
		zero((void *)address, sizeof(uint64_t) * 512);
	}
	uint64_t *layer1 = (uint64_t *) ((layer2[vpn2] >> 10) << 12);
	if (!layer1[vpn1]) {
		uint64_t address = (uint64_t)mm_alloc(sizeof(uint64_t) * 512);
		layer1[vpn1] = ((address >> 12) << 10) | VM_PTE_VALID;
		zero((void *)address, sizeof(uint64_t) * 512);
	}
	uint64_t *layer0 = (uint64_t *) ((layer1[vpn1] >> 10) << 12);

	uint64_t pte = ((physical >> 12) << 10) | (flags & 0xff);

	layer0[vpn0] = pte;
}

uint64_t vm_get_phys(uint64_t *table, size_t virtual)
{
	uint32_t vpn0 = (virtual >> 12) & 0x1ff;
	uint32_t vpn1 = (virtual >> 21) & 0x1ff;
	uint32_t vpn2 = (virtual >> 30) & 0x1ff;
	uint32_t vpn3 = (virtual >> 39) & 0x1ff;

	uint64_t *layer2 = (uint64_t *) ((table[vpn3] >> 10) << 12);
	uint64_t *layer1 = (uint64_t *) ((layer2[vpn2] >> 10) << 12);
	uint64_t *layer0 = (uint64_t *) ((layer1[vpn1] >> 10) << 12);

	return (layer0[vpn0] >> 10) << 12;
}

void vm_reload(void)
{
	__asm__ volatile("sfence.vma");
}
