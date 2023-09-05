#include <arch/x86_64-elf/vga.h>

static inline uint8_t vga_entry_color(enum vga_color fg, enum vga_color bg) 
{
	return fg | bg << 4;
}
 
static inline uint16_t vga_entry(unsigned char uc, uint8_t color) 
{
	return (uint16_t) uc | (uint16_t) color << 8;
}
 
static const size_t VGA_WIDTH = 80;
static const size_t VGA_HEIGHT = 25;
 
size_t vga_row;
size_t vga_column;
uint8_t vga_color;
uint16_t* vga_buffer;
 
void vga_initialize(void) 
{
	vga_row = 0;
	vga_column = 0;
	vga_color = vga_entry_color(VGA_COLOR_LIGHT_GREY, VGA_COLOR_BLACK);
	vga_buffer = (uint16_t*) 0xB8000;
	for (size_t y = 0; y < VGA_HEIGHT; y++) {
		for (size_t x = 0; x < VGA_WIDTH; x++) {
			const size_t index = y * VGA_WIDTH + x;
			vga_buffer[index] = vga_entry(' ', vga_color);
		}
	}
}
 
void vga_setcolor(uint8_t color) 
{
	vga_color = color;
}
 
void vga_put_entry_at(char c, uint8_t color, size_t x, size_t y) 
{
	const size_t index = y * VGA_WIDTH + x;
	vga_buffer[index] = vga_entry(c, color);
}
 
void vga_putchar(char c) 
{
	if (c == '\n') {
		vga_column = 0;
		vga_row++;
		return;
	}

	vga_put_entry_at(c, vga_color, vga_column, vga_row);
	if (++vga_column == VGA_WIDTH) {
		vga_column = 0;
		if (++vga_row == VGA_HEIGHT)
			vga_row = 0;
	}
}
 
void vga_write(const char* data, size_t size) 
{
	for (size_t i = 0; i < size; i++)
		vga_putchar(data[i]);
}
 
void vga_write_string(const char* data) 
{
	vga_write(data, strlen(data));
}
