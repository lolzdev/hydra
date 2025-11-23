#include <drivers/uart.h>
#include <riscv64/isa.h>
#include <riscv64/timer.h>

#define INTERRUPT_MASK (1L << 63)

void trap_handler()
{
	uint64_t scause = riscv_get_scause();
	uint64_t interrupt = scause >> 63;
	uint64_t exception = scause & ~INTERRUPT_MASK;

	if (interrupt) {
		switch(exception) {
		case 1:
			uart_puts("Syscall\n");
			break;
		case 5:
			timer_handle_interrupt();
			break;
		default:
			uart_puts("Unknown interrupt\n");
			break;
		}
	} else {
		switch(exception) {
		case 0:
			uart_puts("Instruction address misaligned\n");
			break;
		case 1:
			uart_puts("Instruction access fault\n");
			break;
		case 2:
			uart_puts("Illegal instruction\n");
			break;
		case 3:
			uart_puts("Breakpoint\n");
			break;
		case 4:
			uart_puts("Load address misaligned\n");
			break;
		case 5:
			uart_puts("Load access fault\n");
			break;
		case 6:
			uart_puts("Store address misaligned\n");
			break;
		case 7:
			uart_puts("Store access fault\n");
			break;
		case 8:
			uart_puts("Environment call from userspace\n");
			break;
		case 9:
			uart_puts("Environment call from supervisor\n");
			break;
		case 12:
			uart_puts("Instruction page fault\n");
			break;
		case 13:
			uart_puts("Load page fault\n");
			break;
		case 15:
			uart_puts("Store page fault\n");
			break;
		default:
			uart_puts("Unknown exception\n");
		}
		while (1);
	}
}
