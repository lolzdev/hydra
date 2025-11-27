#ifndef CPU_H
#define CPU_H
#include <stdint.h>

struct cpu {
	int32_t noff;
	int32_t intena;
};

void cpu_init(uint16_t cpu_count);

#endif
