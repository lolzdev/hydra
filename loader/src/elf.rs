extern crate alloc;

#[repr(C)]
#[derive(Debug, Clone, Copy, bytemuck::Pod, bytemuck::Zeroable)]
struct Header {
	magic: u32, 
	bits: u8, 
	endianness: u8, 
	version: u8,
	abi: u8,
	abi_version: u8 ,
	padding: [u8; 7], // reserved
	ty: u16,
	isa: u16 ,
	version1: u32,
	entry: u64, // program entry point
	p_header: u64, // program header offset
	s_header: u64, // section header offset
	flags: u32,
	h_size: u16, // size of this header
	ph_size: u16, // size of program header
	ph_num: u16, // number of program header table entries
	she_size: u16, // section header table entry size
	sh_num: u16, // number of section header table entries
	shstrndx: u16, // index of the section header table entry containing the section names
}

#[repr(C)]
struct ProgramHeader {
    ty: u32 ,
	flags: u32 ,
	offset: u64 ,
	vaddr: u64 ,
	paddr: u64 ,
	file_size: u64 ,
	mem_size: u64 ,
	align: u64 ,
}

#[repr(C)]
struct SectionHeader {
    name: u32 ,
	ty: u32 ,
	flags: u64 ,
	vaddr: u64 ,
	offset: u64 ,
	size: u64 ,
	link: u32 ,
	info: u32 ,
	addralign: u64 ,
	entsize: u64 ,
}

fn check_magic(magic: u32) -> bool {
    let data: [u8; 4] = bytemuck::cast(magic);
    data[0] == 0x7f && data[1] == 'E' as u8 && data[2] == 'L' as u8 && data[3] == 'F' as u8
}

fn elf_load(data: alloc::vec::Vec<u8>) {
    let header: &Header = bytemuck::from_bytes(&data); 
}
