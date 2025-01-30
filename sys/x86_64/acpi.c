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

#include <x86_64/acpi.h>
#include <mm/mm.h>
#include <vm/vm.h>
#include <log/fb.h>
#include <string.h>
#include <limine.h>

__attribute__((used, section(".limine_requests")))
static volatile struct limine_rsdp_request rsdp_request = {
    .id = LIMINE_RSDP_REQUEST,
    .revision = 2
};

static acpi_rsdt_t *RSDT;
static uint32_t *SDT_LIST;

void acpi_init(void)
{
	uint32_t phys = rsdp_request.response->address;
	acpi_rsdp_t *rsdp = mm_alloc(sizeof(acpi_rsdp_t));
	vm_kmmap(rsdp, (size_t)phys & ~0xfff, PAGE_PRESENT | PAGE_WRITABLE);
	vm_reload();

	phys = rsdp->rsdt_address;
	RSDT = ((size_t)vm_get_kvirt(phys) & ~0xfff) + ((size_t)phys % 0x1000);
	SDT_LIST = (uint32_t *)((size_t)RSDT + sizeof(struct acpi_sdt_header));
}

void *acpi_find_sdt(char signature[4])
{
	size_t count = (RSDT->header.length - sizeof(RSDT->header)) / 4;
	for (size_t i=0; i < count; i++) {
		struct acpi_sdt_header *header = (((size_t)vm_get_kvirt(SDT_LIST[i]) & ~0xfff) + (SDT_LIST[i] % 0x1000));

		if (strncmp(header->signature, signature, 4) == 0) {
			return (void *) header;
		}
	}

	return NULL;
}
