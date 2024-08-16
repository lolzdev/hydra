#ifndef ALLOC_H
#define ALLOC_H

#include <stddef.h>

void *malloc(size_t size);
void free(void *ptr);
void *realloc(void *ptr, size_t new_size);

#endif

