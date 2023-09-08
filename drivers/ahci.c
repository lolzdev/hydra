#include <drivers/ahci.h>

static int check_type(hba_port *port)
{
	uint32_t ssts = port->ssts;

	uint8_t ipm = (ssts >> 8) & 0x0F;
	uint8_t det = ssts & 0x0F;

	if (det != HBA_PORT_DET_PRESENT)
		return AHCI_DEV_NULL;
	if (ipm != HBA_PORT_IPM_ACTIVE)
		return AHCI_DEV_NULL;

	switch (port->sig) {
		case SATA_SIG_ATAPI:
			return AHCI_DEV_SATAPI;
		case SATA_SIG_SEMB:
			return AHCI_DEV_SEMB;
		case SATA_SIG_PM:
			return AHCI_DEV_PM;
		default:
			return AHCI_DEV_SATA;
	}
}

void ahci_init_device(hba_memory *abar, void *command_list)
{
	ahci_probe_port(abar, command_list);
}

void ahci_probe_port(hba_memory *abar, void *command_list)
{
	for (int i=0; i < 32; i++) {
		if (abar->pi & 1) {
			switch (check_type(&abar->ports[i])) {
				case AHCI_DEV_SATA:
					printf("Found SATA drive with index %d\n", i);
					ahci_port_rebase(&abar->ports[i], i, command_list);
					printf("SATA drive configured.\n");
					break;
				case AHCI_DEV_SATAPI:
					printf("Found SATAPI drive at: %d\n", i);
					break;
				case AHCI_DEV_SEMB:
					printf("Found SEMB drive at: %d\n", i);
					break;
				case AHCI_DEV_PM:
					printf("Found PM drive at: %d\n", i);
					break;
				default:
					break;
			}
		}
	}
}

void ahci_port_rebase(hba_port *port, int index, void *command_list)
{
	ahci_stop_cmd(port);

	port->clb = (uint64_t)command_list + (index << 10);
	memset((uint8_t *)port->clb, 0, 1024);

	port->fb = (uint64_t)command_list + (32 << 10) + (index << 8);
	memset((uint8_t *)port->fb, 0, 256);

	hba_cmd_header *cmd_header = (hba_cmd_header *)port->clb;
	
	for (size_t i = 0; i < 32; i++) {
		cmd_header[i].prdtl = 8;
		cmd_header[i].ctba = (uint64_t)command_list + (40 << 10) + (index << 13) + (i << 8);
		memset((uint8_t *)cmd_header[i].ctba, 0, 256);
	}

	ahci_start_cmd(port);
}

void ahci_stop_cmd(hba_port *port)
{
	port->cmd &= ~HBA_PxCMD_ST;
	port->cmd &= ~HBA_PxCMD_FRE;

	while (port->cmd & HBA_PxCMD_FR || port->cmd & HBA_PxCMD_CR);
}

void ahci_start_cmd(hba_port *port)
{
	while (port->cmd & HBA_PxCMD_CR);

	port->cmd |= HBA_PxCMD_FRE;
	port->cmd |= HBA_PxCMD_ST;
}

int ahci_read(hba_port *port, uint64_t start, uint32_t count, uint16_t *buf)
{
	uint32_t startl = (uint32_t)(start & 0xffffffff);
	uint32_t starth = (uint32_t)((start >> 32) & 0xffffffff);
	port->is = -1;
	int spin = 0;
	int slot = ahci_find_cmd_slot(port);
	if (slot == -1)
		return 0;

	hba_cmd_header *cmd_header = (hba_cmd_header *)port->clb;
	cmd_header += slot;
	cmd_header->cfl = sizeof(fis_reg_host_device) / sizeof(uint32_t);
	cmd_header->w = 0;
	cmd_header->prdtl = (uint16_t)((count-1) >> 4) + 1;
	
	hba_cmd_tbl *cmd_tbl = (hba_cmd_tbl *)cmd_header->ctba;
	memset((uint8_t *)cmd_tbl, 0, sizeof(hba_cmd_tbl) + (cmd_header->prdtl-1) * sizeof(hba_prdt_entry));

	int i = 0;
	for (; i < cmd_header->prdtl-1; i++) {
		cmd_tbl->prdt_entry[i].dba = (uint64_t) buf;
		cmd_tbl->prdt_entry[i].dbc = 8*1024-1;
		cmd_tbl->prdt_entry[i].i = 1;
		buf += 4 * 1024;
		count -= 16;
	}

	cmd_tbl->prdt_entry[i].dba = (uint64_t) buf;
	cmd_tbl->prdt_entry[i].dbc = (count << 9) -1;
	cmd_tbl->prdt_entry[i].i = 1;

	fis_reg_host_device *cmd_fis = (fis_reg_host_device *)&cmd_tbl->command_fis;
	
	cmd_fis->type = FIS_TYPE_REG_HD;
	cmd_fis->c = 1;
	cmd_fis->command = ATA_CMD_READ_DMA_EX;

	cmd_fis->lba0 = (uint8_t)startl;
	cmd_fis->lba1 = (uint8_t)(startl>>8);
	cmd_fis->lba2 = (uint8_t)(startl>>16);
	cmd_fis->device = 1<<6;

	cmd_fis->lba3 = (uint8_t)(startl>>24);
	cmd_fis->lba4 = (uint8_t)starth;
	cmd_fis->lba5 = (uint8_t)(starth>>8);
 
	cmd_fis->count = count;

	while ((port->tfd & (ATA_DEV_BUSY | ATA_DEV_DRQ)) && spin < 1000000)
	{
		spin++;
	}

	if (spin == 1000000)
	{
		printf("SATA port is hung\n");
		return 0;
	}

	port->ci = 1<<slot;
	while (1) {
		if ((port->ci & (1<<slot)) == 0) 
			break;
		if (port->is & HBA_PxIS_TFES) {
			printf("Read SATA disk error\n");
			return 0;
		}
	}

	if (port->is & HBA_PxIS_TFES) {
		printf("Read SATA disk error\n");
		return 0;
	}

	return 1;
}

int ahci_find_cmd_slot(hba_port *port)
{
	int slots = (port->sact | port->ci);
	for (size_t i=0; i < 32; i++) {
		if ((slots & 1) == 0)
			return i;
		slots >>= 1;
	}

	printf("Can't find a free command list slot\n");
	return -1;
}
