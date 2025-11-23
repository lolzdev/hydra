#include <riscv64/isa.h>

struct sbiret riscv_sbi_call(uint64_t ext, uint64_t fid, uint64_t arg0, uint64_t arg1, uint64_t arg2)
{
	struct sbiret ret;

	register uint64_t a0 asm("a0") = arg0;
	register uint64_t a1 asm("a1") = arg1;
	register uint64_t a2 asm("a2") = arg2;
	register uint64_t a6 asm("a6") = fid;
	register uint64_t a7 asm("a7") = ext;

	asm volatile (
	"ecall"
	: "+r"(a0), "+r"(a1)
	: "r"(a2), "r"(a6), "r"(a7)
	: "memory"
	);

	ret.error = a0;
	ret.value = a1;
	return ret;
}
