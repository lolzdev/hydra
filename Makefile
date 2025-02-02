OVMF_URL := https://retrage.github.io/edk2-nightly/bin/RELEASEX64_OVMF.fd
RUN := run
OVMF := run/ovmf.fd
LIMINE_DOWNLOAD := limine

.PHONY: all
all: limine_build sys

limine_build: $(LIMINE_DOWNLOAD)
	@$(MAKE) -C limine

.PHONY: sys
sys: libc
	@$(MAKE) -C sys

$(RUN):
	@mkdir -v run

$(LIMINE_DOWNLOAD):
	@echo "Cloning limine..."
	git clone https://github.com/limine-bootloader/limine.git --branch=v8.x-binary --depth=1

$(OVMF): $(RUN)
	@echo "Downloading OVMF..."
	@curl -Ls $(OVMF_URL) -o $(OVMF)

.PHONY: libc
libc:
	@$(MAKE) -C libc
	@$(MAKE) -C libc efi

.PHONY: image
image: all 
	@mkdir -p iso_root/boot; \
	cp sys/hydra iso_root/boot/; \
	cp tests/test_proc/test iso_root/boot/; \
	cp tests/loop/loop iso_root/boot/; \
	mkdir -p iso_root/boot/limine; \
	cp -v limine.conf limine/limine-bios.sys limine/limine-bios-cd.bin limine/limine-uefi-cd.bin iso_root/boot/limine/; \
	mkdir -p iso_root/EFI/BOOT; \
	cp -v limine/BOOTX64.EFI iso_root/EFI/BOOT/; \
	cp -v limine/BOOTIA32.EFI iso_root/EFI/BOOT/; \
	xorriso -as mkisofs -R -r -J -b boot/limine/limine-bios-cd.bin \
        -no-emul-boot -boot-load-size 4 -boot-info-table -hfsplus \
        -apm-block-size 2048 --efi-boot boot/limine/limine-uefi-cd.bin \
        -efi-boot-part --efi-boot-image --protective-msdos-label \
        iso_root -o image.iso; \
	./limine/limine bios-install image.iso; \
	rm -rf iso_root

.PHONY: test
test: $(OVMF) image
	@qemu-system-x86_64 -no-shutdown -no-reboot -d cpu_reset -m 2G -drive if=pflash,format=raw,readonly=on,file=$(OVMF) image.iso -monitor stdio

.PHONY: debug
debug: $(OVMF) image
	@qemu-system-x86_64 -no-shutdown -no-reboot -d int -s -S -m 2G -drive if=pflash,format=raw,readonly=on,file=$(OVMF) image.iso -debugcon stdio

.PHONY: gdb
gdb:	
	@gdb -x gdb.txt sys/hydra

.PHONY: clean
clean:
	@rm -rfv run image.iso mnt limine
	@$(MAKE) -C libc clean
	@$(MAKE) -C sys clean
