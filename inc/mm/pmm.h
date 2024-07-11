#pragma once

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include <multiboot.h>

#define PAGE_SIZE 4096

#define DIV_ROUND_UP(x, y) (x + (y - 1)) / y
#define ALIGN_UP(x, y) DIV_ROUND_UP(x, y) * y
#define ALIGN_DOWN(x, y) (x / y) * y

void pmm_init(multiboot_info_t* mboot);

void* pmm_alloc(size_t pages);
void pmm_free(void* ptr, size_t pages);
