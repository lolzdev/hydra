	/*-
	 * Copyright 2024 Lorenzo Torres
 	 *
	 * Redistribution and use in source and binary forms, with or without
	 * modification, are permitted provided that the following conditions are met:
	 * 1. Redistributions of source code must retain the above copyright
	 *    notice, this list of conditions and the following disclaimer.
	 * 2. Redistributions in binary form must reproduce the above copyright
	 *    notice, this list of conditions and the following disclaimer in the
	 *    documentation and/or other materials provided with the distribution.
	 * 3. Neither the name of the copyright holder nor
	 *    the names of its contributors may be used to endorse or promote products
	 *    derived from this software without specific prior written permission.
	 *
	 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS “AS IS” AND ANY
	 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
	 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
	 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY
	 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
	 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
	 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
	 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
	 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
	 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
	 */

#include <mm/mm.h>
#include <log/fb.h>

#define POW2(x) (1 << (x))
#define LEVEL_SIZE(x) (MIN_BLOCK * POW2(x))

struct block {
	struct block *next;
	size_t level;
};

static size_t mem_size = 0x0;
static struct block *freelist[MAX_LEVEL+1];

void mm_init(struct limine_memmap_entry **memmap, uint64_t entry_count, uint64_t offset)
{
	kprintf("offset: %x\n", offset);
	size_t total_free = 0;
	size_t max_size = 0;
	size_t max_base = 0;
	for (size_t i=0; i < entry_count; i++) {
		struct limine_memmap_entry *map = memmap[i];
		size_t size = map->length;
		size_t end = (map->base + offset) + size;
		//kprintf("region: %x - %x, %x\n", map->base, end, map->type);
		if (end > mem_size) { 
			mem_size = end;
		}

		if (size > max_size && map->type == LIMINE_MEMMAP_USABLE) {
			max_size = size;
			max_base = map->base + offset;
		}

		/*
		if (map->type == LIMINE_MEMMAP_USABLE && map->base > 0) {
			total_free += size;
		}*/
	}
	kprintf("max area: %x - %x\n", max_base, max_base + max_size);
	mm_free_range((void *) max_base, max_size);
}

void mm_free_range(void *start, size_t size)
{
	uint8_t level = 0;
	while (size > LEVEL_SIZE(level)) {
		level++;
	}

	if (level > MAX_LEVEL) level = MAX_LEVEL;
	struct block *prev = mm_create_block((size_t) start, level);
	freelist[MAX_LEVEL] = prev;
	
	size_t count =0;
	for (size_t i = (size_t) start + LEVEL_SIZE(level); i < (size_t) start + size; i += LEVEL_SIZE(level)) {
		if (i >= (size_t) start + size) break;
		struct block *block = mm_create_block(i, level);
		prev->next = block;
		prev = block;
		count++;
	}
	kprintf("created %d new blocks\n", count);
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

void mm_free(void *addr, uint8_t level)
{
	struct block *b = mm_create_block(addr, level);
	
	uint8_t result = mm_merge(b);
	while (result) {
		result = mm_merge(b);
	}

	b->next = freelist[b->level];
	freelist[b->level] = b;
}

void *mm_alloc_block(uint8_t level) {
	if (level < 0) level = 0;
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
		return 0xcafebabe;
	}

	while (b->level > level) {
		mm_split(b);
	}

	return (void *) b;
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

void *mm_alloc(size_t size)
{
	size_t pages = size/PAGE_SIZE;
	if (pages == 0) pages = 1;
	return mm_alloc_pages(pages);
}
