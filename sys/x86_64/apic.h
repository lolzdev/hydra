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

#ifndef APIC_H
#define APIC_H

#include <stdint.h>
#include <stddef.h>
#include <x86_64/acpi.h>

#define IA32_APIC_BASE_MSR 0x1B
#define IA32_APIC_BASE_MSR_BSP 0x100
#define IA32_APIC_BASE_MSR_ENABLE 0x800

#define APIC_LAPIC_ID 0x20
#define APIC_LAPIC_VERSION 0x30
#define APIC_TPR 0x80
#define APIC_APR 0x90
#define APIC_PPR 0xa0
#define APIC_EOI 0xb0
#define APIC_RRD 0xc0
#define APIC_LD  0xd0
#define APIC_DF  0xe0
#define APIC_SIV 0xf0
#define APIC_IC  0x380
#define APIC_CC  0x390
#define APIC_TD  0x3E0

#define APIC_LVT_TMR 0x320

typedef struct madt_table {
	struct acpi_sdt_header header;
	uint32_t lapic_address;
	uint32_t flags;
	uint8_t entries;
} __attribute__((packed)) madt_table_t;

typedef struct apic_entry_header {
	uint8_t entry_type;
	uint8_t record_length;
} __attribute__((packed)) apic_entry_header_t;

typedef struct apic_entry_local {
	uint8_t cpu_id;
	uint8_t id;
	uint32_t flags;
} __attribute__((packed)) apic_entry_local_t;

typedef struct apic_entry_io {
	uint8_t id;
	uint8_t reserved;
	uint32_t address;
	uint32_t gsi_base;
} __attribute__((packed)) apic_entry_io_t;

typedef struct apic_entry_io_iso {
	uint8_t bus_source;
	uint8_t irq_source;
	uint32_t gsi;
	uint16_t flags;
} __attribute__((packed)) apic_entry_io_iso_t;

typedef struct apic_entry_nmi {
	uint8_t cpu_id;
	uint16_t flags;
	uint8_t lint;
} __attribute__((packed)) apic_entry_nmi_t;

typedef struct apic_entry_nmi_source {
	uint8_t nmi_source;
	uint8_t reserved;
	uint16_t flags;
	uint32_t gsi;
} __attribute__((packed)) apic_entry_nmi_source_t;

typedef struct apic_entry_addr_override {
	uint16_t reserved;
	uint64_t address;
} __attribute__((packed)) apic_entry_addr_override_t;

typedef struct apic_entry_x2apic {
	uint16_t reserved;
	uint32_t id;
	uint32_t flags;
	uint32_t acpi_id;
} __attribute__((packed)) apic_entry_x2apic_t;

/* Write to a 32 bits long IOAPIC register */
void apic_io_write32(uint32_t reg, uint32_t value);
/* Write to a 64 bits long IOAPIC register */
void apic_io_write(uint32_t reg, uint64_t value);
/* Read from a 32 bits long IOAPIC register */
uint32_t apic_io_read32(uint32_t reg);
/* Read from a 32 bits long IOAPIC register */
uint64_t apic_io_read(uint32_t reg);

void apic_local_write(uint32_t reg, uint32_t value);
uint32_t apic_local_read(uint32_t reg);
void apic_init(void);
void apic_parse_madt(void);
void apic_io_map(uint8_t irq, uint8_t vector);

#endif
