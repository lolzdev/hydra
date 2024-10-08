extern crate alloc;
use log::info;

#[derive(Debug, Clone)]
pub struct Image {
    pub entry: extern "sysv64" fn(),
}

#[repr(C)]
#[derive(Debug, Clone, Copy, bytemuck::Pod, bytemuck::Zeroable)]
struct Header {
    magic: u32,
    bits: u8,
    endianness: u8,
    version: u8,
    abi: u8,
    abi_version: u8,
    padding: [u8; 7], // reserved
    ty: u16,
    isa: u16,
    version1: u32,
    entry: u64,    // program entry point
    p_header: u64, // program header offset
    s_header: u64, // section header offset
    flags: u32,
    h_size: u16,   // size of this header
    ph_size: u16,  // size of program header
    ph_num: u16,   // number of program header table entries
    she_size: u16, // section header table entry size
    sh_num: u16,   // number of section header table entries
    shstrndx: u16, // index of the section header table entry containing the section names
}

#[repr(C)]
#[derive(Debug, Clone, Copy, bytemuck::Pod, bytemuck::Zeroable)]
struct ProgramHeader {
    ty: u32,
    flags: u32,
    offset: u64,
    vaddr: u64,
    paddr: u64,
    file_size: u64,
    mem_size: u64,
    align: u64,
}

#[repr(C)]
#[derive(Debug, Clone, Copy, bytemuck::Pod, bytemuck::Zeroable)]
struct SectionHeader {
    name: u32,
    ty: u32,
    flags: u64,
    vaddr: u64,
    offset: u64,
    size: u64,
    link: u32,
    info: u32,
    addralign: u64,
    entsize: u64,
}

fn check_magic(magic: u32) -> bool {
    let data: [u8; 4] = bytemuck::cast(magic);
    data[0] == 0x7f && data[1] == 'E' as u8 && data[2] == 'L' as u8 && data[3] == 'F' as u8
}

pub fn elf_load(data: alloc::vec::Vec<u8>) -> Image {
    let header: &Header = bytemuck::from_bytes(&data.as_slice()[..core::mem::size_of::<Header>()]);

    let program_headers_bytes = &data.as_slice()[(header.p_header as usize)
        ..(header.ph_num as usize * core::mem::size_of::<ProgramHeader>())];
    let len = program_headers_bytes.len();
    let ptr = program_headers_bytes.as_ptr() as *const ProgramHeader;
    let program_headers: &[ProgramHeader] =
        unsafe { core::slice::from_raw_parts(ptr, len / core::mem::size_of::<ProgramHeader>()) };

    for &hdr in program_headers {
        if hdr.ty == 0x1 {
            let seg_size = hdr.file_size as usize;
            let code: &[u8] = &data.as_slice()[hdr.offset as usize..hdr.offset as usize + seg_size];
            let mut mem =
                unsafe { core::slice::from_raw_parts_mut(hdr.vaddr as *mut u8, seg_size) };
            mem.copy_from_slice(code);
        }
    }

    let ptr = header.entry as *const ();
    let entry: extern "sysv64" fn() = unsafe { core::mem::transmute(ptr) };
    Image { entry }
}
