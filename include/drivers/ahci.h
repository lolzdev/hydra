#ifndef AHCI_H
#define AHCI_H

#include <drivers/pci.h>

#define FIS_TYPE_REG_HD 0x27	// Register FIS - host to device
#define FIS_TYPE_REG_DH 0x34	// Register FIS - device to host
#define FIS_TYPE_DMA_ACT 0x39	// DMA activate FIS - device to host
#define FIS_TYPE_DMA_SETUP 0x41	// DMA setup FIS - bidirectional
#define FIS_TYPE_DATA 0x46	// Data FIS - bidirectional
#define FIS_TYPE_BIST 0x58 // BIST activate FIS - bidirectional
#define FIS_TYPE_PIO_SETUP 0x5f // PIO setup FIS - device to host
#define FIS_TYPE_DEV_BITS 0xa1 // Set device bits FIS - device to host

#define ATA_CMD_IDENTIFY 0xec
#define ATA_CMD_READ_DMA_EX 0x25
#define ATA_CMD_WRITE_DMA_EX 0x35

#define ATA_DEV_BUSY 0x80
#define ATA_DEV_DRQ 0x08

#define	SATA_SIG_ATA	0x00000101	// SATA drive
#define	SATA_SIG_ATAPI	0xEB140101	// SATAPI drive
#define	SATA_SIG_SEMB	0xC33C0101	// Enclosure management bridge
#define	SATA_SIG_PM	0x96690101	// Port multiplier

#define AHCI_DEV_NULL 0
#define AHCI_DEV_SATA 1
#define AHCI_DEV_SEMB 2
#define AHCI_DEV_PM 3
#define AHCI_DEV_SATAPI 4

#define HBA_PORT_IPM_ACTIVE 1
#define HBA_PORT_DET_PRESENT 3

#define HBA_PxCMD_ST 0x0001
#define HBA_PxCMD_FRE 0x0010
#define HBA_PxCMD_FR 0x4000
#define HBA_PxCMD_CR 0x8000
#define HBA_PxIS_TFES (1 << 30)

#define	AHCI_BASE 0x400000

// FIS - host to device
typedef struct {
	uint8_t type;

	uint8_t pmport:4; // port multiplier
	uint8_t rsrv0:3; // reserved
	uint8_t c:1; // command or control
	uint8_t command; // command register
	uint8_t feature_low; // feature register 7:0
	uint8_t lba0; // LBA 7:0
	uint8_t lba1; // LBA 15:8
	uint8_t lba2; // LBA 23:16
	uint8_t device; // device register
	uint8_t lba3; // LBA 31:24
	uint8_t lba4; // LBA 39:32
	uint8_t lba5; // LBA 47:40
	uint8_t feature_high; // feature register 15:8
	uint16_t count; // count register
	uint8_t icc; // isochronous command completion
	uint8_t control; // control register
	uint8_t rsrv1[4]; // reserved
} fis_reg_host_device;

// FIS - device to host
typedef struct {
	uint8_t type;

	uint8_t pmport:4; // port multiplier
	uint8_t rsrv0:2; // reserved
	uint8_t i:1; // interrupt
	uint8_t rsrv1:1; // reserved
	uint8_t status; // status register
	uint8_t error; // error register
	uint8_t lba0; // LBA 7:0
	uint8_t lba1; // LBA 15:8
	uint8_t lba2; // LBA 23:16
	uint8_t device; // device register
	uint8_t lba3; // LBA 31:24
	uint8_t lba4; // LBA 39:32
	uint8_t lba5; // LBA 47:40
	uint8_t rsrv2; // reserved
	uint8_t count_low; // count register 7:0
	uint8_t count_high; // count register 15:8
	uint8_t rsrv3[6]; // reserved
} fis_reg_device_host;

// FIS - data payload
typedef struct {
	uint8_t type;

	uint8_t pmport:4; // port multiplier
	uint8_t rsrv0:4; // reserved
	uint8_t rsrv1[2]; // reserved
	uint32_t data[1];
} fis_data;

// FIS - PIO setup
typedef struct {
	uint8_t type;

	uint8_t pmport:4; // port multiplier
	uint8_t rsrv0:2; // reserved
	uint8_t d:1; // transfer direction
	uint8_t i:1; // interrupt
	uint8_t rsrv1:1; // reserved
	uint8_t status; // status register
	uint8_t error; // error register
	uint8_t lba0; // LBA 7:0
	uint8_t lba1; // LBA 15:8
	uint8_t lba2; // LBA 23:16
	uint8_t device; // device register
	uint8_t lba3; // LBA 31:24
	uint8_t lba4; // LBA 39:32
	uint8_t lba5; // LBA 47:40
	uint8_t rsrv2; // reserved
	uint8_t count_low; // count register 7:0
	uint8_t count_high; // count register 15:8
	uint8_t rsrv3;
	uint8_t new_status; // new value of status register
	uint16_t transfer_count; // number of bytes to transfer
	uint8_t rsrv4[2]; // reserved
} fis_pio_setup;

