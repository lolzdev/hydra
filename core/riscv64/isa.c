#include <riscv64/isa.h>
#include <stdint.h>

extern void smp_entry(uint64_t hartid);

struct sbiret riscv_sbi_call(uint64_t ext, uint64_t fid, uint64_t arg0, uint64_t arg1, uint64_t arg2)
{
	struct sbiret ret;

	register uint64_t a0 __asm__("a0") = arg0;
	register uint64_t a1 __asm__("a1") = arg1;
	register uint64_t a2 __asm__("a2") = arg2;
	register uint64_t a6 __asm__("a6") = fid;
	register uint64_t a7 __asm__("a7") = ext;

	__asm__ volatile (
	"ecall"
	: "+r"(a0), "+r"(a1)
	: "r"(a2), "r"(a6), "r"(a7)
	: "memory"
	);

	ret.error = a0;
	ret.value = a1;
	return ret;
}

int8_t riscv_sbi_probe_hart(uint64_t hartid) {
	struct sbiret ret = riscv_sbi_call(0x48534D, 2, hartid, 0, 0);
	if (ret.error == 0) {
		return 1;
	} else if (ret.error == -2 || ret.error == -3) {
		return 0;
	}
	return -1;
}

static uintptr_t get_trampoline_addr() {
	uintptr_t addr;

	__asm__ volatile (
		"ld %0, 1f\n\t"
		"j 2f\n\t"
		".align 3\n\t"
		"1: .dword smp_entry\n\t"
		"2:"
		: "=r" (addr)
		:
		: "memory"
	);

	return addr;
}

void riscv_sbi_wake_up(uint64_t hartid, uint64_t stack_top)
{
	riscv_sbi_call(0x48534D, 0, hartid, get_trampoline_addr(), stack_top);
}
