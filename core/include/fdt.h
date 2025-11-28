#ifndef FDT_H
#define FDT_H

#include <stdint.h>

#define FDT_HEADER_MAGIC 0xedfe0dd0
#define FDT_BEGIN_NODE 0x00000001
#define FDT_END_NODE 0x00000002
#define FDT_PROP 0x00000003
#define FDT_NOP 0x00000004
#define FDT_END 0x00000009
#define MEM_MAP_MAX_REGIONS 32

#define BE_TO_LE(x) ((((x)>>24)&0xff) |\
                    (((x)<<8)&0xff0000) |\
                    (((x)>>8)&0xff00) |\
                    (((x)<<24)&0xff000000))

#define BE_TO_LE64(x) ((((x)>>24)&0xff) |\
                      (((x)<<8)&0xff0000) |\
                      (((x)>>8)&0xff00) |\
                      (((x)<<24)&0xff000000))


typedef enum {
	REGION_FREE_RAM,
	REGION_RESERVED
} region_type;

typedef struct {
	uint64_t base;
	uint64_t size;
	region_type type;
} mem_region;

typedef struct {
	mem_region regions[MEM_MAP_MAX_REGIONS];
	int count;
} mem_map;

struct fdt_header {
	uint32_t magic;
	uint32_t totalsize;
	uint32_t off_dt_struct;
	uint32_t off_dt_strings;
	uint32_t off_mem_rsvmap;
	uint32_t version;
	uint32_t last_comp_version;
	uint32_t boot_cpuid_phys;
	uint32_t size_dt_strings;
	uint32_t size_dt_struct;
};

struct fdt_reserve_entry {
	uint64_t address;
	uint64_t size;
};

struct fdt_prop {
	uint32_t len;
	uint32_t nameoff;
};

typedef struct {
	uint64_t ram_base;
	uint64_t ram_size;
	uint32_t cpu_count;
	uint64_t uart_base;
} hw_info;

void fdt_probe(struct fdt_header *header, hw_info *info);
void fdt_get_memory_map(struct fdt_header *header, mem_map *map);

#endif
