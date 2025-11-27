#include <cpu.h>
#include <spinlock.h>
#include <drivers/uart.h>
#include <riscv64/isa.h>

extern struct cpu *CPUS;

void pop_off(void);
void push_off(void);

void spinlock_acquire(spinlock *lock)
{
	push_off(); 

	uint64_t tp = 0;
	__asm__ volatile("mv %0, tp" : "=r"(tp) :);
	if (lock->cpu == &CPUS[tp]) {
		// panic
	}

	int expected = 0;
	int desired = 1;

	while (!atomic_compare_exchange_strong(&lock->locked, &expected, desired)) {
		expected = 0;
	}

	__sync_synchronize(); 

	lock->cpu = &CPUS[tp];
}

void spinlock_release(spinlock *lock)
{
	lock->cpu = 0;
	__sync_synchronize();
	atomic_store(&lock->locked, 0);

	pop_off();
}

void pop_off(void)
{
	uint64_t tp = 0;
	__asm__ volatile("mv %0, tp" : "=r"(tp) :);
	if (!CPUS) return;
	struct cpu *c = &CPUS[tp];
	c->noff -= 1;
	if (c->noff == 0 && c->intena) {
		riscv_enable_interrupts();
	}
}

void push_off(void)
{
	uint64_t tp = 0;
	__asm__ volatile("mv %0, tp" : "=r"(tp) :);

	int32_t old = riscv_get_sstatus() & RISCV_SSTATUS_SIE;
	riscv_disable_interrupts();

	if (!CPUS) return;
	if (CPUS[tp].noff == 0) {
		CPUS[tp].intena = old;
	}
	CPUS[tp].noff += 1;
}
