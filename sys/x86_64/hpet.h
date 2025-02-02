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
#include <x86_64/trap.h>

#define HPET_ENABLE_CNF_MASK 0x1
#define HPET_TMR_INT_STS_MASK(tmr) (0x1 << (tmr))
#define HPET_TMR_INT_STS_CLR(reg, tmr) (*(reg) &= (HPET_TMR_INT_STS_MASK(tmr)))

#define HPET_COUNTER_CLK_PERIOD(reg) (((*reg) >> 32) & 0xffffffff)
#define HPET_VENDOR_ID(reg) (((*reg) >> 16) & 0xffff)
#define HPET_LEG_RT_CAP(reg) (((*reg) >> 15) & 0x1)
#define HPET_COUNT_SIZE_CAP(reg) (((*reg) >> 13) & 0x1)
#define HPET_NUM_TIM_CAP(reg) (((*reg) >> 8) & 0x1f)
#define HPET_REV_ID(reg) ((*reg) & 0xff)

#define HPET_RT_CNF(reg) (((*reg) >> 1) & 0x1)
#define HPET_ENABLE_CNF(reg) ((*reg) & 0x1)
#define HPET_RT_CNF_SET(reg) (*(reg) |= 0x2)
#define HPET_RT_CNF_CLR(reg) (*(reg) &= ~0x2)
#define HPET_ENABLE_CNF_SET(reg) (*(reg) |= 0x1)
#define HPET_ENABLE_CNF_CLR(reg) (*(reg) &= ~0x1)

#define HPET_TMR_ROUTE_CAP(reg) (((*reg) >> 32) & 0xffffffff)
#define HPET_TMR_FSB_INT_DEL_CAP(reg) (((*reg) >> 15) & 0x1)
#define HPET_TMR_FSB_EN_CNF(reg) (((*reg) >> 14) & 0x1)
#define HPET_TMR_FSB_EN_CNF_SET(reg) (*(reg) |= (1 << 14))
#define HPET_TMR_FSB_EN_CNF_CLR(reg) (*(reg) &= ~(1 << 14))
#define HPET_TMR_INT_ROUTE_CNF(reg) (((*reg) >> 9) & 0x1f)
#define HPET_TMR_INT_ROUTE_CNF_SET(reg, value) (*(reg) &= (~(0x1f << 9) | (value & 0x1f) << 9))
#define HPET_TMR_32MODE_CNF(reg) (((*reg) >> 8) & 0x1)
#define HPET_TMR_32MODE_CNF_SET(reg) (*(reg) |= (1 << 8))
#define HPET_TMR_32MODE_CNF_CLR(reg) (*(reg) &= ~(1 << 8))
#define HPET_TMR_VAL_SET_CNF(reg) (((*reg) >> 6) & 0x1)
#define HPET_TMR_VAL_SET_CNF_SET(reg) (*(reg) |= (1 << 6))
#define HPET_TMR_VAL_SET_CNF_CLR(reg) (*(reg) &= ~(1 << 6))
#define HPET_TMR_SIZE_CAP(reg) (((*reg) >> 5) & 0x1)
#define HPET_TMR_PER_INT_CAP(reg) (((*reg) >> 4) & 0x1)
#define HPET_TMR_TYPE_CNF(reg) (((*reg) >> 3) & 0x1)
#define HPET_TMR_TYPE_CNF_SET(reg) (*(reg) |= (1 << 3))
#define HPET_TMR_TYPE_CNF_CLR(reg) (*(reg) &= ~(1 << 3))
#define HPET_TMR_INT_EN_CNF(reg) (((*reg) >> 2) & 0x1)
#define HPET_TMR_INT_EN_CNF_SET(reg) (*(reg) |= (1 << 2))
#define HPET_TMR_INT_EN_CNF_CLR(reg) (*(reg) &= ~(1 << 2))
#define HPET_TMR_INT_TYPE_CNF(reg) (((*reg) >> 1) & 0x1)
#define HPET_TMR_INT_TYPE_CNF_SET(reg) (*(reg) |= (1 << 1))
#define HPET_TMR_INT_TYPE_CNF_CLR(reg) (*(reg) &= ~(1 << 1))

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
	uint8_t legacy_replacement:1;
	uint16_t pci_vendor_id;
	struct hpet_address address;
	uint8_t hpet_number;
	uint16_t minimum_tick;
	uint8_t page_protection;
} __attribute__((packed)) hpet_table_t;

void hpet_init(void);
uint64_t *hpet_get_tmr_conf_cap(uint8_t tmr);
uint64_t hpet_get_tmr_value(uint8_t tmr);
void hpet_set_tmr_value(uint8_t tmr, uint64_t value);
void hpet_disable_pit(void);

#endif
