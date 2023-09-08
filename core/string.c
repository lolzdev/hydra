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

void memmove(void *dest, void *src, size_t n) 
{ 
	char *csrc = (char *)src; 
	char *cdest = (char *)dest; 

	char *temp = k_page_zalloc(1); 

	for (int i=0; i<n; i++) 
		temp[i] = csrc[i]; 

	for (int i=0; i<n; i++) 
	cdest[i] = temp[i]; 

	k_page_free(temp);
} 
