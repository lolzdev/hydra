#ifndef PCI_H
#define PCI_H

#include <stdint.h>
#include <stddef.h>
#include <arch/x86_64-elf/inst.h>
#include <stdio.h>

void pci_init();
void pci_init_device(uint8_t bus, uint8_t slot);

uint16_t pci_config_read(uint8_t bus, uint8_t slot, uint8_t function, uint8_t offset);
uint16_t pci_vendor(uint8_t bus, uint8_t slot);
uint16_t pci_device(uint8_t bus, uint8_t slot);
uint16_t pci_status(uint8_t bus, uint8_t slot);
uint16_t pci_command(uint8_t bus, uint8_t slot);
uint16_t pci_class(uint8_t bus, uint8_t slot);
uint16_t pci_subclass(uint8_t bus, uint8_t slot);
uint16_t pci_header(uint8_t bus, uint8_t slot);
uint64_t pci_bar5(uint8_t bus, uint8_t slot);

#endif
