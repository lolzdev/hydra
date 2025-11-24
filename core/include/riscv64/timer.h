#ifndef RISCV_TIMER_H
#define RISCV_TIMER_H
#include <stdint.h>

#define CLOCK_FREQ 10000000
#define TICK_RATE_HZ 100

void timer_init(void);
void timer_handle_interrupt(void);

#endif
