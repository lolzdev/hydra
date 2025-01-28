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

#ifndef VM_H
#define VM_H

#include <stdint.h>
#include <stddef.h>
#include <limine.h>

#define PAGE_ADDR_MASK 0x000ffffffffff000

#define PAGE_PRESENT 0x1
#define PAGE_WRITABLE 0x1 << 1
#define PAGE_USER 0x1 << 2
#define PAGE_ENTRY(address, flags) ((address & PAGE_ADDR_MASK) | flags)

/* These are tables that represent the various levels of the
 * linear address translation done by the MMU.
 * Each table contains 512 entries, occupying exactly 1 page.
 */
typedef uint64_t *pml4_t;
typedef uint64_t *pdpt_t;
typedef uint64_t *pd_t;
typedef uint64_t *pt_t;

extern void pt_load(uint64_t pml4);

void vm_mmap(pml4_t pml4, void *virtual, void *physical, uint8_t flags);
void vm_kmmap(void *virtual, void *physical, uint8_t flags);
void vm_init(struct limine_memmap_entry **memmap, uint64_t entry_count, uint64_t offset);
void *vm_get_phys(pml4_t pml4, void *virtual);

#endif
