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

struct block {
	struct block *next;
	struct block *prev;
	size_t kval;
};

static size_t mem_size = 0x0;
static struct hydra_memmap *MMAP;
static size_t MMAP_SIZE;
static struct block *freelists[MAX_SIZE - MIN_SIZE];
static uint64_t free_bitmaps[MAX_SIZE-MIN_SIZE];

static inline uint32_t log2(const uint32_t x) {
	uint32_t y;
	asm ( "\tbsr %1, %0\n"
		: "=r"(y)
		: "r" (x)
	);
	return y;
}

void mm_init(struct hydra_memmap *memmap, size_t count)
{
	MMAP = memmap;
	MMAP_SIZE = count;
	for (int i=0; i < count; i++) {
		struct hydra_memmap map = memmap[i];
		size_t size = map.pages / PAGE_SIZE;
		size_t end = map.phys_start + size;
		if (end > mem_size) { 
			mem_size = end;
		}

		if (map.type == HYDRA_MEMMAP_FREE)
			mm_free_range((void *)map.phys_start, (void *)end);
	}
}

void mm_free_range(void *start, void *end)
{
	for (size_t i = (size_t)start; i < (size_t)end; i += PAGE_SIZE) {
		mm_free(i);
	}
}

struct block *mm_buddy(struct block *block)
{
	uint64_t addr = (uint64_t) block;
	
	if (addr % POW2(block->kval + 1) == 0) 
		return (struct block *)((uint64_t)block + POW2(block->kval - 1));
	else
		return (struct block *)((uint64_t)block - POW2(block->kval - 1));
}

uint8_t mm_buddy_is_free(struct block *block)
{
	struct block *buddy = mm_buddy(block);
	if (block->next == buddy || block->prev == buddy)
		return 1;
	else
		return 0;
}

void mm_free(void *addr)
{
	struct block *block;

	if (((uint64_t)addr % PAGE_SIZE) != 0 || (size_t) addr >= mem_size)
		return;

	block = (struct block*) addr;
	if (mm_buddy_is_free(block)) {
		struct block *buddy = mm_buddy(block);
		block->prev->next = block->next;
		block->next->prev = block->prev;
		buddy->prev->next = buddy->next;
		buddy->next->prev = buddy->prev;

		if ((uint64_t)block > (uint64_t)buddy)
			block = buddy;
		block->kval += 1;
	}

	block->next = freelists[block->kval - MIN_SIZE];
	block->prev = NULL;
	freelists[block->kval - MIN_SIZE] = block;
}

void *mm_alloc(size_t size) {
	if (size < MIN_SIZE) size = MIN_SIZE;
	if (size > MAX_SIZE) size = MAX_SIZE;

	struct block *block = NULL;

	if (freelists[size - MIN_SIZE] != NULL) {
		block = freelists[size - MIN_SIZE];
		goto found;
	}

	for (size_t i = size - MIN_SIZE; i < MAX_SIZE - MIN_SIZE; i++) {
		if (freelists[i] != NULL) {
			block = freelists[size - MIN_SIZE];
			break;
		}
	}

	if (block == NULL) return NULL;

found:
	while (block->kval-1 >= 2*size) {
		struct block *buddy = mm_buddy(block);
		block->kval = block->kval - 1
		buddy->kval = block->kval;
	}
}


