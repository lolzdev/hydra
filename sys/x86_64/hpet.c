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

#include <x86_64/hpet.h>
#include <x86_64/apic.h>
#include <vm/vm.h>
#include <mm/mm.h>
#include <log/fb.h>
#include <x86_64/inst.h>

static uint64_t HPET;
static uint64_t HPET_FREQ;
static uint16_t HPET_MIN_TICK;

__attribute__((interrupt))
void hpet_int(struct interrupt_frame *frame)
{
	kprintf("timer1\n");
	__outb(0x20, 0x20);
}

static inline uint64_t HPET_SECONDS(uint64_t seconds)
{
	return HPET_FREQ * seconds;
}

uint64_t *hpet_get_tmr_conf_cap(uint8_t tmr)
{
	return (uint64_t *) (HPET + (0x100 + 0x20 * tmr));
}

uint64_t hpet_get_tmr_value(uint8_t tmr)
{
	return *((uint64_t *) (HPET + (0x108 + 0x20 * tmr)));
}

void hpet_set_tmr_value(uint8_t tmr, uint64_t value)
{
	*((volatile uint64_t *) (HPET + (0x108 + 0x20 * tmr))) = value;
}

void hpet_init_tmr(uint8_t tmr, uint64_t time)
{
	volatile uint64_t *main_counter_value = HPET + 0xf0;
	uint64_t *tmr0 = hpet_get_tmr_conf_cap(tmr);
	HPET_TMR_TYPE_CNF_SET(tmr0);
	HPET_TMR_VAL_SET_CNF_SET(tmr0);
	HPET_TMR_INT_EN_CNF_SET(tmr0);
	hpet_set_tmr_value(tmr, *main_counter_value + time);
}

void hpet_init(void)
{
	hpet_table_t *hpet_table = acpi_find_sdt("HPET");
	HPET = mm_alloc(sizeof(uint64_t) * 4 + (0x30 * 32));
	vm_kmmap(HPET, hpet_table->address.address, PAGE_PRESENT | PAGE_WRITABLE | PAGE_NO_CACHE);
	vm_reload();

	volatile uint64_t *general_caps = HPET;
	volatile uint64_t *general_config = HPET + 0x10;
	HPET_FREQ = 0x38d7ea4c68000 / (HPET_COUNTER_CLK_PERIOD(general_caps));
	HPET_MIN_TICK = hpet_table->minimum_tick;
	HPET_ENABLE_CNF_SET(general_config);

	hpet_init_tmr(0, HPET_SECONDS(2));
}

void hpet_disable_pit(void) {
    uint8_t mask = __inb(0x21);
    mask |= 0x1;
    __outb(0x21, mask);
	__outb(0x43, 0x30);
	__outb(0x40, 0x0);
	__outb(0x40, 0x0);
}
