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

__attribute__ ((__noreturn__))
void _start(void) {
	struct limine_framebuffer *framebuffer = framebuffer_request.response->framebuffers[0];
	init_fb(framebuffer->width, framebuffer->height, framebuffer->address);

	printf("Initializing hydra\n");
	init_gdt();
	printf("Global Descriptor Table initialized.\n");
	init_idt();
	printf("Interrupt Descriptor Table initialized.\n");
	//init_paging();
	//printf("Paging initialized.\n");
	//uint8_t *a = (uint8_t *)k_page_zalloc(1);
	//printf("Allocated address: 0x%x\n", (uint64_t)a);
	//*a = 2;
	while (1);
}
