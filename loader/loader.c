	/*-
	 * Copyright 2024 Lorenzo Torres
 	 *
	 * Redistribution and use in source and binary forms, with or without
	 * modification, are permitted provided that the following conditions are met:
	 * 1. Redistributions of source code must retain the above copyright
	 *    notice, this list of conditions and the following disclaimer.
	 * 2. Redistributions in binary form must reproduce the above copyright
	 *    notice, this list of conditions and the following disclaimer in the
	 *    documentation and/or other materials provided with the distribution.
	 * 3. Neither the name of the copyright holder nor
	 *    the names of its contributors may be used to endorse or promote products
	 *    derived from this software without specific prior written permission.
	 *
	 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS “AS IS” AND ANY
	 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
	 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
	 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY
	 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
	 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
	 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
	 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
	 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
	 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
	 */

#include <loader.h>
#include <efi/types.h>
#include <string.h>
#include <filesystem.h>
#include <alloc.h>
#include <fb.h>
#include <tags.h>

extern EFI_HANDLE *image_handle;
extern EFI_SYSTEM_TABLE *system_table;
extern EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL *stdout;
extern EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL *stderr;

void *kernel_read(CHAR16 *path)
{
	CHAR16 kernel_path[256];
    CHAR16 *p;

    memcpy(kernel_path, path, sizeof(kernel_path) - 1);
    kernel_path[(sizeof(kernel_path)/2) - 1] = L'\0';

    for (p = kernel_path; *p; ++p) {
        if (*p == L'/') {
            *p = L'\\';
        }
    }

    file_t kernel = sfs_open(kernel_path, EFI_FILE_MODE_READ);
    if (kernel.status != 0)
    {
        stdout->OutputString(stdout, L"Failed to open kernel binary: File not found\n");
        for (;;)
            ;
    }

	void *buffer = (void *) malloc(kernel.info.PhysicalSize);

    sfs_read(&kernel, buffer);

    if (buffer == NULL) {
        stdout->OutputString(stdout, L"Error: Failed to allocate memory for kernel data buffer\n");
        for (;;)
            ;
    }

    if (kernel.status != 0) {
        stdout->OutputString(stdout, L"Error: Failed to read kernel data\n");
        for (;;)
            ;
        ;
    }

    sfs_close(&kernel);
    free(kernel_path);

	return buffer;
}


EFI_STATUS efi_memory_map(UINTN *key, EFI_MEMORY_DESCRIPTOR **descs, UINTN *desc_size, UINT32 *version)
{
	UINTN *size = NULL;
	system_table->BootServices->GetMemoryMap(size, NULL, NULL, NULL, NULL);
	*descs = (EFI_MEMORY_DESCRIPTOR *) malloc(*size);
	return system_table->BootServices->GetMemoryMap(size, *descs, key, desc_size, version);
}

void kernel_memory_map(elf_image *img, struct hydra_memmap **memmap, uint64_t *size)
{
	EFI_MEMORY_DESCRIPTOR *descs = NULL;
	UINTN *key = NULL;
	UINTN desc_size;
	UINT32 version;
	efi_memory_map(key, &descs, &desc_size, &version);
	*size /= sizeof(EFI_MEMORY_DESCRIPTOR);
	*memmap = (struct hydra_memmap *) malloc(sizeof(struct hydra_memmap) * (*size + img->memmap_count));
	
	for (uint32_t i=0; i < *size; i++) {
		(*memmap)[i].phys_start = descs[i].PhysicalStart;
		(*memmap)[i].virt_start = descs[i].VirtualStart;
		(*memmap)[i].pages = descs[i].NumberOfPages;
		(*memmap)[i].type = HYDRA_MEMMAP_RESERVED;
	}

	for (uint32_t i=0; i < img->memmap_count; i++) {
		(*memmap)[i].phys_start = img->memmap[i+*size].start;
		(*memmap)[i].virt_start = img->memmap[i+*size].start;
		(*memmap)[i].pages = (img->memmap[i+*size].end - img->memmap[i+*size].end) / 0x1000;
		(*memmap)[i].type = HYDRA_MEMMAP_KERNEL;
	}

	*size += img->memmap_count;

	free(descs);
}

void kernel_tags(elf_image *img)
{
	void *tags = img->tags;

	while (tags != NULL) {
		uint8_t type = ((uint8_t *) tags)[0];

		switch (type) {
			case HYDRA_TAG_MEMMAP_TYPE: {
				struct hydra_memmap *memmap = NULL;
				uint64_t size = 0;
				kernel_memory_map(img, &memmap, &size);
				struct hydra_tag_memmap *tag = (struct hydra_tag_memmap *) tags;

				tag->memmap_count = size;
				tag->memmap = memmap;

				tags += sizeof(struct hydra_tag_memmap);
				stdout->OutputString(stdout, L"Written tag\n\r");
				break;
			}
			case HYDRA_TAG_FRAMEBUFFER_TYPE: {
				hydra_framebuffer_t *addr;
				fb_get_all(&addr);
				struct hydra_tag_framebuffer *tag = (struct hydra_tag_framebuffer *) tags;
				
				tag->framebuffers = addr;
				tags += sizeof(struct hydra_tag_framebuffer);
				stdout->OutputString(stdout, L"Written tag\n\r");

				break;								 
			}
			default:
				return;
		}
	}
}

EFI_STATUS kernel_load(CHAR16 *path, elf_image *img)
{
    void *buffer = kernel_read(path);

	elf_result res = elf_check(buffer);
	if (res == ELF_OK) {
        stdout->OutputString(stdout, L"Valid ELF file\n\r");
	} else {
        stdout->OutputString(stdout, L"Invalid ELF file\n\r");
	}

	res = elf_map(buffer, img);
	if (res != ELF_OK) {
        stdout->OutputString(stdout, L"Can't load ELF file\n\r");
	} else {
        stdout->OutputString(stdout, L"ELF file loaded in memory\n\r");
	}
    
    free(buffer);

	kernel_tags(img);

	return EFI_SUCCESS;
}

EFI_STATUS kernel_exit_uefi(void)
{
	EFI_STATUS status;
    EFI_MEMORY_DESCRIPTOR tmp_mmap[1];
	UINTN efi_mmap_size = sizeof(tmp_mmap), efi_desc_size = 0;
    UINTN mmap_key = 0;
	EFI_MEMORY_DESCRIPTOR *efi_mmap = NULL;
	UINT32 efi_desc_ver = 0;

	stdout->ClearScreen(stdout);

    system_table->BootServices->GetMemoryMap(&efi_mmap_size, tmp_mmap, &mmap_key, &efi_desc_size, &efi_desc_ver);
    efi_mmap_size += 4096;	

	status = system_table->BootServices->AllocatePool(EfiLoaderData, efi_mmap_size, (void **)&efi_mmap);
	if (status) return status;

	status = system_table->BootServices->GetMemoryMap(&efi_mmap_size, efi_mmap, &mmap_key, &efi_desc_size, &efi_desc_ver);
	status = system_table->BootServices->ExitBootServices(*image_handle, mmap_key);

	return status;
}
