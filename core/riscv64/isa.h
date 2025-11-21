#ifndef ISA_H
#define ISA_H

#include <stdint.h>

static inline void riscv_set_satp(uint64_t satp)
{
	__asm__ volatile("csrw satp, %0" : : "r"(satp));
}

#endif
