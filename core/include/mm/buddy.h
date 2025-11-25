#ifndef BUDDY_H
#define BUDDY_H

#include <stdint.h>
#include <stddef.h>

#define PAGE_SIZE 0x1000
#define MIN_BLOCK PAGE_SIZE
#define MAX_LEVEL 7
#define MAX_BLOCK (MIN_BLOCK * (1 << MAX_LEVEL))

/*
 * There is a 99.999% possibility that in other
 * parts of the codebase only these functions are
 * needed:
 * - mm_alloc()
 * - mm_free()
 * - mm_alloc_pages()
 * - mm_free_pages()
 */

/* 
 * Initialize the allocator by setting the whole
 * heap as free.
 */
void buddy_init(void);
/*
 * Free a range of memory given a starting address
 * and a size.
 */
void mm_free_range(void *start, size_t size);
/*
 * Free a block by specifying its address and
 * the level.
 */
void mm_free_block(void *addr, uint8_t level);
/*
 * Free a previously allocated region of memory.
 * Similar to mm_free_block() but using a size
 * in bytes instead of a level.
 */
void mm_free(void *addr, size_t size);
/*
 * Same as mm_free() but uses a number of pages
 * specified by the `size` parameter.
 */
void mm_free_pages(void *addr, size_t size);
/*
 * Search for a free spot to allocate a new block
 * given its level. If there isn't enough free
 * space the function returns NULL.
 */
void *mm_alloc_block(uint8_t level);
/*
 * Given a block, return its buddy. Check out
 * the algorithm description to find out more.
 */
struct block *mm_buddy(struct block *block);
/*
 * Check out whether, given a block, its buddy
 * is free. Used when freeing to eventually
 * merge blocks.
 */
uint8_t mm_buddy_is_free(struct block *block);
/*
 * Create a new block given its address and level.
 */
struct block *mm_create_block(size_t addr, uint8_t level);
/*
 * Allocate a number of pages specified by `size`.
 */
void *mm_alloc_pages(size_t size);
/*
 * Allocate a memory region with a size in bytes
 * specified by `size`.
 */
void *mm_alloc(size_t size);

#endif
