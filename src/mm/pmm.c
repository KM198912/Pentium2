#include <mm/pmm.h>
#include <mm/bitmap.h>
#include <lib/string.h>

// Ported over from ZanOS (https://github.com/asterd-og/ZanOS)

uint8_t* pmm_bitmap = NULL;
uint32_t pmm_free_pages = 0;
uint32_t pmm_used_pages = 0;
uint32_t pmm_total_pages = 0;
uint32_t pmm_last_page = 0;

void pmm_init(multiboot_info_t* mboot) {
    multiboot_memory_map_t* mmap = (multiboot_memory_map_t*)mboot->mmap_addr;

    uint32_t mmap_size = mboot->mmap_length;
    uint32_t mmap_count = mmap_size / sizeof(multiboot_memory_map_t);

    uint32_t higher_address = 0;
    uint32_t top_address = 0;

    for (uint32_t i = 0; i < mmap_count; i++) {
        if (mmap[i].type != MULTIBOOT_MEMORY_AVAILABLE) {
            continue;
        }
        top_address = mmap[i].addr + mmap[i].len;
        if (top_address > higher_address) {
            higher_address = top_address;
        }
    }

    pmm_total_pages = higher_address / PAGE_SIZE;
    uint32_t bitmap_size = ALIGN_UP(pmm_total_pages / 8, PAGE_SIZE);

    for (uint32_t i = 0; i < mmap_count; i++) {
        if (mmap[i].type != MULTIBOOT_MEMORY_AVAILABLE || mmap[i].len < bitmap_size) continue;

        pmm_bitmap = (uint8_t*)mmap[i].addr;
        memset(pmm_bitmap, 0xFF, bitmap_size);
        mmap[i].addr += bitmap_size;
        mmap[i].len -= bitmap_size;

        break;
    }

    for (uint32_t i = 0; i < mmap_count; i++) {
        if (mmap[i].type != MULTIBOOT_MEMORY_AVAILABLE) continue;

        for (uint32_t j = 0; j < mmap[i].len; j += PAGE_SIZE) {
            bitmap_clear(pmm_bitmap, (mmap[i].addr + j) / PAGE_SIZE);
        }
    }

    char str2[10];
    printf("PMM Initialised. Total size: %s\n", itoa(bitmap_size, str2, 10));
}

uint32_t pmm_find_pages(size_t n) {
    uint32_t pages = 0;
    uint32_t first_page = pmm_last_page;
    while (pages < n) {
        if (pmm_last_page == pmm_total_pages) {
            return 0;
        }
        if (bitmap_get(pmm_bitmap, pmm_last_page + pages) == 0) {
            pages++;
            if (pages == n) {
                for (uint32_t i = 0; i < pages; i++) {
                    bitmap_set(pmm_bitmap, pmm_last_page + i);
                }
                pmm_last_page += pages;
                return first_page; // Return the start of the pages
            }
        }
        else {
            pmm_last_page += (pages == 0 ? 1 : pages);
            first_page = pmm_last_page;
            pages = 0;
        }
    }
    return 0;
}

void* pmm_alloc(size_t n) {
    uint32_t first = pmm_find_pages(n);
    if (first == 0) {
        pmm_last_page = 0;
        first = pmm_find_pages(n);
        if (first == 0) {
            printf("pmm_alloc(): Couldn't allocate %lu pages: Not enough memory.\n", n);
            return NULL;
        }
    }
    uint32_t addr = first * PAGE_SIZE;
    return (void*)(addr);
}

void pmm_free(void* ptr, size_t pages) {
    uint32_t idx = (uint32_t)ptr / PAGE_SIZE;
    for (uint32_t i = 0; i < pages; i++) {
        bitmap_clear(pmm_bitmap, idx + i);
    }
}