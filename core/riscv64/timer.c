#include <riscv64/timer.h>
#include <riscv64/isa.h>
#include <drivers/uart.h>
#include <sched.h>

uint64_t timer_interval;

void timer_init(void)
{
	timer_interval = CLOCK_FREQ / TICK_RATE_HZ;

	riscv_sbi_set_timer(riscv_get_time() + timer_interval);

	__asm__ volatile("csrs sie, %0" :: "r"(1 << 5));
}

void timer_handle_interrupt(void)
{
	riscv_sbi_set_timer(riscv_get_time() + timer_interval);
	sched_tick();
}
