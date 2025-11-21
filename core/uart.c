#include "uart.h"
#include <stdint.h>
#include <stdarg.h>

static volatile char *uart = (char *) UART_BASE;

void uart_putc(char c)
{
	uart[0] = c;
}

void uart_puts(char *s)
{
	while (*s != 0)
		uart_putc(*s++);
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

void uart_printf(char* format, ...)
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
		    uart_putc(*traverse);
		    traverse++;
		}

			if (*traverse == '\0') break;

		traverse++;

		switch(*traverse)
		{
		    case 'c' : i = va_arg(arg,int);
				uart_putc(i);
				break;

		    case 'd' : i = va_arg(arg,int64_t);
				if(i<0)
				{
				    i = -i;
				    uart_putc('-');
				}
				uart_puts(convert(i,10));
				break;

		    case 'o': i = va_arg(arg,uint64_t);
				uart_puts(convert(i,8));
				break;

		    case 's': s = va_arg(arg,char *);
				uart_puts(s);
				break;

		    case 'x': i = va_arg(arg,uint64_t);
				uart_puts(convert(i,16));
				break;
		}
	}

	va_end(arg);
}
