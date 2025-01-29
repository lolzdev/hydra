CC:=clang
LD=ld.lld

CFLAGS:=-O0 -Wall -Wextra -g -ffreestanding
LDFLAGS = -nostdlib -m elf_x86_64 -z max-page-size=0x1000

all:
	$(CC) $(CFLAGS) -c test.S -o test.o
	$(LD) $(LDFLAGS) test.o -o test
