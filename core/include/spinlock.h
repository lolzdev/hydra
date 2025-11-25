#ifndef SPINLOCK_H
#define SPINLOCK_H

#include <stdint.h>

struct spinlock {
	volatile uint8_t locked;
};

static void spinlock_aquire(struct spinlock *lock)
{
	while (lock->locked);
	lock->locked = 1;
}

static void spinlock_release(struct spinlock *lock)
{
	lock->locked = 0;
}

#endif
