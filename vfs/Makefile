include ../config.mk

SRC:=main.c

ELF:=vfs
OBJS:=$(SRC:.c=.o)

.PHONY: all
all: $(ELF)

.PHONY: $(ELF)
$(ELF): $(OBJS)
	$(LD) $(LDFLAGS) $(OBJS) -o $(ELF)

.PHONY: clean
clean:
	@rm -rfv $(OBJS) $(BIN) $(ELF)

%.o: %.c
	$(CC) $(CFLAGS) -Icore/include -c $< -o $@
