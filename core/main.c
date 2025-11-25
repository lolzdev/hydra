#include <stdint.h>
#include <drivers/uart.h>
#include <mm/buddy.h>
#include <riscv64/vm/vm.h>
#include <riscv64/isa.h>
#include <riscv64/timer.h>
#include <sched.h>

extern void kernel_trap_entry(void);
extern void kernel_init(void);
extern void sched_start(void);

void kmain(uint64_t hart_id, uint64_t device_tree)
{
	(void)device_tree;
	
	uart_puts("\033[2J\033[0;0HBooting hydra...\n");
	riscv_set_stvec((uint64_t)&kernel_trap_entry);
	buddy_init();
	vm_init();

	for (size_t i=0; i < 10; i++) {
		if (i == hart_id) continue;
		if (riscv_sbi_probe_hart(i) < 0) continue;

		void *hart_stack = mm_alloc_pages(4);
		uint64_t hart_stack_top = (uint64_t)hart_stack + (VM_PAGE_SIZE * 4);

		riscv_sbi_wake_up(i, hart_stack_top);
	}

	vm_load_page_table(kernel_pt.page_table);
	kernel_init();
	sched_init();

	timer_init();
	riscv_enable_interrupts();
}

void kmain_smp(void)
{
	vm_load_page_table(kernel_pt.page_table);
}
