#include <stdint.h>
#include <stdarg.h>

int main(void)
{
	char *msg1 = "Hello world from nvme!\n";
	__asm__ volatile("mv a1, %0; li a0, 0x1; ecall" : : "r"(msg1));
	while (1);

	return 0;
}
