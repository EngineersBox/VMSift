#pragma once

#ifndef H_VMSIFT_PAGEMAP
#define H_VMSIFT_PAGEMAP

#include <stdint.h>
#include <stdio.h>
#include <sys/types.h>

#define _XOPEN_SOURCE 700

typedef struct {
    uint64_t pfn : 55;
    unsigned int soft_dirty : 1;
    unsigned int file_page : 1;
    unsigned int swapped : 1;
    unsigned int present : 1;
} PagemapEntry;

uintptr_t get_virtual_page_number(uintptr_t virtual_address);

/**
 * Parse the pagemap entry for the given virtual address.
 *
 * @param[out] entry      the parsed entry
 * @param[in]  pagemap_fd file descriptor to an open /proc/pid/pagemap file
 * @param[in]  vaddr      virtual address to get entry for
 * @return 0 for success, 1 for failure
 */
int pagemap_get_entry(PagemapEntry *entry, int pagemap_fd, uintptr_t vaddr);

/**
 * Convert the given virtual address to physical using /proc/PID/pagemap.
 *
 * @param[out] paddr physical address
 * @param[in]  pid   process to convert for
 * @param[in] vaddr virtual address to get entry for
 * @return 0 for success, 1 for failure
 */
int virtual_to_physical_user(uintptr_t *paddr, pid_t pid, uintptr_t vaddr);

#endif // H_VMSIFT_PAGEMAP