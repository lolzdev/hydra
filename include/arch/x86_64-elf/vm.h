#ifndef VM_H
#define VM_H

#include <stdint.h>
#include <stddef.h>
#include <stdio.h>
#include <arch/x86_64-elf/mm.h>
#include <limine.h>

void init_paging(struct limine_memmap_response *memmap);
void kvmmap(uint64_t *pml4, void *phys, void *virt, uint64_t flags);
void kvmalloc(uint64_t *pml4, void *phys, void *virt, uint64_t flags);
void kernel_map(void *phys, void *virt, uint64_t flags);

#endif
