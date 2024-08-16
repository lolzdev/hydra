OVMF_URL := https://retrage.github.io/edk2-nightly/bin/RELEASEX64_OVMF.fd
RUN := run
OVMF := run/ovmf.fd

.PHONY: all
all: loader sys

.PHONY: sys
sys: libc
	@make -C sys

$(RUN):
	@mkdir -v run

$(OVMF): $(RUN)
	@echo "Downloading OVMF..."
	@curl -Ls $(OVMF_URL) -o $(OVMF)

.PHONY: libc
libc:
	@make -C libc
	@make -C libc efi

.PHONY: loader
loader: libc
	@cp libc/libc-efi.a loader/
	@make -C loader


.PHONY: image
image: all
	@dd if=/dev/zero of=boot.img bs=1M count=64; \
	mkfs.fat -F 32 -n EFI_SYSTEM boot.img; \
	mkdir -p mnt; \
	sudo mount -o loop boot.img mnt; \
	sudo mkdir -p mnt/EFI/BOOT; \
	sudo cp loader/BOOTX64.EFI mnt/EFI/BOOT/BOOTX64.EFI; \
	sudo mkdir -p mnt/boot/kernel; \
	sudo cp sys/hydra mnt/boot/kernel/hydra; \
	sudo umount mnt; \
	rm -rf mnt

.PHONY: test
test: $(OVMF) image
	@qemu-system-x86_64 -m 2G -drive if=pflash,format=raw,readonly=on,file=$(OVMF) -drive if=ide,format=raw,file=boot.img -debugcon stdio

.PHONY: debug
debug: $(OVMF) image
	@qemu-system-x86_64 -s -S -m 2G -drive if=pflash,format=raw,readonly=on,file=$(OVMF) -drive if=ide,format=raw,file=boot.img -debugcon stdio

.PHONY: gdb
gdb:	
	@gdb -x gdb.txt sys/hydra

.PHONY: clean
clean:
	@rm -rfv run boot.img mnt
	@make -C loader clean
	@make -C libc clean
	@make -C sys clean
