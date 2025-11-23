#ifndef UART_H
#define UART_H

#define UART_BASE 0x10000000

void uart_putc(char c);
void uart_puts(char *s);
void uart_printf(char* format, ...);

#endif
