#ifndef SPINLOCK_H
#define SPINLOCK_H

#include <stdint.h>
#include <stdatomic.h>

typedef struct {
	_Atomic int32_t locked;
	struct cpu *cpu;
} spinlock;

#define SPINLOCK_INIT 0

void spinlock_acquire(spinlock *lock);
void spinlock_release(spinlock *lock);

#endif
