#ifndef SPINLOCK_H
#define SPINLOCK_H

#include <stdint.h>
#include <stdatomic.h>

struct spinlock {
	atomic_bool locked;
};

static void spinlock_aquire(struct spinlock *lock)
{
	while (atomic_exchange_explicit(&lock->locked, true, memory_order_acquire));
}

static void spinlock_release(struct spinlock *lock)
{
	atomic_store_explicit(&lock->locked, false, memory_order_release);
}

#endif
