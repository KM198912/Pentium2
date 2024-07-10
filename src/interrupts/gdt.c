#include <interrupts/gdt.h>
static gdt_entry_t gdt[6];
gdt_ptr_t gdt_ptr;
tss_t tss;
tss_t tss_entry;

void write_tss(gdt_entry_t *g) {
    uint32_t base = (uint32_t) &tss_entry;
    uint32_t limit = sizeof(tss_entry) - 1;

    g->limit_low = limit & 0xFFFF;
    g->base_low = base & 0xFFFFFF;
    g->access = 0x89;
    g->granularity = (limit >> 16) & 0x0F;
    g->granularity |= 0x40;
    g->base_high = (base >> 24) & 0xFF;

    memset(&tss_entry, 0, sizeof(tss_entry));
    tss_entry.ss0 = 0x10;
    tss_entry.esp0 = 0x20000;
}

void set_kernel_stack(uint32_t stack) {
    tss_entry.esp0 = stack;
}

extern void gdt_flush(uint32_t);
extern void flush_tss(void);

void gdt_init() {
    gdt_ptr.limit = (sizeof(gdt_entry_t) * 6) - 1;
    gdt_ptr.base = (uint32_t)&gdt;

    gdt[0] = (gdt_entry_t){0};

    gdt[1] = (gdt_entry_t){
        .limit_low = 0xFFFF,
        .base_low = 0,
        .base_middle = 0,
        .access = 0x9A,
        .granularity = 0xCF,
        .base_high = 0
    };

    gdt[2] = (gdt_entry_t){
        .limit_low = 0xFFFF,
        .base_low = 0,
        .base_middle = 0,
        .access = 0x92,
        .granularity = 0xCF,
        .base_high = 0
    };

    gdt[3] = (gdt_entry_t){
        .limit_low = 0xFFFF,
        .base_low = 0,
        .base_middle = 0,
        .access = 0xFA,
        .granularity = 0xCF,
        .base_high = 0
    };

    gdt[4] = (gdt_entry_t){
        .limit_low = 0xFFFF,
        .base_low = 0,
        .base_middle = 0,
        .access = 0xF2,
        .granularity = 0xCF,
        .base_high = 0
    };

    write_tss(&gdt[5]);

    gdt_flush((uint32_t)&gdt_ptr);
    flush_tss();
}
