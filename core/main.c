#include <stdint.h>
#include "uart.h"
#include "mm/buddy.h"
#include "riscv64/vm/vm.h"

void kmain(uint64_t hart_id, uint64_t device_tree)
{
	(void)hart_id;
	(void)device_tree;
	
	uart_puts("Booting hydra...\n");
	buddy_init();
	vm_init();
}
