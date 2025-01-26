include sources.mk

CC:=clang
LD:=ld.lld

CFLAGS:=-I../libc/include -I../loader/include -I. -g
CFLAGS+=-O0 -Wall -Wextra -g -ffreestanding \
		 -Wno-instantiation-after-specialization \
		 -Wno-int-conversion \
		 -Wno-infinite-recursion \
         -fno-stack-protector \
		 -fno-stack-check \
		 -fno-omit-frame-pointer \
		 -fno-lto \
		 -m64 \
         -march=x86-64 \
		 -mabi=sysv \
		 -mno-80387 \
		 -mno-mmx \
		 -mno-red-zone \
		 -fno-pie \
		 --target=x86_64-pc-none-elf \
         -fno-builtin \
		 -mno-sse \
		 -mno-sse2 \
		 -mno-avx

LDFLAGS = -nostdlib -static -m elf_x86_64 -z max-page-size=0x1000 -T x86_64/linker.ld

BIN:=hydra
OBJS:=$(SRCS:.c=.o)
OBJS:=$(OBJS:.S=.o)
OBJS+=../libc/libc.a

.PHONY: all
all: $(BIN)

.PHONY: $(BIN)
$(BIN): $(OBJS)
	$(LD) $(LDFLAGS) $(OBJS) -o $(BIN)

.PHONY: clean
clean:
	@rm -rfv $(OBJS) $(BIN)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

%.o: %.S
	$(CC) $(CFLAGS) -c $< -o $@
