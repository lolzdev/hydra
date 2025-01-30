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

#include <x86_64/pit.h>
#include <x86_64/inst.h>
#include <log/fb.h>

static uint64_t sleep_ticks;

__attribute__((interrupt))
void int_pit(struct interrupt_frame *frame)
{
	sleep_ticks--;
	__outb(0x20, 0x20);
}

uint32_t pit_get(void) {
	uint32_t count = 0;
	
	__cli();
	
	__outb(0x43,0b0000000);
	
	count = __inb(0x40);
	count |= __inb(0x40) << 8;
	
	return count;
}

void pit_set(uint32_t count)
{
	__cli();

	__outb(0x40,count & 0xff);
	__outb(0x40,(count & 0xff00) >> 8);
}

void pit_set_frequency(uint64_t ms) {
    uint16_t divisor = PIT_FREQUENCY / (ms * 1000);

    __outb(PIT_COMMAND_PORT, 0x36);
    __outb(PIT_CHANNEL_0, divisor & 0xFF);
    __outb(PIT_CHANNEL_0, (divisor >> 8) & 0xFF);
}

void pit_disable(void) {
    uint8_t mask = __inb(0x21);
    mask |= 0x1; // disable PIT
    __outb(0x21, mask);
}

void pit_enable(void) {
    uint8_t mask = __inb(0x21);
    mask &= ~0x1; // disable PIT
    __outb(0x21, mask);
}

void pit_sleep(uint64_t ms)
{
	pit_enable();
	__cli();
	sleep_ticks = ms;
	pit_set_frequency(1);
	__sti();

	while (sleep_ticks > 0) {
		__asm__ volatile("hlt");
	}
	pit_disable();
}
