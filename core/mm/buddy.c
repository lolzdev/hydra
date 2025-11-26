/*
 * Buddy memory allocator. The algorithm is described in
 * Knuth, D. E. (1997). The Art of Computer Programming:
 * Fundamental Algorithms, Volume 1. Addison-Wesley Professional.
 */

#include <mm/buddy.h>
#include <drivers/uart.h>
#include <spinlock.h>
#include <mem.h>

#define POW2(x) (1 << (x))
#define LEVEL_SIZE(x) (MIN_BLOCK * POW2(x))
#define ALIGNUP(data, align) (((data) + (align) - 1) & ~((align) - 1))
#define ALIGNDOWN(data, align) ((data) & ~((align) - 1))

struct block {
	struct block *next;
	size_t level;
};

/* Provided by the linker */
extern unsigned char __memory_start[];
/*
 * Array of linked lists. Each linked list
 * contains all the blocks available for
 * allocation at a given level.
 */
static struct block *freelist[MAX_LEVEL+1];

static struct spinlock lock;

void buddy_init(void)
{
	lock.locked = 0;
	size_t ram_base = (size_t)__memory_start;
	/* Currently the memory has a fixed size, should be changed */
	size_t ram_size = 128 * 1024 * 1024;
	size_t memory_end = ram_base + ram_size;

	size_t memory_size = memory_end - ram_base;

	mm_free_range((void *)__memory_start, memory_size);
	uart_puts("Buddy allocator initialized.\n");
}

void mm_free_range(void *start, size_t size)
{
	size_t start_addr = (size_t)start;
	size_t aligned_start = ALIGNUP(start_addr, 4096);

	if (aligned_start > start_addr + size) return;
	size -= (aligned_start - start_addr);

	start = (void*)aligned_start;

	size_t current_addr = (size_t)start;
	size_t end_addr = current_addr + size;

	/*
	 * Create new blocks to cover all the available memory.
	 */
	while (current_addr + LEVEL_SIZE(0) <= end_addr) {
		uint8_t level = MAX_LEVEL;
		while (level > 0) {
			size_t blk_size = LEVEL_SIZE(level);
			if (current_addr + blk_size > end_addr) {
				level--;
				continue;
			}
			if (current_addr & (blk_size - 1)) {
				level--;
				continue;
			}
			break;
		}

		struct block *b = mm_create_block(current_addr, level);
		b->next = freelist[level];
		freelist[level] = b;

		current_addr += LEVEL_SIZE(level);
	}
}

struct block *mm_create_block(size_t addr, uint8_t level)
{
	struct block *b = (struct block *) addr;
	b->level = level;
	b->next = NULL;

	return b;
}

struct block *mm_buddy(struct block *block)
{
	return (struct block *) ((size_t)block ^ LEVEL_SIZE(block->level));
}

uint8_t mm_is_free(struct block *block)
{
	if (!block) return 0;
	if (block->level > MAX_LEVEL) return 0;
	struct block *b = freelist[block->level];

	while (b != NULL) {
		if (b == block) {
			return 1;
		}

		b = b->next;
	}

	return 0;
}

void mm_split(struct block *b)
{
	b->level--;
	struct block *buddy = mm_create_block((size_t) mm_buddy(b), b->level);
	buddy->next = freelist[b->level];
	freelist[b->level] = buddy;
}

/*
 * Merge block `b` with its buddy. Return 1
 * if the merge was successful, 0 otherwise.
 */
uint8_t mm_merge(struct block *b)
{
	struct block *buddy = mm_buddy(b);

	if (mm_is_free(buddy)) {
		struct block *s = freelist[buddy->level];
		struct block *prev = NULL;

		while (s != NULL) {
			if (s == buddy) {
				if (prev != NULL) {
					prev->next = s->next;
				} else {
					freelist[buddy->level] = s->next;
				}
			}
			
			prev = s;
			s = s->next;
		}

		if (buddy < b) {
			b = buddy;
		}

		b->level++;
		return 1;
	}

	return 0;
}

void mm_free_block(void *addr, uint8_t level)
{
	/* Create a new block to represent the free space */
	struct block *b = mm_create_block((size_t)addr, level);
	
	/* 
	 * Merge the block with its buddy until there is
	 * no free block anymore.
	 */
	uint8_t result = mm_merge(b);
	while (result) {
		result = mm_merge(b);
	}

	b->next = freelist[b->level];
	freelist[b->level] = b;
}

void *mm_alloc_block(uint8_t level) {
	if (level > MAX_LEVEL) level = MAX_LEVEL;

	if (freelist[level] != NULL) {
		struct block *b = freelist[level];
		if (b == NULL) return NULL;
		freelist[level] = b->next;
		return (void *) b;
	}

	struct block *b = NULL;

	for (int i=level; i <= MAX_LEVEL; i++) {
		if (freelist[i] != NULL) {
			b = freelist[i];
			if (b == NULL) continue;
			freelist[i] = b->next;
			break;
		}
	}

	if (b == NULL) {
		return 0x0;
	}

	while (b->level > level) {
		mm_split(b);
	}

	return (void *) b;
}

void mm_free_pages(void *addr, size_t size)
{
	spinlock_aquire(&lock);
	uint8_t level = 0;
	while ((size * PAGE_SIZE) > (size_t)LEVEL_SIZE(level)) level++;

	mm_free_block(addr, level);
	spinlock_release(&lock);
}

void *mm_alloc_pages(size_t size)
{
	uint8_t level = 0;
	while ((size * PAGE_SIZE) > (size_t)LEVEL_SIZE(level)) level++;

	/* 
	 * In this case, no free block was found, we can assume
	 * that the memory is full.
	 */
	if (level > MAX_LEVEL) return 0x0;

	spinlock_aquire(&lock);
	struct block *b = mm_alloc_block(level);
	memset(b, 0x0, size * 0x1000);
	spinlock_release(&lock);

	return b;
}

void mm_free(void *addr, size_t size)
{
	size = ALIGNUP(size, PAGE_SIZE);
	size_t pages = size/PAGE_SIZE;
	if (pages == 0) pages = 1;
	mm_free_pages(addr, pages);
}

void *mm_alloc(size_t size)
{
	size = ALIGNUP(size, PAGE_SIZE);
	size_t pages = size/PAGE_SIZE;
	if (pages == 0) pages = 1;
	return mm_alloc_pages(pages);
}
