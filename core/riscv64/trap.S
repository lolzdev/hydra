.section .text
.align 4
.globl trap_entry
.extern current_proc
.extern kernel_pt
/*
 * When in user mode, traps are handled
 * by this procedure.
 */
trap_entry:
	/* Save the a0 register for later in sscratch */
	csrw sscratch, a0
	/* Load the current running process defined in core/sched.c in a0 */
	ld a0, current_proc

	/* Save all registers in the process structure. */
	sd ra, 0(a0)
	sd sp, 8(a0)
	sd gp, 16(a0)
	sd tp, 24(a0)
	sd t0, 32(a0)
	sd t1, 40(a0)
	sd t2, 48(a0)
	sd t3, 56(a0)
	sd t4, 64(a0)
	sd t5, 72(a0)
	sd t6, 80(a0)
	sd s0, 88(a0)
	sd s1, 96(a0)
	sd s2, 104(a0)
	sd s3, 112(a0)
	sd s4, 120(a0)
	sd s5, 128(a0)
	sd s6, 136(a0)
	sd s7, 144(a0)
	sd s8, 152(a0)
	sd s9, 160(a0)
	sd s10, 168(a0)
	sd s11, 176(a0)
	sd a1, 192(a0)
	sd a2, 200(a0)
	sd a3, 208(a0)
	sd a4, 216(a0)
	sd a5, 224(a0)
	sd a6, 232(a0)
	sd a7, 240(a0)

	/* Save the status flags. */
	csrr t0, sstatus
	sd t0, 264(a0)

	/* Save the program counter to restore execution later. */
	csrr t0, sepc
	sd t0, 248(a0)

	/* Restore and save the a0 register as well. */
	csrr t1, sscratch
	sd t1, 184(a0)

	/* Load the kernel stack pointer */
	ld sp, 256(a0)   

	/* Call the trap handler defined in core/riscv64/trap_handler.c */
	call trap_handler

	/* This is the same procedure as before but instead of saving the registers, they get loaded. */
	ld a0, current_proc

	ld t0, 280(a0)
	csrw satp, t0
	sfence.vma zero, zero

	ld t0, 248(a0)
	csrw sepc, t0

	ld t0, 264(a0)
	csrw sstatus, t0

	ld ra, 0(a0)
	ld sp, 8(a0)
	ld gp, 16(a0)
	ld tp, 24(a0)
	ld t0, 32(a0)
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
	ld a1, 192(a0)
	ld a2, 200(a0)
	ld a3, 208(a0)
	ld a4, 216(a0)
	ld a5, 224(a0)
	ld a6, 232(a0)
	ld a7, 240(a0)

	ld a0, 184(a0)

	/* Return to user mode after the process state is restored */
	sret
