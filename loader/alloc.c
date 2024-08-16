#include <alloc.h>
#include <efi/sys_table.h>

extern EFI_SYSTEM_TABLE *system_table;

void *malloc(size_t size)
{
    void *ptr;
    if (system_table->BootServices->AllocatePool(EfiLoaderData, size, &ptr) != 0)
    {
        return NULL;
    }

    return ptr;
}

void free(void *ptr)
{
    system_table->BootServices->FreePool(ptr);
}

void cpy_mem(void *dest, const void *src, size_t size)
{
    char *d = dest;
    const char *s = src;
    while (size--)
    {
        *d++ = *s++;
    }
}

void *realloc(void *ptr, size_t new_size)
{
    if (!ptr)
        return malloc(new_size);

    void *new_ptr = malloc(new_size);
    if (!new_ptr)
        return NULL;

    cpy_mem(new_ptr, ptr, new_size);
    free(ptr);
    return new_ptr;
}
