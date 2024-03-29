#include <stdio.h>

void putchar(char c)
{
#ifdef x86_64
	fb_putchar(c);
#endif
}

void puts(const char *s)
{
	while (*s != '\0') {
		putchar(*s);
		s++;
	}
}

char *convert(unsigned int num, int base)
{
    static char representation[]= "0123456789ABCDEF";
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

void printf(char* format,...)
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
            putchar(*traverse);
            traverse++;
        }

		if (*traverse == '\0') break;

        traverse++;

        switch(*traverse)
        {
            case 'c' : i = va_arg(arg,int);
                        putchar(i);
                        break;

            case 'd' : i = va_arg(arg,int);
                        if(i<0)
                        {
                            i = -i;
                            putchar('-');
                        }
                        puts(convert(i,10));
                        break;

            case 'o': i = va_arg(arg,unsigned int);
                        puts(convert(i,8));
                        break;

            case 's': s = va_arg(arg,char *);
                        puts(s);
                        break;

            case 'x': i = va_arg(arg,unsigned int);
                        puts(convert(i,16));
                        break;
        }
    }

    va_end(arg);
}


