
LDFLAGS:=-T${LD_SCRIPT}\
		 -ffreestanding\
		 -O2\
		 -nostdlib\
		 -target ${TGT_TRIPLE}\
		 -z noexecstack

SRCS:=kmain.c \
	  boot.S

OBJS:=${SRCS:.S=.o}
OBJS:=${OBJS:.c=.o}

all: build

build: ${OBJS}
	${CC} ${LDFLAGS} ${CFLAGS} ${OBJS} -o ${BIN}

%.o: %.S %.c
	${CC} -target ${TGT_TRIPLE} -c $@ -o $<

isodir:
	@mkdir -pv iso/boot/grub

iso: isodir build
	@cp -v ${BIN} iso/boot/
	@cp -v grub/grub.cfg iso/boot/
	@grub-mkrescue -o hydra.iso iso

.PHONY: clean
clean:
	@echo "removing objects"
	@rm -rfv ${OBJS}
	@rm -rfv ${BIN}
	@rm -rfv iso hydra.iso

include config.mk
