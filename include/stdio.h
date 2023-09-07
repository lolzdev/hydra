#ifndef STDIO_H
#define STDIO_H

#include <stdarg.h>

#ifdef x86_64
#include <arch/x86_64-elf/fb.h>
#endif

void putchar(char c);
void puts(const char *s);
void printf(char *format, ...);

#endif
