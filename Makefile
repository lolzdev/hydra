include config/config.mk

DIRS=bootstrap core arch
CONFIGS=${shell realpath config}
INCLUDE=${shell realpath include}
OBJ=${shell find ./ -name "*.o" -print}

export ROOT_DIR=${PWD}

.PHONY: ${DIRS} clean

all: kernel

iso: kernel
	@mkdir -pv isodir/boot/grub
	@cp -v config/grub.cfg isodir/boot/grub/grub.cfg
	@cp -v hydra isodir/boot/hydra
	@grub-mkrescue -o hydra.iso isodir
	@rm -rfv isodir

kernel: ${DIRS}
	${LD} ${LDFLAGS} -o hydra ${OBJ}

clean: ${DIRS}
	@rm -rfv hydra
	@rm -rfv hydra.iso

${DIRS}:
ifeq (,$(filter clean,$(MAKECMDGOALS)))
	make -I${CONFIGS} -C $@
else
	make -I${CONFIGS} -C $@ clean
endif

iso: ${DIRS}

distclean: clean
