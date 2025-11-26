#include <drivers/uart.h>
#include <stdint.h>
#include <stdarg.h>
#include <spinlock.h>

static struct spinlock lock;
static volatile char *uart = (char *) UART_BASE;

void uart_init(void)
{
	lock.locked = 0;
}

void uart_putc(char c)
{
	uart[0] = c;
}

void uart_puts(char *s)
{
	spinlock_aquire(&lock);
	while (*s != 0)
		uart_putc(*s++);
	spinlock_release(&lock);
}

static char *convert(unsigned long num, int base)
{
	static char Representation[] = "0123456789abcdef";
	static char buffer[50];
	char *ptr;

	ptr = &buffer[49];
	*ptr = '\0';

	do {
		*--ptr = Representation[num % base];
		num /= base;
	} while(num != 0);

	return(ptr);
}

void uart_printf(char* format, ...)
{
	char *traverse;
	long i; 
	char *s;

	va_list arg;
	va_start(arg, format);

	for(traverse = format; *traverse != '\0'; traverse++)
	{
		if (*traverse != '%') {
			uart_putc(*traverse);
			continue;
		}

		traverse++;

		if (*traverse == '\0') {
			break;
		}

		switch(*traverse)
		{
		case 'c' : 
			i = va_arg(arg, int);
			uart_putc(i);
			break;

		case 'd' : 
			i = va_arg(arg, long);
			if(i < 0) {
				i = -i;
		        	uart_putc('-');
		    	}
			uart_puts(convert(i, 10));
			break;

		case 'o': 
			i = va_arg(arg, unsigned long);
			uart_puts(convert(i, 8));
			break;

		case 's': 
			s = va_arg(arg, char *);
			if (!s) { 
				uart_puts("(null)"); 
			} else {
				uart_puts(s);
			}
			break;

		case 'x': 
			i = va_arg(arg, unsigned long);
			uart_puts(convert(i, 16));
			break;
		
		case '%':
			uart_putc('%');
			break;
		    
		default:
			uart_putc('%');
			uart_putc(*traverse);
			break;
		}
	}

	va_end(arg);
}
