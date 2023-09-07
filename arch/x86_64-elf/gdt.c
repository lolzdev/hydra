#include <arch/x86_64-elf/gdt.h>

static gdt_t GDT;

void encode_gdt_entry(uint64_t *entry, uint64_t limit, uint64_t base, uint64_t access, uint64_t flags)
{
	*entry = (limit & 0xffff) | ((base & 0xffff) << 16) | ((base & 0xff0000) << 32) | ((access & 0xff) << 40) | ((limit & 0xf0000) << 48) | ((flags & 0xf) << 52) | ((base & 0xff000000) << 56);
}

void init_gdt()
{
	GDT[0] = 0x0;
	encode_gdt_entry(&GDT[1], 0xfffff, 0x0, 0x9a, 0x0);
	encode_gdt_entry(&GDT[2], 0xfffff, 0x0, 0x92, 0x0);

	encode_gdt_entry(&GDT[3], 0xfffff, 0x0, 0x9a, 0x4);
	encode_gdt_entry(&GDT[4], 0xfffff, 0x0, 0x92, 0x4);
	// kernel code
	encode_gdt_entry(&GDT[5], 0xfffff, 0x0, 0x9a, 0xa);
	// kernel data
	encode_gdt_entry(&GDT[6], 0xfffff, 0x0, 0x92, 0xc);

	load_gdt((sizeof(uint64_t) * 7) - 1, (uint64_t) GDT);
}
