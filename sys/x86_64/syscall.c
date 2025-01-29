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

#include <x86_64/syscall.h>

#include <stdint.h>
#include <log/fb.h>

#define IA32_STAR   0xC0000081
#define IA32_LSTAR  0xC0000082
#define IA32_FMASK  0xC0000084

extern void syscall_entry(void);

static inline void __wrmsr(uint32_t msr, uint64_t value) {
    uint32_t low = value & 0xffffffff;
    uint32_t high = value >> 32;

    __asm__ volatile ("wrmsr" :: "c"(msr), "a"(low), "d"(high));
}

extern uint64_t syscall_handler(uint64_t arg1, uint64_t arg2, uint64_t arg3, uint64_t arg4, uint64_t arg5, uint64_t arg6)
{
	uint64_t id;
	__asm__("movq %%rax,%0" : "=r"(id));

	switch (id) {
		case 0x1:
			char *str = (char *) arg1;
			kprintf(str);
	}

	return 0;
}

void syscall_init() {
    uint64_t star_value = ((uint64_t)0x18 << 48) | ((uint64_t)0x8 << 32);
    uint64_t fmask_value = 0x200;

	__wrmsr(IA32_STAR, 0x0013000800000000);
    __wrmsr(IA32_LSTAR, (uint64_t)syscall_entry);
    __wrmsr(IA32_FMASK, 0xFFFFFFFFFFFFFFFD);
}
