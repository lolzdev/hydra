#ifndef SPINLOCK_H
#define SPINLOCK_H

#include <stdint.h>

struct spinlock {
	volatile uint8_t locked;
};

static void spinlock_aquire(struct spinlock *lock)
{
	lock->locked = 1;
}

static void spinlock_release(struct spinlock *lock)
{
	lock->locked = 0;
}

static void spinlock_wait(struct spinlock *lock)
{
	while (lock->locked);
}

#endif
