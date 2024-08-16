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

#include <elf.h>
#include <alloc.h>
#include <string.h>
#include <filesystem.h>

size_t utf8_char_to_wchar(const char *utf8_str, CHAR16 *wchar_str);
extern EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL *stdout;
extern EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL *stderr;

elf_result elf_check(uint8_t *buffer)
{
	elf_header_t *header = (elf_header_t *) buffer;

	return (ELF_CHECK_MAGIC(&header->magic) && header->bits == 2 && header->version == 1) ? ELF_OK : ELF_INVALID;
}

elf_result elf_map(uint8_t *buffer, elf_image *image)
{
	elf_result status = elf_check(buffer);
	if (status != ELF_OK) return status;
	elf_header_t *header = (elf_header_t *) buffer;

	elf_program_header_t *p_headers = (elf_program_header_t *) (buffer + header->p_header);
	
	image->memmap_count = header->ph_num;
	image->memmap = (struct elf_memmap_entry *) malloc(sizeof(struct elf_memmap_entry) * header->ph_num);

	for (uint32_t i=0; i < header->ph_num; i++) {
		elf_program_header_t p_header = p_headers[i];
		image->memmap[i].start = p_header.vaddr;
		image->memmap[i].end = p_header.vaddr + p_header.mem_size;
		image->memmap[i].type = p_header.type;
		if (p_header.type == ELF_PT_LOAD) {
			void *seg = (void *)((uint64_t) buffer + p_header.offset);
			void *vaddr = (void *) p_header.vaddr;
			uint64_t seg_size = p_header.file_size;
			uint64_t mem_size = p_header.mem_size;
			uint64_t bss = mem_size - seg_size;
			if (bss > 0) {
				memset((void *)((uint64_t)vaddr + seg_size), 0x0, bss);
			}

			memcpy(vaddr, seg, seg_size);
		}
	}

	image->tags = NULL;
	elf_section_header_t *sec_headers = (elf_section_header_t *) ((uint64_t)buffer + header->s_header);
	uint64_t names = ((uint64_t)buffer + sec_headers[header->shstrndx].offset);
	for (uint32_t i=0; i < header->sh_num; i++) {
		char *name = (char *) (names + sec_headers[i].name);

		if (strcmp(name, ".hydra_tags") == 0) {
			image->tags = (void *) sec_headers[i].vaddr;
		}		
		
	}

	image->entry = (void *) header->entry;

	return ELF_OK;
}
