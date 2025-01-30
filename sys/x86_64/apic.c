	/*-
	 * Copyright 2025 Lorenzo Torres
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

#include <x86_64/apic.h>
#include <x86_64/inst.h>
#include <x86_64/pit.h>
#include <mm/mm.h>
#include <vm/vm.h>
#include <log/fb.h>

static uint32_t *LAPIC_BASE;
static uint32_t *IOAPIC;

void apic_io_write32(uint32_t reg, uint32_t value)
{
	if (!IOAPIC) return;

	*((volatile uint32_t *) IOAPIC) = reg;
	*((volatile uint32_t *) (size_t)IOAPIC + 0x10) = value;
}

void apic_io_write(uint32_t reg, uint64_t value)
{
	if (!IOAPIC) return;

	uint32_t high = (value >> 32) & 0x00000000ffffffff;
	uint32_t low = value & 0x00000000ffffffff;

	*((volatile uint32_t *) IOAPIC) = reg;
	*((volatile uint32_t *) (size_t)IOAPIC + 0x10) = low;
	*((volatile uint32_t *) IOAPIC) = reg + 1;
	*((volatile uint32_t *) (size_t)IOAPIC + 0x10) = high;
}

uint32_t apic_io_read32(uint32_t reg)
{
	if (!IOAPIC) return 0;

	*((volatile uint32_t *) IOAPIC) = reg;
	return *((volatile uint32_t *) ((size_t)IOAPIC + 0x10));
}

uint64_t apic_io_read(uint32_t reg)
{
	uint32_t low = apic_io_read32(reg);
	uint32_t high = apic_io_read32(reg+1);
	
	return (uint64_t)low | ((uint64_t)high << 32);
}

void apic_io_map(uint8_t irq, uint8_t vector)
{
	*((volatile uint32_t *) (size_t)IOAPIC + (0x10 + irq * 2)) = (uint32_t) vector & 0xff;
}

void apic_init(void)
{
	apic_parse_madt();

	apic_io_map(1, 39);
}

void apic_parse_madt(void)
{
	madt_table_t *madt = acpi_find_sdt("APIC");
	//LAPIC_BASE = madt->lapic_address;

	struct apic_entry_header *hdr = &madt->entries;
	size_t table_end = (size_t)madt + madt->header.length;

	while (hdr < table_end) {
		apic_entry_local_t *entry_local;
		apic_entry_io_t *entry_io;
		apic_entry_io_iso_t *entry_io_iso;
		apic_entry_nmi_source_t *entry_nmi_source;
		apic_entry_nmi_t *entry_nmi;
		apic_entry_addr_override_t *entry_addr_override;
		apic_entry_x2apic_t *entry_x2apic;
		switch (hdr->entry_type) {
			case 0x0:
				goto lbl_entry_local;
				break;
			case 0x1:
				goto lbl_entry_io;
				break;
			case 0x2:
				goto lbl_entry_io_iso;
				break;
			case 0x3:
				goto lbl_entry_nmi_source;
				break;
			case 0x4:
				goto lbl_entry_nmi;
				break;
			case 0x5:
				goto lbl_entry_addr_override;
				break;
			case 0x9:
				goto lbl_entry_x2apic;
				break;
		}

lbl_entry_local:
		entry_local = ((size_t)hdr+sizeof(apic_entry_header_t));
		kprintf("cpu: %x\n", entry_local->cpu_id);
		goto lbl_entry_done;
lbl_entry_io:
		entry_io = ((size_t)hdr+sizeof(apic_entry_header_t));
		IOAPIC = mm_alloc_pages(1);
		vm_kmmap(IOAPIC, (uint64_t)entry_io->address & 0x00000000ffffffff, PAGE_PRESENT | PAGE_WRITABLE);
		vm_reload();
		goto lbl_entry_done;
lbl_entry_io_iso:
		entry_io_iso = ((size_t)hdr+sizeof(apic_entry_header_t));
		kprintf("bus_source: %x\n", entry_io_iso->bus_source);
		goto lbl_entry_done;
lbl_entry_nmi_source:
		entry_nmi_source = ((size_t)hdr+sizeof(apic_entry_header_t));
		kprintf("nmi_source: %x\n", entry_nmi_source->nmi_source);
		goto lbl_entry_done;
lbl_entry_nmi:
		entry_nmi = ((size_t)hdr+sizeof(apic_entry_header_t));
		kprintf("nmi cpu id: %x\n", entry_nmi->cpu_id);
		goto lbl_entry_done;
lbl_entry_addr_override:
		entry_addr_override = ((size_t)hdr+sizeof(apic_entry_header_t));
		kprintf("override: %x\n", entry_addr_override->address);
		goto lbl_entry_done;
lbl_entry_x2apic:
		entry_x2apic = ((size_t)hdr+sizeof(apic_entry_header_t));
		kprintf("x2apic id: %x\n", entry_x2apic->id);
		goto lbl_entry_done;
lbl_entry_done:
		hdr += hdr->record_length/2;
	}
}
