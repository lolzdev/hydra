include ../config.mk

SRC:=main.c

ELF:=nvme
OBJS:=$(SRC:.c=.o)

.PHONY: all
all: $(ELF)

.PHONY: $(ELF)
$(ELF): $(OBJS)
	$(LD) -T../libc/user.ld $(LDFLAGS) $(OBJS) ../libc/crt0.o -o $(ELF)

.PHONY: clean
clean:
	@rm -rfv $(OBJS) $(BIN) $(ELF)

%.o: %.c
	$(CC) $(CFLAGS) -Icore/include -c $< -o $@
