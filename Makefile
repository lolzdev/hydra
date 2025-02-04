PREFIX=riscv64-unknown-elf-
CC=$(PREFIX)gcc
LD=$(PREFIX)ld

SRC:=sys/bootstrap.S \
	 sys/main.c \
	 sys/uart.c

ELF:=kernel.elf
BIN:=kernel.bin
OBJS:=$(SRC:.c=.o)
OBJS:=$(OBJS:.S=.o)

CFLAGS:=-Wall -Wextra -O -ggdb -gdwarf-2 -mcmodel=medany
CFLAGS+=-I./sys/include
LDFLAGS=-z max-page-size=4096

.PHONY: all
all: $(BIN)

.PHONY: $(ELF)
$(ELF): $(OBJS)
	$(LD) $(LDFLAGS) -Tsys/linker.ld $(OBJS) -o $(ELF)

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
