#include <tar.h>
#include <stdint.h>
#include <stddef.h>
#include <proc.h>
#include <drivers/uart.h>

extern char _binary_initramfs_start[];
extern char _binary_initramfs_end[];

static uint64_t oct2int(uint8_t *oct) {
	uint64_t ret = 0;
	while (*oct >= '0' && *oct <= '7') {
		ret = (ret * 8) + (*oct++ - '0');
	}
	return ret;
}

/* Initialize essential services */
void kernel_init()
{
	struct tar_header *header = (struct tar_header *) _binary_initramfs_start;
	uart_puts("Core services found: ");
	while (1) {
		if (header->name[0] == '\0') {
			break;
		}

		uart_puts((char *)header->name);
		size_t file_size = oct2int(header->size);
		uint8_t *data = (uint8_t *) header + sizeof(struct tar_header);
		
		process_create((uint64_t)data);

		size_t offset = sizeof(struct tar_header) + ((file_size + sizeof(struct tar_header) - 1) / sizeof(struct tar_header)) * sizeof(struct tar_header);

		header = (struct tar_header *) ((size_t)header + offset);
	}
	uart_puts("\n");
}
