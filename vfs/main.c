#include <stdint.h>
#include <stdarg.h>

int main(void)
{
	char *msg = "Hello world!\n";
	char *msg1 = "Hello world from child!\n";
	uint64_t a0 = 3;
	__asm__ volatile("li a1, 0xca; li a0, 0xfe; ecall" : : :);
	__asm__ volatile("mv %0, a0" : "=r"(a0) : :);
	if (a0 == 0) {
		__asm__ volatile("mv a1, %0; li a0, 0x1; ecall" : : "r"(msg1));
	} else {
		__asm__ volatile("mv a1, %0; li a0, 0x1; ecall" : : "r"(msg));
	}
	while (1);

	return 0;
}
