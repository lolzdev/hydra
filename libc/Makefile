CC:=clang
LD:=lld
CFLAGS:=-ffreestanding -mno-red-zone -std=c11 -Iinclude \
	-Werror -Wall \
	-fno-builtin

ifeq ($(MAKECMDGOALS),efi)
	CFLAGS+=-target x86_64-unknown-windows
else
	CFLAGS+=-target x86_64-unknown-elf
endif

SRCS:=string.c
OBJS:=$(SRCS:.c=.o)
OBJS_EFI:=$(SRCS:.c=.o.efi)


.PHONY: all
all: libc.a

%.o: %.c
	@echo "[CC] $< > $@"
	@$(CC) $(CFLAGS) -c $< -o $@

%.o.efi: %.c
	@echo "[CC] $< > $@"
	@$(CC) $(CFLAGS) -c $< -o $@

.PHONY: clean
clean:
	@rm -rfv $(OBJS) *.a $(OBJS_EFI)

.PHONY: libc.a
libc.a: $(OBJS)
	@echo "[AR] $(OBJS) > libc.a"
	@ar r $@ $(OBJS)

.PHONY: libc-efi.a
libc-efi.a: $(OBJS_EFI)
	@echo "[AR] $(OBJS_EFI) > libc-efi.a"
	@ar r $@ $(OBJS_EFI)

.PHONY: efi
efi: libc-efi.a
