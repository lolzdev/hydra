#include <stdio.h>
#include <arch/x86_64-elf/vga.h>
#include <arch/x86_64-elf/gdt.h>
#include <arch/x86_64-elf/idt.h>
#include <arch/x86_64-elf/vm.h>
#include <arch/x86_64-elf/mm.h>
#include <arch/x86_64-elf/fb.h>
#include <arch/x86_64-elf/xbm.h>
#include <limine.h>

static volatile struct limine_framebuffer_request framebuffer_request = {
    .id = LIMINE_FRAMEBUFFER_REQUEST,
    .revision = 0
};

static volatile struct limine_memmap_request memmap_request = {
	.id = LIMINE_MEMMAP_REQUEST,
	.revision = 0,
	.response = NULL
};

__attribute__ ((__noreturn__))
void _start(void) {
	struct limine_framebuffer *framebuffer = framebuffer_request.response->framebuffers[0];
	init_fb(framebuffer->width, framebuffer->height, framebuffer->address);
	
	printf("0x%x\n", framebuffer->address);
	printf("Initializing hydra\n");
	uint64_t mem_size = init_mm(memmap_request.response);
	printf("Memory allocator initialized with %d bytes.\n", mem_size);
	init_gdt();
	printf("Global Descriptor Table initialized.\n");
	init_idt();
	printf("Interrupt Descriptor Table initialized.\n");
	init_paging(memmap_request.response);
	printf("Paging initialized.\n");
	while (1);
}
