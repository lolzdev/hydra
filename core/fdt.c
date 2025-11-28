#include <fdt.h>
#include <stdint.h>
#include <stddef.h>
#include <drivers/uart.h>
#include <mem.h>

#define ALIGNUP(data, align) (((data) + (align) - 1) & ~((align) - 1))

static inline uint32_t swap32(uint32_t x) {
	return BE_TO_LE(x);
}

static uint64_t fdt_read_cells(const uint32_t *ptr, int cells) {
	if (cells == 1) return swap32(ptr[0]);
	if (cells == 2) return ((uint64_t)swap32(ptr[0]) << 32) | swap32(ptr[1]);
	return 0;
}

static int32_t strcmp(char *s1, char *s2)
{
	while (*s1 != '\0' && *s2 != '\0') {
		if (*s1++ != *s2++) {
			return 1;
		}
	}

	return 0;
}

void fdt_probe(struct fdt_header *header, hw_info *info)
{
	info->ram_base = 0;
	info->ram_size = 0;
	info->cpu_count = 0;

	int root_addr_cells = 2;
	int root_size_cells = 2;

	char *strings = (char *)((uint64_t)header + swap32(header->off_dt_strings));
	const uint32_t *struct_ptr = (const uint32_t *)((char *)header + swap32(header->off_dt_struct));
	const uint32_t *end_ptr = struct_ptr + (swap32(header->size_dt_struct) / 4);

	int depth = 0;
	int in_memory_node = 0;

	while (struct_ptr < end_ptr) {
		uint32_t token = swap32(*struct_ptr);
		struct_ptr++;

		switch (token) {
			case FDT_BEGIN_NODE: {
				char *name = (char *)struct_ptr;
				size_t name_len = 0;
				while (name[name_len] != '\0') name_len++;
				struct_ptr += (name_len + 4) / 4;

				if (depth == 0) {}
				depth++;

				in_memory_node = 0;
				break;
			}
			case FDT_END_NODE:
				depth--;
				in_memory_node = 0;
				break;
			case FDT_PROP: {
				uint32_t len = swap32(*struct_ptr++);
				uint32_t nameoff = swap32(*struct_ptr++);
				char *prop_name = strings + nameoff;
				const uint32_t *prop_val = struct_ptr;

				struct_ptr += (len + 3) / 4;

				if (depth == 1) {
					if (strcmp(prop_name, "#address-cells") == 0) 
						root_addr_cells = swap32(*prop_val);
					if (strcmp(prop_name, "#size-cells") == 0) 
						root_size_cells = swap32(*prop_val);
				}

				if (strcmp(prop_name, "device_type") == 0) {
					char *val_str = (char*)prop_val;
					if (strcmp(val_str, "memory") == 0) in_memory_node = 1;
					if (strcmp(val_str, "cpu") == 0) {
						info->cpu_count++;
					}
				}

				if (in_memory_node && strcmp(prop_name, "reg") == 0) {
					info->ram_base = fdt_read_cells(prop_val, root_addr_cells);
					const uint32_t *size_ptr = prop_val + root_addr_cells;
					info->ram_size = fdt_read_cells(size_ptr, root_size_cells);
				}

				break;
			}
			case FDT_END:
				return;
		}
	}
}

static inline uint64_t swap64(uint64_t x) {
    return ((x & 0x00000000000000FFULL) << 56) |
           ((x & 0x000000000000FF00ULL) << 40) |
           ((x & 0x0000000000FF0000ULL) << 24) |
           ((x & 0x00000000FF000000ULL) << 8)  |
           ((x & 0x000000FF00000000ULL) >> 8)  |
           ((x & 0x0000FF0000000000ULL) >> 24) |
           ((x & 0x00FF000000000000ULL) >> 40) |
           ((x & 0xFF00000000000000ULL) >> 56);
}

static void add_mem_region(mem_map *map, uint64_t base, uint64_t size, region_type type)
{
	if (map->count >= MEM_MAP_MAX_REGIONS) return;
	if (size == 0) return;

	map->regions[map->count].base = base;
	map->regions[map->count].size = size;
	map->regions[map->count].type = type;
	map->count++;
}

void fdt_get_memory_map(struct fdt_header *header, mem_map *map)
{
	map->count = 0;

	struct fdt_reserve_entry *rsv_map = (struct fdt_reserve_entry *) ((uint64_t)header + swap32(header->off_mem_rsvmap));

	int i = 0;
	while (1) {
		uint64_t addr = swap64(rsv_map[i].address);
		uint64_t size = swap64(rsv_map[i].size);

		if (addr == 0 && size == 0) break;

		add_mem_region(map, addr, size, REGION_RESERVED);
		i++;
	}

	add_mem_region(map, (uint64_t)header, swap32(header->totalsize), REGION_RESERVED);

	char *strings = (char *)((uint64_t)header + swap32(header->off_dt_strings));
	const uint32_t *struct_ptr = (const uint32_t *)((char *)header + swap32(header->off_dt_struct));
	const uint32_t *end_ptr = struct_ptr + (swap32(header->size_dt_struct) / 4);

	int root_addr_cells = 2;
	int root_size_cells = 2;
	int depth = 0;
	int in_memory_node = 0;

	while (struct_ptr < end_ptr) {
		uint32_t token = swap32(*struct_ptr++);

		switch (token) {
			case FDT_BEGIN_NODE: {
				char *name = (char *)struct_ptr;
				int len = 0; 
				while (name[len] != '\0') len++;
				struct_ptr += (len + 4) / 4;

				if (depth == 0) { /* Root Node */ }
				depth++;

				in_memory_node = 0;
				if ((len >= 6 && name[0]=='m' && name[1]=='e' && name[2]=='m' && name[5]=='y') ||
					(len == 6 && strcmp(name, "memory") == 0)) {
					in_memory_node = 1;
				}
				break;
			}
			case FDT_END_NODE:
				depth--;
				in_memory_node = 0;
				break;
			case FDT_PROP: {
				uint32_t len = swap32(*struct_ptr++);
				uint32_t nameoff = swap32(*struct_ptr++);
				char *prop_name = strings + nameoff;
				const uint32_t *prop_val = struct_ptr;
				struct_ptr += (len + 3) / 4;

				if (depth == 1) {
				if (strcmp(prop_name, "#address-cells") == 0) 
					root_addr_cells = swap32(*prop_val);
				if (strcmp(prop_name, "#size-cells") == 0) 
					root_size_cells = swap32(*prop_val);
				}

				if (in_memory_node && strcmp(prop_name, "reg") == 0) {
					const uint32_t *p = prop_val;
					int remaining_len = len;
					
					while (remaining_len > 0) {
						uint64_t base = fdt_read_cells(p, root_addr_cells);
						p += root_addr_cells;
						uint64_t size = fdt_read_cells(p, root_size_cells);
						p += root_size_cells;
						
						add_mem_region(map, base, size, REGION_FREE_RAM);
						
						remaining_len -= (root_addr_cells + root_size_cells) * 4;
					}
				}
				break;
			}
			case FDT_END: return;
		}
	}
}
