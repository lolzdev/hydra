#ifndef VM_H
#define VM_H
#include <stdint.h>
#include <stddef.h>

#define VM_PAGE_SIZE 0x1000
#define HH_MASK 0xffffffff00000000UL

#define VM_PTE_VALID 0x1
#define VM_PTE_READ 0x1 << 1
#define VM_PTE_WRITE 0x1 << 2
#define VM_PTE_EXEC 0x1 << 3
#define VM_PTE_USER 0x1 << 4

extern uint64_t *kernel_pt;

/*
 * Create the kernel page table making sure of
 * keeping everything in the higher half of the
 * address space.
 */
void vm_init(void);
/*
 * Given a page table, map address `virtual` to
 * `physical` using `flags`.
 */
void vm_mmap(uint64_t *table, size_t virtual, size_t physical, uint64_t flags);
/*
 * Load a page table by creating the correct value
 * for the CSR and flush the cache.
 */
void vm_load_page_table(uint64_t *pt);
/*
 * Flush the cache to make sure that the page table
 * updates take effect.
 */
void vm_reload(void);
/*
 * Walk the page table to get the physical address
 * mapped to address `virtual`.
 */
uint64_t vm_get_phys(uint64_t *table, size_t virtual);
/* Allocate a new page table. */
uint64_t *vm_create_page_table(void);

#endif
