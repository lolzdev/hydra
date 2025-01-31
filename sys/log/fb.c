	/*-
	 * Copyright 2024 Lorenzo Torres
 	 *
	 * Redistribution and use in source and binary forms, with or without
	 * modification, are permitted provided that the following conditions are met:
	 * 1. Redistributions of source code must retain the above copyright
	 *    notice, this list of conditions and the following disclaimer.
	 * 2. Redistributions in binary form must reproduce the above copyright
	 *    notice, this list of conditions and the following disclaimer in the
	 *    documentation and/or other materials provided with the distribution.
	 * 3. Neither the name of the copyright holder nor
	 *    the names of its contributors may be used to endorse or promote products
	 *    derived from this software without specific prior written permission.
	 *
	 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS “AS IS” AND ANY
	 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
	 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
	 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY
	 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
	 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
	 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
	 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
	 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
	 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
	 */

#include <log/fb.h>
#include <stdarg.h>
#include <log/font8x16.h>

static uint32_t char_width = 0;
static uint32_t char_height = 0;
static uint32_t fb_width;
static uint32_t fb_height;
static size_t row;
static size_t column;
static uint32_t *framebuffer;

void fb_init(uint32_t width, uint32_t height, uint32_t *fb)
{
	fb_width = width;
	fb_height = height;
	framebuffer = fb;
	char_width = width / 9;
	char_height = height / 17;
	row = 0;
	column = 0;
}

void fb_kputchar_at(uint32_t x, uint32_t y, char c )
{
	x *= 9;
	y *= 17;
	uint32_t original_x = x;
	const char *data = font_8x16[c - 32];
	for (int j=0; j<16; j++) {
		char byte = data[j];
		for (int i=8; i>-1; i--) {
			uint32_t index = y*fb_width + x;
			framebuffer[index] = ((byte >> i) & 0x1) ? 0xffffff : 0x0;
			x++;
		}
		x = original_x;
		y++;
	}
}


void fb_kputchar(char c)
{
	if (row >= fb_height) row = 0;
	if (column >= fb_width) column = 0;
	if (c == '\n') {
		if (row > char_height) {
			row = 0;
		}
		column = 0;
		row++;
		return;
	}

	fb_kputchar_at(column, row, c);
	column++;
	if (column > char_width) {
		column = 0;
		row++;
	}
}

void kputchar(char c)
{
	fb_kputchar(c);
}

void kputs(const char *s)
{
	while (*s != '\0') {
		kputchar(*s);
		s++;
	}
}

static char *convert(uint64_t num, int base)
{
    static char representation[]= "0123456789abcdef";
    static char buffer[50];
    char *ptr;

    ptr = &buffer[49];
    *ptr = '\0';

    while(1) {
        *--ptr = representation[num%base];
		if (num == 0) break;
        num /= base;
    }

    return ptr;
}

void kprintf(char* format, ...)
{
    char *traverse;
    int i;
    char *s;

    va_list arg;
    va_start(arg, format);

    for(traverse = format; *traverse != '\0'; traverse++)
    {
        while( *traverse != '%' && *traverse != '\0')
        {
            kputchar(*traverse);
            traverse++;
        }

		if (*traverse == '\0') break;

        traverse++;

        switch(*traverse)
        {
            case 'c' : i = va_arg(arg,int);
                        kputchar(i);
                        break;

            case 'd' : i = va_arg(arg,int64_t);
                        if(i<0)
                        {
                            i = -i;
                            kputchar('-');
                        }
                        kputs(convert(i,10));
                        break;

            case 'o': i = va_arg(arg,uint64_t);
                        kputs(convert(i,8));
                        break;

            case 's': s = va_arg(arg,char *);
                        kputs(s);
                        break;

            case 'x': i = va_arg(arg,uint64_t);
                        kputs(convert(i,16));
                        break;
        }
    }

    va_end(arg);
}
