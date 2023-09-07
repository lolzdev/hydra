include config/config.mk

DIRS=bootstrap core arch
CONFIGS=${shell realpath config}
INCLUDE=${shell realpath include}
OBJ=${shell find ./ -name "*.o" -print}

export ROOT_DIR=${PWD}

.PHONY: ${DIRS} clean

all: kernel

iso: kernel
	@make -C limine
	@mkdir -pv isodir
	@cp -v config/limine.cfg limine/limine-bios.sys limine/limine-bios-cd.bin limine/limine-uefi-cd.bin isodir/
	@cp -v hydra isodir/
	@mkdir -p isodir/EFI/BOOT
	@cp -v limine/BOOTX64.EFI isodir/EFI/BOOT/
	@cp -v limine/BOOTIA32.EFI isodir/EFI/BOOT/
	@xorriso -as mkisofs -b limine-bios-cd.bin \
        -no-emul-boot -boot-load-size 4 -boot-info-table \
        --efi-boot limine-uefi-cd.bin \
        -efi-boot-part --efi-boot-image --protective-msdos-label \
        isodir -o hydra.iso
	@rm -rfv isodir
	./limine/limine bios-install hydra.iso

kernel: ${DIRS}
	${LD} ${LDFLAGS} -o hydra ${OBJ}

clean: ${DIRS}
	@rm -rfv hydra
	@rm -rfv hydra.iso
	@make -C limine clean

${DIRS}:
ifeq (,$(filter clean,$(MAKECMDGOALS)))
	make -I${CONFIGS} -C $@
else
	make -I${CONFIGS} -C $@ clean
endif

iso: ${DIRS}

distclean: clean
