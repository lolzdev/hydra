#include <tags.h>

__attribute__((used, section(".hydra_tags")))
static volatile struct hydra_tag_memmap memmap = {
	.type = HYDRA_TAG_MEMMAP_TYPE
};

void _start(void)
{
	while(1);
}
