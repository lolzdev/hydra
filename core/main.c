#include <stdint.h>
#include <drivers/uart.h>
#include <mm/buddy.h>
#include <riscv64/vm/vm.h>
#include <riscv64/isa.h>
#include <riscv64/timer.h>
#include <sched.h>

extern void trap_handler(void);
extern void kernel_init(void);
extern void sched_start(void);


void kmain(uint64_t hart_id, uint64_t device_tree)
{
	(void)hart_id;
	(void)device_tree;
	
	uart_puts("Booting hydra...\n");
	riscv_set_stvec((uint64_t)&trap_handler);
	buddy_init();
	vm_init();
	vm_load_page_table(kernel_pt);
	uart_puts("Paging enabled\n");
	kernel_init();
	sched_init();

	timer_init();
	riscv_enable_interrupts();
}
