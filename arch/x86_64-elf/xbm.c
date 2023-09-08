#include <arch/x86_64-elf/xbm.h>
#include <fonts/font_8x16.h>

void put_char_at(uint32_t *buffer, char c, uint32_t x, uint32_t y, uint32_t buffer_width)
{
	uint32_t original_x = x;
	const char *data = font_8x16[c - 32];
	for (int j=0; j<16; j++) {
		char byte = data[j];
		for (int i=8; i>-1; i--) {
			uint32_t index = y*buffer_width + x;
			buffer[index] = ((byte >> i) & 0x1) ? 0xffffff : 0x0;
			x++;
		}
		x = original_x;
		y++;
	}
}
