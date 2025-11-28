include config.mk

SRC:=core/riscv64/bootstrap.S\
     core/main.c\
     core/uart.c\
     core/proc.c\
     core/mem.c\
     core/init.c\
     core/sched.c\
     core/cpu.c\
     core/fdt.c\
     core/spinlock.c\
     core/mm/buddy.c\
     core/riscv64/isa.c\
     core/riscv64/timer.c\
     core/riscv64/vm/vm.c\
     core/riscv64/trap.S\
     core/riscv64/trampoline.S\
     core/riscv64/context_switch.S\
     core/riscv64/trap_handler.c\
     core/riscv64/syscall.c

ELF:=kernel.elf
BIN:=kernel.bin
OBJS:=$(SRC:.c=.o)
OBJS:=$(OBJS:.S=.o)

.PHONY: all
all: $(ELF)

.PHONY: $(ELF)
$(ELF): $(OBJS) initrd
	$(LD) $(LDFLAGS) -Tcore/riscv64/linker.ld $(OBJS) ramdisk.o -o $(ELF)

.PHONY: $(BIN)
$(BIN): $(ELF)
	$(PREFIX)objcopy $(ELF) -O binary $(BIN)

.PHONY: clean
clean:
	@rm -rfv $(OBJS) $(BIN) $(ELF)

.PHONY: qemu
qemu: $(ELF)
	qemu-system-riscv64 -smp 3 -m 256M -M virt -nographic -kernel kernel.elf
.PHONY: debug 
debug: $(ELF)
	qemu-system-riscv64 -smp 1 -m 2G -M virt -nographic -kernel kernel.elf -s -S

.PHONY: initrd
initrd: libc
	@make -C vfs
	@make -C nvme
	@mkdir initrd
	@cp vfs/vfs nvme/nvme initrd/
	tar -cf initramfs.tar -C initrd/ vfs nvme
	@rm -rf initrd
	$(CC) -c ramdisk.S -o ramdisk.o
	@rm -rf initramfs.tar

.PHONY: libc
libc:
	@make -C libc

%.o: %.c
	$(CC) $(CFLAGS) -Icore/include -c $< -o $@

%.o: %.S
	$(CC) $(CFLAGS) -c $< -o $@
