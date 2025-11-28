#include <stdint.h>
#include <drivers/uart.h>
#include <mm/buddy.h>
#include <riscv64/vm/vm.h>
#include <riscv64/isa.h>
#include <riscv64/timer.h>
#include <sched.h>
#include <cpu.h>
#include <fdt.h>

extern void kernel_trap_entry(void);
extern void kernel_init(void);
extern void sched_start(void);

static volatile uint8_t started = 0;

void kmain(uint64_t hartid, uint64_t device_tree)
{
	(void) hartid;
	buddy_boot();
	uart_init();
	uart_puts("Booting hydra...\n");
	riscv_set_stvec((uint64_t)&kernel_trap_entry);

	vm_init();
	vm_mmap(kernel_pt.page_table, device_tree, device_tree, VM_PTE_VALID | VM_PTE_READ | VM_PTE_WRITE | VM_PTE_EXEC);
	vm_load_page_table(kernel_pt.page_table);

	

	hw_info info = {0};
	struct fdt_header *header = (struct fdt_header *) device_tree;

	size_t fdt_size = (((header->totalsize) + (VM_PAGE_SIZE) - 1) & ~((VM_PAGE_SIZE) - 1));
	size_t fdt_pages = fdt_size/VM_PAGE_SIZE;
	if (fdt_pages == 0) fdt_pages = 1;
	for (size_t i=0; i < fdt_pages; i++) {
		vm_mmap(kernel_pt.page_table, device_tree + (i * VM_PAGE_SIZE), device_tree + (i * VM_PAGE_SIZE), VM_PTE_VALID | VM_PTE_READ | VM_PTE_WRITE | VM_PTE_EXEC);
	}

	vm_reload();

	fdt_probe(header, &info);
	buddy_init(header);
	
	cpu_init(info.cpu_count);

	for (uint16_t i=0; i < info.cpu_count; i++) {
		void *hart_stack = mm_alloc_pages(4);
		uint64_t hart_stack_top = (uint64_t)hart_stack + (VM_PAGE_SIZE * 4);
		riscv_sbi_wake_up(i, hart_stack_top);
	}

	sched_init(info.cpu_count);
	kernel_init();

	timer_init();
	riscv_enable_interrupts();

	__sync_synchronize();
	started = 1;
}

void kmain_smp(void)
{
	while (started != 1);
	__sync_synchronize();
	vm_load_page_table(kernel_pt.page_table);
	riscv_set_stvec((uint64_t)&kernel_trap_entry);
	timer_init();
	riscv_enable_interrupts();
}
