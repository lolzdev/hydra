#ifndef ISA_H
#define ISA_H

#include <stdint.h>

#define RISCV_SATP_SV48 9
#define RISCV_MAKE_SATP(addr, mode) (((uint64_t)(mode) << 60) | ((uint64_t)(addr) >> 12))
#define RISCV_SSTATUS_SIE (1L << 1)

static inline void riscv_set_satp(uint64_t satp)
{
	__asm__ volatile("csrw satp, %0" : : "r"(satp));
}

static inline void riscv_set_stvec(uint64_t stvec)
{
	__asm__ volatile("csrw stvec, %0" : : "r"(stvec));
}

static inline void riscv_set_sstatus(uint64_t sstatus)
{
	__asm__ volatile("csrs sstatus, %0" : : "r"(sstatus));
}

static inline void riscv_set_sepc(uint64_t sepc)
{
	__asm__ volatile("csrs sepc, %0" : : "r"(sepc));
}

static inline uint64_t riscv_get_sstatus(void)
{
	uint64_t sstatus = 0;
	__asm__ volatile("csrr %0, sstatus" : "=r"(sstatus) :);
	return sstatus;
}

static inline uint64_t riscv_get_stval(void)
{
	uint64_t stval = 0;
	__asm__ volatile("csrr %0, stval" : "=r"(stval) :);
	return stval;
}

static inline uint64_t riscv_get_scause(void)
{
	uint64_t scause = 0;
	__asm__ volatile("csrr %0, scause" : "=r"(scause) :);
	return scause;
}

static inline void riscv_enable_interrupts(void)
{
	riscv_set_sstatus(RISCV_SSTATUS_SIE);
}

static inline void riscv_disable_interrupts(void)
{
	uint64_t current = riscv_get_sstatus();
	riscv_set_sstatus(current & ~RISCV_SSTATUS_SIE);
}

static inline uint64_t riscv_get_time(void)
{
	uint64_t time = 0;
	__asm__ volatile("rdtime %0" : "=r"(time) :);
	return time;
}

/* SBI specific functions */
struct sbiret {
	uint64_t error;
	uint64_t value;
};

struct sbiret riscv_sbi_call(uint64_t ext, uint64_t fid, uint64_t arg0, uint64_t arg1, uint64_t arg2);

static inline void riscv_sbi_set_timer(uint64_t time)
{
	riscv_sbi_call(0x54494D45, 0, time, 0, 0);
}

#endif
