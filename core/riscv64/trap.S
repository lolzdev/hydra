.section .text
.align 4
.globl trap_entry
.extern current_proc
/*
 * When in user mode, traps are handled
 * by this procedure.
 */
trap_entry:
	/* Save the a0 register for later in sscratch and load the current running process frame */
	csrrw a0, sscratch, a0

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

	csrw sscratch, a0

	/* Load the kernel stack pointer */
	ld sp, 256(a0)

	ld tp, 272(a0)

	la t0, kernel_trap_entry
	csrw stvec, t0

	/* Call the trap handler defined in core/riscv64/trap_handler.c */
	call trap_handler

	la t0, trap_entry
	csrw stvec, t0

	/* This is the same procedure as before but instead of saving the registers, they get loaded. */
	csrrw a0, sscratch, a0

	ld t0, 288(a0)
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

.globl kernel_trap_entry
.align 4
kernel_trap_entry:
	addi sp, sp, -256

	sd ra, 0(sp)
	sd sp, 8(sp)
	sd gp, 16(sp)
	sd tp, 24(sp)
	sd t0, 32(sp)
	sd t1, 40(sp)
	sd t2, 48(sp)
	sd s0, 56(sp)
	sd s1, 64(sp)
	sd a0, 72(sp)
	sd a1, 80(sp)
	sd a2, 88(sp)
	sd a3, 96(sp)
	sd a4, 104(sp)
	sd a5, 112(sp)
	sd a6, 120(sp)
	sd a7, 128(sp)
	sd s2, 136(sp)
	sd s3, 144(sp)
	sd s4, 152(sp)
	sd s5, 160(sp)
	sd s6, 168(sp)
	sd s7, 176(sp)
	sd s8, 184(sp)
	sd s9, 192(sp)
	sd s10, 200(sp)
	sd s11, 208(sp)
	sd t3, 216(sp)
	sd t4, 224(sp)
	sd t5, 232(sp)
	sd t6, 240(sp)

	call trap_handler

	ld ra, 0(sp)
	ld gp, 16(sp)
	ld tp, 24(sp)
	ld t0, 32(sp)
	ld t1, 40(sp)
	ld t2, 48(sp)
	ld s0, 56(sp)
	ld s1, 64(sp)
	ld a0, 72(sp)
	ld a1, 80(sp)
	ld a2, 88(sp)
	ld a3, 96(sp)
	ld a4, 104(sp)
	ld a5, 112(sp)
	ld a6, 120(sp)
	ld a7, 128(sp)
	ld s2, 136(sp)
	ld s3, 144(sp)
	ld s4, 152(sp)
	ld s5, 160(sp)
	ld s6, 168(sp)
	ld s7, 176(sp)
	ld s8, 184(sp)
	ld s9, 192(sp)
	ld s10, 200(sp)
	ld s11, 208(sp)
	ld t3, 216(sp)
	ld t4, 224(sp)
	ld t5, 232(sp)
	ld t6, 240(sp)

	addi sp, sp, 256

	sret
