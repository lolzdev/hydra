#include <stdio.h>
#include <arch/x86_64-elf/vga.h>

__attribute__ ((__noreturn__))
void kmain(void) {
	vga_initialize();

	printf("Initializing Hydra\n");
	while (1);
}
