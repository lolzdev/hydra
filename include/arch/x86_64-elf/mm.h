#ifndef MM_H
#define MM_H

#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <limine.h>

#define NPAGES 1024
#define PAGE_SIZE 0x1000
#define PAGE_FREE 0
#define PAGE_USED 1 << 1
#define PAGE_LAST 1 << 2

uint64_t init_mm(struct limine_memmap_response *memmap);
void *k_page_alloc(uint16_t pages);
void *k_page_zalloc(uint16_t pages);
void k_page_free(void *addr);

#endif
