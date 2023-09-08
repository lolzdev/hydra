#ifndef STRING_H
#define STRING_H

#include <stddef.h>
#include <stdint.h>
#ifdef x86_64
#include <arch/x86_64-elf/mm.h>
#endif

size_t strlen(const char* str);
void memset(uint8_t *mem, uint8_t c, uint64_t n);
void memmove(void *dest, void *src, size_t n);

#endif
