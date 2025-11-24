.section .text
.align 4
.globl user_jump
.extern trap_entry
.extern uart_puts

/* void user_jump(struct frame *proc_frame, uint64_t *proc_table) */
user_jump:
	ld ra, 0(a0)
	ld gp, 16(a0)
	ld tp, 24(a0)
	ld t1, 40(a0)
	ld t2, 48(a0)
	ld t3, 56(a0)
	ld t4, 64(a0)
	ld t5, 72(a0)
	ld t6, 80(a0)
	ld s0, 88(a0)
	ld s1, 96(a0)
	ld s2, 104(a0)
	ld s3, 112(a0)
	ld s4, 120(a0)
	ld s5, 128(a0)
	ld s6, 136(a0)
	ld s7, 144(a0)
	ld s8, 152(a0)
	ld s9, 160(a0)
	ld s10, 168(a0)
	ld s11, 176(a0)
	ld a2, 200(a0)
	ld a3, 208(a0)
	ld a4, 216(a0)
	ld a5, 224(a0)
	ld a6, 232(a0)
	ld a7, 240(a0)

	la t0, trap_entry
	csrw stvec, t0

	csrr t0, sstatus
	andi t0, t0, ~(1 << 8)
	csrw sstatus, t0

	ld t0, 248(a0)
	csrw sepc, t0
	ld t0, 32(a0)

	csrw sscratch, sp

	ld sp, 8(a0)
	csrw satp, a1
	ld a1, 192(a0)
	ld a0, 184(a0)
	sfence.vma zero, zero

	sret
