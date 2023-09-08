#include <drivers/pci.h>
#include <drivers/ahci.h>
#include <arch/x86_64-elf/vm.h>
#include <arch/x86_64-elf/mm.h>

static const char *pci_classes[20] = {
	"Unclassified",
	"Mass Storage Controller",
	"Network Controller",
	"Display Controller",
	"Multimedia Controller",
	"Memory Controller",
	"Bridge",
	"Simple Communication Controller",
	"Base System Peripheral",
	"Input Device Controller",
	"Docking Station",
	"Processor",
	"Serial Bus Controller",
	"Wireless Controller",
	"Intelligent Controller",
	"Satellite Communication Controller",
	"Encryption Controller",
	"Signal Processing Controller",
	"Processing Accelerator",
	"Non Essential Instrumentation"
};

uint16_t pci_config_read(uint8_t bus, uint8_t slot, uint8_t function, uint8_t offset)
{
	uint32_t addr = ((uint32_t)bus << 16) | ((uint32_t)slot << 11) | ((uint32_t)function << 8) | ((uint32_t)(offset & ~0x3)) | (0x1 << 31);

	outl(0xcf8, addr);
	return (uint16_t)((inl(0xcfc) >> (((uint32_t)offset & 2) * 8)) & 0xffff);
}

inline uint16_t pci_vendor(uint8_t bus, uint8_t slot)
{
	return pci_config_read(bus, slot, 0, 0x0);
}

inline uint16_t pci_device(uint8_t bus, uint8_t slot)
{
	return pci_config_read(bus, slot, 0, 0x2);
}

inline uint16_t pci_status(uint8_t bus, uint8_t slot)
{
	return pci_config_read(bus, slot, 0, 0x6);
}

inline uint16_t pci_command(uint8_t bus, uint8_t slot)
{
	return pci_config_read(bus, slot, 0, 0x4);
}

inline uint16_t pci_class(uint8_t bus, uint8_t slot)
{
	return (pci_config_read(bus, slot, 0, 0xA) >> 8) & 0xff;
}

inline uint16_t pci_subclass(uint8_t bus, uint8_t slot)
{
	return pci_config_read(bus, slot, 0, 0xA) & 0xff;
}

inline uint16_t pci_header(uint8_t bus, uint8_t slot)
{
	return pci_config_read(bus, slot, 0, 0xc) & 0xff;
}

inline uint64_t pci_bar5(uint8_t bus, uint8_t slot)
{
	return ((uint32_t)pci_config_read(bus, slot, 0, 0x24) | 
	((uint32_t)pci_config_read(bus, slot, 0, 0x26) << 16));
}

void pci_init_device(uint8_t bus, uint8_t slot)
{
	uint16_t vendor = pci_vendor(bus, slot);
	uint16_t device = pci_device(bus, slot);
	uint16_t class = pci_class(bus, slot);
	uint16_t subclass = pci_subclass(bus, slot);
	uint64_t bar5 = (uint64_t)pci_bar5(bus, slot);

	if (vendor == 0xffff)
		return; // no device
	printf("%s device vendor: 0x%x, device: 0x%x\n", pci_classes[class], vendor, device);

	if (class == 0x1) {
		if (subclass == 0x6) {
			for (size_t i=0; i < 2; i++) {
				kernel_map((void *)bar5+(i * 0x1000), (void *)bar5+(i * 0x1000), 0x3);
			}
			uint64_t command_list = (uint64_t)k_page_zalloc(11);	
			for (size_t i=0; i < 11; i++) {
				kernel_map((void *)command_list+(i * 0x1000), (void *)command_list+(i * 0x1000), 0x3);
			}
			ahci_init_device((hba_memory *)bar5, (void *)command_list);
		}
	}
}

void pci_init()
{
	for (uint16_t bus=0; bus < 256; bus++) {
		for (uint8_t device=0; device < 32; device++) {
			pci_init_device(bus, device);
		}
	}
}
