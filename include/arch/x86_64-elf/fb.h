#ifndef FB_H
#define FB_H

#include <arch/x86_64-elf/xbm.h>

void fb_putchar(char c);
void fb_putchar_at(uint32_t x, uint32_t y, char c);
void init_fb(uint32_t width, uint32_t height, uint32_t *fb);

#endif