// FIS - DMA setup
typedef struct {
	uint8_t type;

	uint8_t pmport:4; // port multiplier
	uint8_t rsv0:1; // reserved
	uint8_t d:1; // transfer direction
	uint8_t i:1; // interrupt
	uint8_t a:1; // auto activate
	uint8_t rsrv0[2]; // reserved
	uint64_t buffer_id; // DMA buffer identifier
	uint32_t rsrv1;
	uint32_t buffer_offset;
	uint32_t transfer_count; // number of bytes to transfer
	uint32_t rsrv2;
} fis_dma_setup;

// HBA port
typedef volatile struct {
	uint64_t clb; // command list base address
	uint64_t fb; // FIS base address
	uint32_t is; // interrupt status
	uint32_t ie; // interrupt enable
	uint32_t cmd; // command and status
	uint32_t rsrv0; // reserved
	uint32_t tfd; // task file data
	uint32_t sig; // signature
	uint32_t ssts; // sata status
	uint32_t sctl; // sata control
	uint32_t serror; // sata error
	uint32_t sact; // sata active
	uint32_t ci; // command issue
	uint32_t sntf; // sata notification
	uint32_t fbs; // fis-based switch control
	uint32_t rsrv1[11]; // reserved
	uint32_t vendor[4]; // vendor specific
} hba_port;

// HBA memory
typedef volatile struct {
	uint32_t cap; // capability
	uint32_t ghc; // global host control
	uint32_t is; // interrupt status
	uint32_t pi; // port implemented
	uint32_t vs; // version
	uint32_t ccc_ctl; // command completion coalescing control
	uint32_t ccc_pts; // command completion coalescing ports
	uint32_t em_loc; // enclosure management location
	uint32_t em_ctl; // enclosure management control
	uint32_t cape; // capabilities extended
	uint32_t bohc; // bios/os handoff control and status
	uint8_t rsrv[0xa0-0x2c];
	uint8_t vendor[0x100-0xa0];
	hba_port ports[32];
} hba_memory;

// HBA received FIS
typedef volatile struct {
	fis_dma_setup dma_setup;
	uint8_t pad0[4];
	fis_pio_setup pio_setup;
	uint8_t pad1[12];
	fis_reg_device_host device_host;
	uint8_t pad2[4];
	uint8_t sdbf[0x60-0x58];
	uint8_t unknown[64];
	uint8_t rsrv[0x100-0xA0];
} hba_fis;

// HBA command header
typedef struct {
	uint8_t cfl:5; // command fis length
	uint8_t a:1; // ATAPI
	uint8_t w:1; // write
	uint8_t p:1; // prefetchable
	uint8_t r:1; // reset
	uint8_t b:1; // BIST
	uint8_t c:1; // clear busy
	uint8_t rsrv0:1; // reserved
	uint8_t pmp:4; // port multiplier
	uint16_t prdtl; // physical region descriptor table length in entries
	volatile uint32_t prdcb; // physical region descriptor byte count transferred
	uint64_t ctba; // command table descriptor base address
	uint32_t rsrv1[4]; // reserved
} hba_cmd_header;

typedef struct {
	uint64_t dba; // data base address
	uint32_t rsrv0; // reserved
	uint32_t dbc:22; // byte count
	uint32_t rsrv1:9; // reserved
	uint32_t i:1;
} hba_prdt_entry;

// HBA command table
typedef struct {
	uint8_t command_fis[64];
	uint8_t acmd[16]; // ATAPI command
	uint8_t rsrv[48]; // reserved
	hba_prdt_entry prdt_entry[1];
} hba_cmd_tbl;

void ahci_init_device(hba_memory *abar, void *command_list);
void ahci_probe_port(hba_memory *abar, void *command_list);
void ahci_port_rebase(hba_port *port, int index, void *command_list);
void ahci_stop_cmd(hba_port *port);
void ahci_start_cmd(hba_port *port);
int ahci_read(hba_port *port, uint64_t start, uint32_t count, uint16_t *buf);
int ahci_find_cmd_slot(hba_port *port);

#endif
