#include "pagemap.h"

#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

uintptr_t get_virtual_page_number(uintptr_t virtual_address) {
    return vaddr / sysconf(_SC_PAGE_SIZE);
}

int pagemap_get_entry(PagemapEntry *entry, int pagemap_fd, uintptr_t vaddr) {
    ssize_t ret;
    uint64_t data;
    uintptr_t vpn = get_virtual_page_number(vaddr);
    size_t nread = 0;
    while (nread < sizeof(data)) {
        ret = pread(
            pagemap_fd,
            ((uint8_t*)&data) + nread,
            sizeof(data) - nread,
            vpn * sizeof(data) + nread
        );
        nread += ret;
        if (ret <= 0) {
            return 1;
        }
    }
    entry->pfn = data & (((uint64_t) 1 << 55) - 1);
    entry->soft_dirty = (data >> 55) & 1;
    entry->file_page = (data >> 61) & 1;
    entry->swapped = (data >> 62) & 1;
    entry->present = (data >> 63) & 1;
    return 0;
}



int virtual_to_physical_user(uintptr_t *paddr, pid_t pid, uintptr_t vaddr) {
    char pagemap_file[BUFSIZ];
    snprintf(
        pagemap_file,
        sizeof(pagemap_file),
        "/proc/%ju/pagemap",
        (uintmax_t) pid
    );
    int pagemap_fd = open(pagemap_file, O_RDONLY);
    if (pagemap_fd < 0) {
        return 1;
    }
    PagemapEntry entry;
    if (pagemap_get_entry(&entry, pagemap_fd, vaddr)) {
        return 1;
    }
    close(pagemap_fd);
    *paddr = (entry.pfn * sysconf(_SC_PAGE_SIZE)) + (vaddr % sysconf(_SC_PAGE_SIZE));
    return 0;
}