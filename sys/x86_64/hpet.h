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

#ifndef HPET_H
#define HPET_H

#include <x86_64/acpi.h>

struct hpet_address {
	uint8_t address_space_id;
    uint8_t register_bit_width;
    uint8_t register_bit_offset;
    uint8_t reserved;
    uint64_t address;
} __attribute__((packed));

typedef struct hpet_table {
	struct acpi_sdt_header header;
	uint8_t hw_rev_id;
	uint8_t comparator_count:5;
	uint8_t counter_size:1;
	uint8_t reserved:1;
	uint6_t legacy_replacement:1;
	uint16_t pci_vendor_id;
	struct hpet_address address;
	uint8_t hpet_number;
	uint16_t minimum_tick;
	uint8_t page_protection;
} __attribute__((packed)) hpet_table_t;

struct hpet_general_caps {
	uint32_t counter_clk_period;
	uint16_t vendor_id;
	uint8_t legacy_cap:1;
	uint8_t reserved:1;
	uint8_t count_size_cap:1;
	uint8_t timer_count:5;
	uint8_t rev_id;
} __attribute__((packed));

struct hpet_general_config {
	uint64_t reserved:62;
	uint8_t legacy_enable:1;
	uint8_t enable:1;
} __attribute__((packed));

struct hpet_general_int_status {
	uint32_t reserved;
	uint32_t status;
} __attribute__((packed));

struct hpet_tmr_conf_cap {
	uint32_t int_route_cap;
	uint16_t reserved;
	uint8_t fsb_cap:1;
	uint8_t fsb_enable:1;
	uint8_t int_route:5;
	uint8_t mode32_bit:1;
	uint8_t reserved1:1;
	uint8_t val_set:1;
	uint8_t size_cap:1;
	uint8_t periodic_cap:1;
	uint8_t type:1;
	uint8_t int_enable:1;
	uint8_t int_type:1;
	uint8_t reserved2:1;
} __attribute__((packed));

#endif
