.section .text.entry
.global _start
.extern main
.extern __bss_start
.extern __bss_end
.extern __global_pointer$

_start:
.option push
.option norelax
	la gp, __global_pointer$
.option pop

	la t0, __bss_start
	la t1, __bss_end
	bge t0, t1, bss_done

bss_loop:
	sd zero, 0(t0)
	addi t0, t0, 8
	blt t0, t1, bss_loop
bss_done:
	li a0, 0
	li a1, 0

	call main
loop:
j loop
