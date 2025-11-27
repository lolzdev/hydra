#include <drivers/uart.h>
#include <riscv64/isa.h>
#include <riscv64/timer.h>
#include <riscv64/vm/vm.h>
#include <sched.h>
#include <proc.h>
#include <spinlock.h>
#include <cpu.h>

extern void syscall_handle(struct frame *frame);
extern struct process_node **current_proc;
extern struct cpu *CPUS;

#define INTERRUPT_MASK (1L << 63)

void trap_handler(void)
{
	uint64_t satp = 0;
	__asm__ volatile("csrr %0, satp" : "=r"(satp) :);
	vm_load_page_table(kernel_pt.page_table);
	uint64_t scause = riscv_get_scause();
	uint64_t stval = 0;
	uint64_t sepc = 0;
	uint64_t sp = 0;
	__asm__ volatile("csrr %0, stval" : "=r"(stval) :);
	__asm__ volatile("csrr %0, sepc" : "=r"(sepc) :);
	__asm__ volatile("mv %0, sp" : "=r"(sp) :);
	uint64_t interrupt = scause >> 63;
	uint64_t exception = scause & ~INTERRUPT_MASK;
	uint64_t tp = 0;
	__asm__ volatile("mv %0, tp" : "=r"(tp) :);

	if (interrupt) {
		switch(exception) {
		case 1:
			__asm__ volatile("csrc sip, %0" :: "r"(1 << 1));
			break;
		case 5:
			//uart_printf("timer: %d\n", tp);
			timer_handle_interrupt();
			return;
		default:
			uart_printf("sp: 0x%x, satp: 0x%x, tp: %d\nsepc: 0x%x\nstval: 0x%x\nscause: 0x%x\n", sp, satp, tp, sepc, stval, scause);
			uart_puts("Unknown interrupt\n");
			break;
		}
	} else {
		switch(exception) {
		case 0:
			uart_printf("sp: 0x%x, satp: 0x%x, tp: %d\nsepc: 0x%x\nstval: 0x%x\nscause: 0x%x\n", sp, satp, tp, sepc, stval, scause);
			uart_puts("Instruction address misaligned\n");
			break;
		case 1:
			uart_puts("Instruction access fault\n");
			break;
		case 2:
			uart_puts("Illegal instruction\n");
				while (1);
			break;
		case 3:
			uart_puts("Breakpoint\n");
			break;
		case 4:
			uart_puts("Load address misaligned\n");
			break;
		case 5:
			uart_printf("sp: 0x%x, satp: 0x%x, tp: %d\nsepc: 0x%x\nstval: 0x%x\nscause: 0x%x\n", sp, satp, tp, sepc, stval, scause);
			uart_puts("Load access fault\n");
			break;
		case 6:
			uart_puts("Store address misaligned\n");
			break;
		case 7:
			uart_puts("Store access fault\n");
			break;
		case 8:
			syscall_handle(&current_proc[tp]->proc.frame);
			uint64_t test = current_proc[tp]->proc.frame.pc;
			//uart_printf("fork %x\n", &current_proc[tp]->proc.frame.pc);
			current_proc[tp]->proc.frame.pc += 4;
			//uart_printf("fork %x\n", test + 4);
			sched_tick();
			break;
		case 9:
			uart_puts("Environment call from supervisor\n");
			break;
		case 12:
			uart_printf("sp: 0x%x, satp: 0x%x, tp: %d\nsepc: 0x%x\nstval: 0x%x\nscause: 0x%x\n", sp, satp, tp, sepc, stval, scause);
			uart_puts("Instruction page fault\n");
			while (1);
			break;
		case 13:
			uart_printf("sp: 0x%x, satp: 0x%x, tp: %d\nsepc: 0x%x\nstval: 0x%x\nscause: 0x%x\n", sp, satp, tp, sepc, stval, scause);
			uart_puts("Load page fault\n");
			while (1);
			break;
		case 15:
			uart_puts("Store page fault\n");
			while (1);
			break;
		default:
			uart_puts("Unknown exception\n");
			while (1);
		}
	}
}
