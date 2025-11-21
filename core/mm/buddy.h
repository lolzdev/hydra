#ifndef BUDDY_H
#define BUDDY_H

#include <stdint.h>
#include <stddef.h>

#define PAGE_SIZE 0x1000
#define MIN_BLOCK PAGE_SIZE
#define MAX_LEVEL 7
#define MAX_BLOCK (MIN_BLOCK * (1 << MAX_LEVEL))

void buddy_init(void);
void mm_free_range(void *start, size_t size);
void mm_free_block(void *addr, uint8_t level);
void mm_free(void *addr, size_t size);
void mm_free_pages(void *addr, size_t size);
void *mm_alloc_block(uint8_t level);
struct block *mm_buddy(struct block *block);
uint8_t mm_buddy_is_free(struct block *block);
struct block *mm_create_block(size_t addr, uint8_t level);
void *mm_alloc_pages(size_t size);
void *mm_alloc(size_t size);

#endif
