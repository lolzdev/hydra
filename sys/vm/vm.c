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

#include <vm/vm.h>
#include <mm/mm.h>
#include <log/fb.h>
#include <string.h>

__attribute__((used, section(".limine_requests")))
static volatile struct limine_executable_address_request executable_request = {
    .id = LIMINE_EXECUTABLE_ADDRESS_REQUEST,
    .revision = 0
};

static pml4_t k_page_table;
static pml4_t k_reverse_table;
static uint64_t virt_offset;
extern void user_function(void);

void vm_init(struct limine_memmap_entry **memmap, uint64_t entry_count, uint64_t offset)
{
	virt_offset = offset;
	k_page_table = mm_alloc_pages(1);
	k_reverse_table = mm_alloc_pages(1);
	memset(k_page_table, 0x0, PAGE_SIZE);
	memset(k_reverse_table, 0x0, PAGE_SIZE);
	vm_kmmap(k_page_table, k_page_table, PAGE_PRESENT | PAGE_WRITABLE);
	vm_kmmap(k_reverse_table, k_reverse_table, PAGE_PRESENT | PAGE_WRITABLE);
	
	for (size_t i=0; i < entry_count; i++) {
		struct limine_memmap_entry *map = memmap[i];
		size_t size = map->length;
		size_t end = (map->base + offset) + size;

		for (size_t i=0; i < size / 0x1000; i++) {
			size_t virt = ((map->base + offset) & ~(0xfff)) + (i*0x1000);
			if (map->type == 0x6 && map->base == executable_request.response->physical_base) {
				size_t phys = 0xffffffff80000000 + (i * 0x1000);
				vm_kmmap(phys, map->base + (i*0x1000), PAGE_PRESENT | PAGE_WRITABLE);
				vm_mmap(k_reverse_table, map->base + (i*0x1000), phys, PAGE_PRESENT | PAGE_WRITABLE );
			} else if (map->type != 0x6) {
				vm_kmmap(virt, virt-offset, PAGE_PRESENT | PAGE_WRITABLE);
				vm_mmap(k_reverse_table, virt-offset, virt, PAGE_PRESENT | PAGE_WRITABLE);
			}
		}
	}

	vm_reload();
}

void vm_reload(void)
{
	size_t addr = (size_t) k_page_table - virt_offset;
	__asm__ ("movq %0, %%cr3;"::"b"((uint64_t) addr));
}

void *vm_get_kvirt(void *physical)
{
	return vm_get_phys(k_reverse_table, physical);
}

void *vm_get_kphys(void *virtual)
{
	return vm_get_phys(k_page_table, virtual);
}

void *vm_get_phys(pml4_t pml4, void *virtual)
{
	size_t pml4e = 	((size_t) virtual >> 39) & 0x1ff;
	size_t pdpte = 	((size_t) virtual >> 30) & 0x1ff;
	size_t pde 	 = 	((size_t) virtual >> 21) & 0x1ff;
	size_t pte 	 = 	((size_t) virtual >> 12) & 0x1ff;

	pdpt_t pdpt = (pml4[pml4e] & ~0xfff)+virt_offset;
	pd_t pd = (pdpt[pdpte] & ~0xfff)+virt_offset;
	pt_t pt = (pd[pde] & ~0xfff)+virt_offset;
	return (void *) ((pt[pte] & ~0xfff));
}

void vm_mmap(pml4_t pml4, void *virtual, void *physical, uint8_t flags)
{
	size_t pml4e = 	((size_t) virtual >> 39) & 0x1ff;
	size_t pdpte = 	((size_t) virtual >> 30) & 0x1ff;
	size_t pde 	 = 	((size_t) virtual >> 21) & 0x1ff;
	size_t pte 	 = 	((size_t) virtual >> 12) & 0x1ff;

	pdpt_t pdpt;
	pd_t pd;
	pt_t pt;
	
	if (!(pml4[pml4e] & 0x1)) {
		pdpt = mm_alloc_pages(1);
		memset(pdpt, 0x0, PAGE_SIZE);
		size_t entry = ((size_t)pdpt & ~0x1ff) | flags;
		pml4[pml4e] = entry-virt_offset;
	} else {
		pdpt = (pml4[pml4e] & ~0x1ff) + virt_offset;
	}

	if (!(pdpt[pdpte] & 0x1)) {
		pd = mm_alloc_pages(1);
		memset(pd, 0x0, PAGE_SIZE);
		pdpt[pdpte] = (((size_t)pd & ~0x1ff) | flags)-virt_offset;
	} else {
		pd = (pdpt[pdpte] & ~0x1ff) + virt_offset;
	}

	if (!(pd[pde] & 0x1)) {
		pt = mm_alloc_pages(1);
		memset(pt, 0x0, PAGE_SIZE);
		pd[pde] = (((size_t)pt & ~0x1ff) | flags)-virt_offset;
	} else {
		pt = (pd[pde] & ~0x1ff) + virt_offset;
	}

	pt[pte] = ((size_t)physical & ~0x1ff) | flags;
}

void vm_kmmap(void *virtual, void *physical, uint8_t flags)
{
	vm_mmap(k_page_table, virtual, physical, flags);
	vm_mmap(k_reverse_table, physical, virtual, flags);
}
