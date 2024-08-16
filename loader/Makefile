CC := clang
LD := lld
CFLAGS := -ffreestanding -mno-red-zone -std=c11 -I. -I../libc/include -I./include \
	-target x86_64-unknown-windows \
	-Werror -Wall \
	-fno-builtin \
	-mno-stack-arg-probe \
	-g
LDFLAGS := -flavor link -subsystem:efi_application -entry:efi_main
SRCS := loader.c \
		alloc.c \
		filesystem.c \
		elf.c \
		main.c
OBJS := $(SRCS:.c=.o)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

.PHONY: BOOTX64.EFI
BOOTX64.EFI: $(OBJS)
	$(LD) $(LDFLAGS) $(OBJS) libc-efi.a -out:$@

.PHONY: clean
clean:
	@rm -rfv BOOTX64.EFI $(OBJS) *.a

.PHONY: all
all: BOOTX64.EFI


