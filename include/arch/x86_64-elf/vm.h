#ifndef VM_H
#define VM_H

#include <stdint.h>
#include <stddef.h>
#include <stdio.h>

void init_paging();
void kvmmap(uint64_t *pml4, void *phys, void *virt, uint64_t flags);

#endif
