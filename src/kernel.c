#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <multiboot.h>
#include <flanterm/flanterm.h>
#include <flanterm/backends/fb.h>
#include <lib/printf.h>
#include <interrupts/gdt.h>
struct flanterm_context *ft_ctx = NULL;
void outb(uint16_t port, uint8_t val)
{
    asm volatile ("outb %0, %1" : : "a"(val), "Nd"(port));
}
uint8_t inb(uint16_t port)
{
    uint8_t ret;
    asm volatile ("inb %1, %0" : "=a"(ret) : "Nd"(port));
    return ret;
}
multiboot_info_t* mb;
uint32_t* framebuffer;
uint32_t framebuffer_width;
uint32_t framebuffer_height;
uint32_t framebuffer_pitch;

void enable_sse() {
    uint32_t cr0;
    uint32_t cr4;
    asm volatile ("mov %%cr0, %0" : "=r"(cr0));
    cr0 &= ~(1 << 2);
    asm volatile ("mov %0, %%cr0" : : "r"(cr0));
    asm volatile ("mov %%cr4, %0" : "=r"(cr4));
    cr4 |= 1 << 9;
    cr4 |= 1 << 10;
    asm volatile ("mov %0, %%cr4" : : "r"(cr4));
}

void test_user_function() {
    for(;;);
}
extern void jump_usermode();
void kernel_entry(uintptr_t stack_top, uintptr_t stack_bottom,
						multiboot_info_t* mboot, uint32_t magic) 
{
    mb = mboot;
    framebuffer = (uint32_t*)mb->framebuffer_addr;
    framebuffer_width = mb->framebuffer_width;
    framebuffer_height = mb->framebuffer_height;
    framebuffer_pitch = mb->framebuffer_pitch;
    uint32_t red_mask_size = mb->framebuffer_red_mask_size;
    uint32_t red_mask_shift = mb->framebuffer_red_field_position;
    uint32_t green_mask_size = mb->framebuffer_green_mask_size;
    uint32_t green_mask_shift = mb->framebuffer_green_field_position;
    uint32_t blue_mask_size = mb->framebuffer_blue_mask_size;
    uint32_t blue_mask_shift = mb->framebuffer_blue_field_position;
     ft_ctx = flanterm_fb_init(
        NULL,
        NULL,
        framebuffer, framebuffer_width, framebuffer_height, framebuffer_pitch,
        red_mask_size, red_mask_shift,
        green_mask_size, green_mask_shift,
        blue_mask_size, blue_mask_shift,
        NULL,
        NULL, NULL,
        NULL, NULL,
        NULL, NULL,
        NULL, 0, 0, 1,
        0, 0,
        0
    );
    printf("NexusOS Booting...\n");
    enable_sse();
    printf("SSE Enabled\n");
    gdt_init();
    printf("GDT Initialized\n");
    jump_usermode();
    while (1) {
        asm volatile ("hlt");
    }

}
void __udivmoddi4()
{
   for(;;);
}
void putchar_(char c) {
    const char message[1] = {c};
    flanterm_write(ft_ctx, message, 1);
}