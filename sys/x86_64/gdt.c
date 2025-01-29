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

#include <x86_64/gdt.h>
#include <x86_64/tss.h>

static gdt_t GDT;
static tss_t TSS;


void gdt_encode_entry(uint64_t *entry, uint64_t limit, uint64_t base, uint64_t access, uint64_t flags)
{
	*entry = (limit & 0xffff) | ((base & 0xffff) << 16) | ((base & 0xff0000) << 32) | ((access & 0xff) << 40) | ((limit & 0xf0000) << 48) | ((flags & 0xf) << 52) | ((base & 0xff000000) << 56);
}

void gdt_init(void)
{
	GDT[0] = 0x0;
	// kernel code
	gdt_encode_entry(&GDT[1], 0xfffff, 0x0, 0x9a, 0xa);
	// kernel data
	gdt_encode_entry(&GDT[2], 0xfffff, 0x0, 0x92, 0xc);
	// user data
	gdt_encode_entry(&GDT[3], 0xfffff, 0x0, 0xf2, 0xc);
	// user code
	gdt_encode_entry(&GDT[4], 0xfffff, 0x0, 0xfa, 0xa);
	// tss
	//gdt_encode_entry(&GDT[5], sizeof(tss_t)-1, (uint64_t)(&TSS) &0x00000000ffffffff, 0x89, 0x0);
	//GDT[6] = (uint64_t)(&TSS) & 0xffffffff00000000;

	gdt_load((sizeof(uint64_t) * 3) - 1, (uint64_t) GDT);
	//TSS.rsp0 = 0x10;
}
