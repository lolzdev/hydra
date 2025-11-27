#include <stdint.h>
#include <drivers/uart.h>
#include <mm/buddy.h>
#include <riscv64/vm/vm.h>
#include <riscv64/isa.h>
#include <riscv64/timer.h>
#include <sched.h>
#include <cpu.h>

extern void kernel_trap_entry(void);
extern void kernel_init(void);
extern void sched_start(void);

static volatile uint8_t started = 0;

void kmain(uint64_t hart_id, uint64_t device_tree)
{
	(void)device_tree;

	uint16_t cpu_count = 3;

	buddy_init();
	cpu_init(cpu_count);
	uart_init();
	uart_puts("Booting hydra...\n");
	riscv_set_stvec((uint64_t)&kernel_trap_entry);
	vm_init();

	vm_load_page_table(kernel_pt.page_table);
	sched_init(cpu_count);
	kernel_init();

	for (uint16_t i=0; i < cpu_count; i++) {
		void *hart_stack = mm_alloc_pages(4);
		uint64_t hart_stack_top = (uint64_t)hart_stack + (VM_PAGE_SIZE * 4);
		riscv_sbi_wake_up(i, hart_stack_top);
	}

	timer_init();
	riscv_enable_interrupts();

	__sync_synchronize();
	started = 1;
}

void kmain_smp(uint64_t hartid)
{
	while (started != 1);
	__sync_synchronize();
	vm_load_page_table(kernel_pt.page_table);
	riscv_set_stvec((uint64_t)&kernel_trap_entry);
	timer_init();
	riscv_enable_interrupts();
}
