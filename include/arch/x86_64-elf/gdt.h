#ifndef GDT_H
#define GDT_H

#include <stdint.h>

typedef uint64_t gdt_entry;
typedef uint64_t gdt_t[7];
typedef struct {
	uint16_t limit;
	uint64_t base;
} __attribute__((packed)) gdtr_t;

extern void load_gdt(uint16_t limit, uint64_t base);
void encode_gdt_entry(uint64_t *entry, uint64_t limit, uint64_t base, uint64_t access, uint64_t flags);

void init_gdt();

#endif
