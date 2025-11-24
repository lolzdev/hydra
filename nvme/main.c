#include <stdint.h>
#include <stdarg.h>

static volatile char *uart = (char *) 0x10000000;

void uart_putc(char c)
{
	*uart = c;
}

void uart_puts(const char *s)
{
	while (*s != 0) {
		uart_putc(*s++);
	}
}

void uart_print_num(long num, int base, int sign) 
{
	char buf[65]; 
	char *ptr = &buf[64];
	*ptr = '\0';

	static const char representation[] = "0123456789abcdef";

	unsigned long n = num;

	if (sign && (num < 0)) {
		n = -num;
	}

	do {
		*--ptr = representation[n % base];
		n /= base;
	} while (n != 0);

	if (sign && (num < 0)) {
		*--ptr = '-';
	}

	uart_puts(ptr);
}

void uart_printf(char* format, ...)
{
	char *traverse;
	long l_val;
	int i_val;
	char *s_val;

	va_list arg;
	va_start(arg, format);

	for(traverse = format; *traverse != '\0'; traverse++) {
		if (*traverse != '%') {
			uart_putc(*traverse);
			continue;
		}

		traverse++;

		if (*traverse == '\0') {
			break;
		}

		switch(*traverse) {
		case 'c' :
			i_val = va_arg(arg, int);
			uart_putc(i_val);
			break;
		case 'd' :
			i_val = va_arg(arg, int);
			uart_print_num(i_val, 10, 1); 
			break;
		case 'l': 
			if (*(traverse+1) == 'd') {
			 traverse++;
			 l_val = va_arg(arg, long);
			 uart_print_num(l_val, 10, 1);
			}
			break;
		case 'x':
			l_val = va_arg(arg, unsigned long);
			uart_print_num(l_val, 16, 0);
			break;
		case 's':
			s_val = va_arg(arg, char *);
			if (!s_val) {
				uart_puts("(null)");
			} else {
				uart_puts(s_val);
			}
			break;
		default:
			uart_putc('%');
			uart_putc(*traverse);
			break;
		}
	}

	va_end(arg);
}

void delay(int count)
{
	volatile int i;
	for (i = 0; i < count; i++);
}

int main(void)
{
	int a = 0; 

	while (1) {
		uart_printf("Proc B: Value = %d\n", a);
		a++;

		delay(1000000); 
	}

	return 0;
}
