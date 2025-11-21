#ifndef VM_H
#define VM_H
#include <stdint.h>
#include <stddef.h>

void vm_init(void);
void vm_mmap(uint64_t *table, size_t virtual, size_t physical, uint64_t flags);

#endif
