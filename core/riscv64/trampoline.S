.section .data.trampoline
.align 12
global_pt_l2:
    .zero 4096

.section .text.trampoline
.global _start
_start:
    lla t0, global_pt_l2

    li t1, 0x80000000
    li t3, 0xcf

    srli t4, t1, 12
    slli t4, t4, 10
    or t5, t4, t3

    li t6, 2
    slli t6, t6, 3
    add t6, t0, t6
    sd t5, 0(t6)

    li t6, 510
    slli t6, t6, 3
    add t6, t0, t6
    sd t5, 0(t6)

    li t4, 0
    slli t4, t4, 10
    or t5, t4, t3
    sd t5, 0(t0)

    srli t6, t0, 12
    li t1, 8
    slli t1, t1, 60
    or t6, t6, t1

    csrw satp, t6
    sfence.vma

    lla ra, _entry
    jr ra
