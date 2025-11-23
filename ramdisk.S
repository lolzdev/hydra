.section .rodata
.align 12
.globl _binary_initramfs_start
.globl _binary_initramfs_end

_binary_initramfs_start:
	.incbin "initramfs.tar"
_binary_initramfs_end:
