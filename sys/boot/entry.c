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

#include <log/fb.h>
#include <mm/mm.h>
#include <vm/vm.h>
#include <x86_64/gdt.h>
#include <x86_64/idt.h>
#include <x86_64/usr.h>
#include <x86_64/pit.h>
#include <x86_64/syscall.h>
#include <x86_64/acpi.h>
#include <x86_64/apic.h>
#include <x86_64/hpet.h>
#include <limine.h>

__attribute__((used, section(".limine_requests")))
static volatile LIMINE_BASE_REVISION(3);

__attribute__((used, section(".limine_requests")))
static volatile struct limine_framebuffer_request framebuffer_request = {
    .id = LIMINE_FRAMEBUFFER_REQUEST,
    .revision = 0
};

__attribute__((used, section(".limine_requests")))
static volatile struct limine_memmap_request memmap_request = {
    .id = LIMINE_MEMMAP_REQUEST,
    .revision = 0
};

__attribute__((used, section(".limine_requests")))
static volatile struct limine_hhdm_request hhdm_request = {
    .id = LIMINE_HHDM_REQUEST,
    .revision = 0
};

__attribute__((used, section(".limine_requests")))
static volatile struct limine_module_request module_request = {
    .id = LIMINE_MODULE_REQUEST,
    .revision = 0
};

__attribute__((used, section(".limine_requests_start")))
static volatile LIMINE_REQUESTS_START_MARKER;

__attribute__((used, section(".limine_requests_end")))
static volatile LIMINE_REQUESTS_END_MARKER;

extern void user_function(void)
{
	while(1);
}

void _start(void)
{
	pit_disable();
	struct limine_framebuffer *fb = framebuffer_request.response->framebuffers[0];
	fb_init(fb->width, fb->height, fb->address);
	gdt_init();
	kprintf("Global Descriptor Table initialized.\n", 0x100);
	idt_init();
	kprintf("Interrupt Descriptor Table initialized.\n", 0x100);
	mm_init(memmap_request.response->entries, memmap_request.response->entry_count, hhdm_request.response->offset);
	kprintf("Memory manager initialized.\n");
	vm_init(memmap_request.response->entries, memmap_request.response->entry_count, hhdm_request.response->offset);
	kprintf("Paging initialized.\n");
	acpi_init();
	kprintf("ACPI initialized.\n");
	apic_init();
	kprintf("APIC initialized.\n");
	hpet_init();
	kprintf("HPET initialized.\n");
	//syscall_init();
	//usr_init(module_request.response->modules, module_request.response->module_count);
	
	while(1);
}
