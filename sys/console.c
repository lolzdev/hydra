	/*-
	 * Copyright 2025 Lorenzo Torres
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

#include <console.h>
#include <uart.h>
#include <types.h>
#include <stdarg.h>

/* Convert a number to a string in base `base` */
char *cns_convert(uint64_t number, size_t base)
{
	char digits[] = "0123456789abcdef";
	static char result[20];
	result[0] = '\0';

	size_t i = 0;
	do {
		char digit = digits[number % base];
		number /= base;
		result[i+1] = result[i];
		result[i] = digit;

		i++;
	} while (number > 0);

	char *p1 = result;
	char *p2 = result + i - 1;

	while (p1 < p2) {
		char tmp = *p1;
		*p1++ = *p2;
		*p2-- = tmp;
	}

	return result;
}

void cns_printf(char *fmt, ...)
{
	va_list argv;
	va_start(argv, fmt);

	while (*fmt != '\0') {
		if (*fmt == '%') {
			fmt++;
			if (*fmt == '%') {
				uart_tx('%');
			} else if (*fmt == 'c') {
				char char_to_print = va_arg(argv, int);
				uart_tx(char_to_print);
			} else if (*fmt == 's') {
				char *s = va_arg(argv, char *);
				uart_puts(s);
			} else if (*fmt == 'x') {
				char *s = cns_convert(va_arg(argv, uint64_t), 16);
				uart_puts(s);
			} else if (*fmt == 'd') {
				char *s = cns_convert(va_arg(argv, uint64_t), 10);
				uart_puts(s);
			} else if (*fmt == 'b') {
				char *s = cns_convert(va_arg(argv, uint64_t), 2);
				uart_puts(s);
			}
		} else {
			uart_tx(*fmt);
		}
		fmt++;
	}
	va_end(argv);
}
