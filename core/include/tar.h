#ifndef TAR_H
#define TAR_H
#include <stdint.h>

struct tar_header {
	uint8_t name[100];
	uint8_t mode[8];
	uint8_t uid[8];
	uint8_t gid[8];
	uint8_t size[12];
	uint8_t mtime[12];
	uint8_t chksum[8];
	uint8_t type;
	/* 
	 * There are some other fields in UStar format
	 * but since I'm not using those right now I'll
	 * just leave them as padding.
	 */
	uint8_t padding[355];
};

#endif
