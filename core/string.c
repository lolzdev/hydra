#include <string.h>

size_t strlen(const char* str)
{
	size_t len = 0;
	while (str[len])
		len++;
	return len;
}

void memset(uint8_t *mem, uint8_t c, uint64_t n)
{
    for (uint64_t i = 0; i < n; i++) {
        *(mem + i) = c;
    }
}
