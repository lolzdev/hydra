#include "buddy.h"
#include "../uart.h"

#define POW2(x) (1 << (x))
#define LEVEL_SIZE(x) (MIN_BLOCK * POW2(x))
#define ALIGNUP(data, align) (((data) + (align) - 1) & ~((align) - 1))
#define ALIGNDOWN(data, align) ((data) & ~((align) - 1))

struct block {
	struct block *next;
	size_t level;
};

extern unsigned char __memory_start[];
static struct block *freelist[MAX_LEVEL+1];

void buddy_init(void)
{
	size_t memory_end = 0x80000000;
	size_t memory_size = memory_end - (size_t)__memory_start;

	mm_free_range((void *)__memory_start, memory_size);
	uart_puts("Buddy allocator initialized.\n");
}

void mm_free_range(void *start, size_t size)
{
	uint8_t level = 0;
	while (size > (size_t)LEVEL_SIZE(level)) {
		if (level > MAX_LEVEL) {
			level = MAX_LEVEL;
			break;
		}
		level++;
	}

	struct block *prev = mm_create_block((size_t) start, level);
	freelist[level] = prev;
	
	size_t count = 0;
	for (size_t i = (size_t) start + LEVEL_SIZE(level); i < (size_t) start + size; i += LEVEL_SIZE(level)) {
		if (i >= (size_t) start + size) break;
		struct block *block = mm_create_block(i, level);
		prev->next = block;
		prev = block;
		count++;
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

		b->level++;
		
		return 1;
	}

	return 0;
}

void mm_free_block(void *addr, uint8_t level)
{
	struct block *b = mm_create_block((size_t)addr, level);
	
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
		freelist[level] = b->next;
		return (void *) b;
	}

	struct block *b = NULL;

	for (int i=level; i <= MAX_LEVEL; i++) {
		if (freelist[i] != NULL) {
			b = freelist[i];
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
	uint8_t level = 0;
	while ((size * PAGE_SIZE) > LEVEL_SIZE(level)) level++;

	mm_free_block(addr, level);
}

void *mm_alloc_pages(size_t size)
{
	uint8_t level = 0;
	while ((size * PAGE_SIZE) > LEVEL_SIZE(level)) level++;

	/* In this case, no free block was found, we can assume
	 * that the memory is full.
	 */
	if (level > MAX_LEVEL) return 0x0;

	return mm_alloc_block(level);
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
