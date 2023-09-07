#include <arch/x86_64-elf/fb.h>

uint32_t char_width = 0;
uint32_t char_height = 0;
uint32_t fb_width;
uint32_t fb_height;
size_t row;
size_t column;
uint32_t *framebuffer;

void init_fb(uint32_t width, uint32_t height, uint32_t *fb)
{
	fb_width = width;
	fb_height = height;
	framebuffer = fb;
	char_width = width / 9;
	char_height = height / 17;
	row = 0;
	column = 0;
}

void fb_putchar_at(uint32_t x, uint32_t y, char c)
{
	put_char_at(framebuffer, c, x * 9, y * 17, fb_width);
}

void fb_putchar(char c)
{
	if (c == '\n') {
		row = 0;
		column++;
		return;
	}

	fb_putchar_at(row, column, c);
	row++;
	if (row > char_width) {
		row = 0;
		column++;
	}
}
