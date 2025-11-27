#include <cpu.h>
#include <mm/buddy.h>
#include <drivers/uart.h>

struct cpu *CPUS = 0;

void cpu_init(uint16_t cpu_count)
{
	CPUS = mm_alloc(sizeof(struct cpu) * cpu_count);
	for (int i=0; i < cpu_count; i++) {
		CPUS[i].noff = 0;
		CPUS[i].intena = 0;
	}
}
