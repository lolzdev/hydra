	/*-
	 * Copyright 2025 Lorenzo Torres
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

#ifndef ELF_H
#define ELF_H

#include <stdint.h>
#include <stddef.h>

#define ELF_MAGIC0 0x7f
#define ELF_MAGIC1 'E'
#define ELF_MAGIC2 'L'
#define ELF_MAGIC3 'F'

#define ELF_CHECK_MAGIC(magic) \
	((((uint8_t *)magic)[0] == ELF_MAGIC0) && \
	(((uint8_t *)magic)[1] == ELF_MAGIC1) && \
	(((uint8_t *)magic)[2] == ELF_MAGIC2) && \
	(((uint8_t *)magic)[3] == ELF_MAGIC3))


// ELF file type
#define ELF_TYPE_NONE 0x0
#define ELF_TYPE_REL 0x1
#define ELF_TYPE_EXEC 0x2
#define ELF_TYPE_DYN 0x3
#define ELF_TYPE_CORE 0x4
#define ELF_TYPE_LOOS 0xFE00
#define ELF_TYPE_HIOS 0xFEFF
#define ELF_TYPE_LOPROC 0xFF00
#define ELF_TYPE_HIPROC 0xFFFF

// ELF program header type
#define ELF_PT_NULL 0x0
#define ELF_PT_LOAD 0x1
#define ELF_PT_DYNAMIC 0x2
#define ELF_PT_INTERP 0x3
#define ELF_PT_NOTE 0x4
#define ELF_PT_SHLIB 0x5
#define ELF_PT_PHDR 0x6
#define ELF_PT_TLS 0x7
#define ELF_PT_LOOS 0x60000000
#define ELF_PT_HIOS 0x6fffffff
#define ELF_PT_LOPROC 0x70000000
#define ELF_PT_HIPROC 0x7fffffff

// ELF program header segment type
#define ELF_PF_X 0x1
#define ELF_PF_W 0x2
#define ELF_PF_R 0x3

#define SHT_NULL 			0x0
#define SHT_PROGBITS		0x1
#define SHT_SYMTAB 			0x2
#define SHT_STRTAB 			0x3
#define SHT_RELA 			0x4
#define SHT_HASH 			0x5
#define SHT_DYNAMIC 		0x6
#define SHT_NOTE 			0x7
#define SHT_NOBITS			0x8
#define SHT_REL				0x9
#define SHT_SHLIB 			0x0A
#define SHT_DYNSYM 			0x0B
#define SHT_INIT_ARRAY		0x0E
#define SHT_FINI_ARRAY		0x0F
#define SHT_PREINIT_ARRAY 	0x10
#define SHT_GROUP 			0x11
#define SHT_SYMTAB_SHNDX 	0x12
#define SHT_NUM				0x13
#define SHT_LOOS			0x60000000

#define SHF_WRITE 	0x1
#define SHF_ALLOC 	0x2 	
#define SHF_EXECINSTR 	0x4 	
#define SHF_MERGE 	0x10 	
#define SHF_STRINGS 	0x20 	
#define SHF_INFO_LINK 	0x40 	
#define SHF_LINK_ORDER 	0x80 	
#define SHF_OS_NONCONFORMING 	0x100 	
#define SHF_GROUP 	0x200 	
#define SHF_TLS 	0x400 	
#define SHF_MASKOS 	0x0FF00000 	
#define SHF_MASKPROC 	0xF000000
#define SHF_ORDERED 	0x4000000 	
#define SHF_EXCLUDE 	0x8000000

#define ELF_OK 0x0
#define ELF_INVALID 0x1
#define ELF_ERROR 0x2

typedef uint8_t elf_result;

enum ElfABI 
{
	SysV = 0x0,
	HPUX = 0x1,
	NetBSD = 0x2,
	Linux = 0x3,
	GNUHurd = 0x4,
	Solaris = 0x6,
	AIX = 0x7,
	IRIX = 0x8,
	FreeBSD = 0x9,
	Tru64 = 0x0A,
	OpenBSD = 0x0c,
	OpenVMS = 0x0d,
	NonStop = 0x0E,
	AROS = 0x0F,
	FenixOS = 0x10,
	Nuxi = 0x11,
	OpenVOS = 0x12,
};

typedef struct elf_header {
	uint32_t magic;
	uint8_t bits; // 32 (1) or 64 (2)
	uint8_t endianness; // little (1) or big (2)
	uint8_t version;
	uint8_t abi; // see ElfABI
	uint8_t abi_version;
	uint8_t padding[7]; // reserved
	uint16_t type;
	uint16_t isa;
	uint32_t version1;
	uint64_t entry; // program entry point
	uint64_t p_header; // program header offset
	uint64_t s_header; // section header offset
	uint32_t flags;
	uint16_t h_size; // size of this header
	uint16_t ph_size; // size of program header
	uint16_t ph_num; // number of program header table entries
	uint16_t she_size; // section header table entry size
	uint16_t sh_num; // number of section header table entries
	uint16_t shstrndx; // index of the section header table entry containing the section names
} elf_header_t;

typedef struct elf_program_header {
	uint32_t type;
	uint32_t flags;
	uint64_t offset;
	uint64_t vaddr;
	uint64_t paddr;
	uint64_t file_size;
	uint64_t mem_size;
	uint64_t align;
} elf_program_header_t;

typedef struct elf_section_header {
	uint32_t name;
	uint32_t type;
	uint64_t flags;
	uint64_t vaddr;
	uint64_t offset;
	uint64_t size;
	uint32_t link;
	uint32_t info;
	uint64_t addralign;
	uint64_t entsize;
} elf_section_header_t;

struct elf_memmap_entry {
	uint64_t start, end;
	uint8_t type;
};

#endif
