#ifndef INST_H
#define INST_H

#define outl(reg, addr) __asm__ volatile("outl %0, %1" : : "a"(addr), "Nd" (reg) : "memory")

static inline uint32_t inl(uint64_t reg)
{
	uint32_t data;
	__asm__ volatile("inl %1, %0" : "=a"(data) : "Nd"(reg) : "memory");
	return data;
}

#endif
