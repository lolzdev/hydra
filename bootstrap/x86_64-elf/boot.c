#include <stdio.h>
#include <arch/x86_64-elf/vga.h>
#include <arch/x86_64-elf/gdt.h>
#include <arch/x86_64-elf/idt.h>
#include <arch/x86_64-elf/vm.h>

__attribute__ ((__noreturn__))
void kmain(void) {
	vga_initialize();

	printf("Initializing hydra\n");
	init_gdt();
	printf("Global Descriptor Table initialized.\n");
	init_idt();
	printf("Interrupt Descriptor Table initialized.\n");
	init_paging();
	printf("Paging initialized.\n");
	while (1);
}
