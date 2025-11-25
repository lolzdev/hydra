#include <stdint.h>
#include <stdarg.h>

int main(void)
{
	__asm__ volatile("ecall");
	while (1);

	return 0;
}
