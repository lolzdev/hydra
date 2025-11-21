include config.mk

SRC:=core/riscv64/bootstrap.S\
     core/main.c\
     core/uart.c\
     core/mm/buddy.c\
     core/riscv64/vm/vm.c

ELF:=kernel.elf
BIN:=kernel.bin
OBJS:=$(SRC:.c=.o)
OBJS:=$(OBJS:.S=.o)

.PHONY: all
all: $(ELF)

.PHONY: $(ELF)
$(ELF): $(OBJS)
	$(LD) $(LDFLAGS) -Tcore/riscv64/linker.ld $(OBJS) -o $(ELF)

.PHONY: $(BIN)
$(BIN): $(ELF)
	$(PREFIX)objcopy $(ELF) -O binary $(BIN)

.PHONY: clean
clean:
	@rm -rfv $(OBJS) $(BIN) $(ELF)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

%.o: %.S
	$(CC) $(CFLAGS) -c $< -o $@
