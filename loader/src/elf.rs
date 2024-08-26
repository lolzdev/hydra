#[repr(u8)]
enum FileType {
    TypeNone = 0x0,
    TypeRelocatable = 0x1,
    TypeExecutable = 0x2,
    TypeDynamic = 0x3,
    TypeCore = 0x4,
    TypeLowOs = 0xfe00,
    TypeHighOs = 0xfeff,
    TypeLowProc = 0xff00,
    TypeHighProc = 0xffff,
}

#[repr(u8)]
enum HeaderType {
    Null = 0x0
    Load = 0x1
    Dynamic = 0x2
    Interp = 0x3
    Note = 0x4
    Shlib = 0x5
    Phdr = 0x6
    Tls = 0x7
    LowOs = 0x60000000
    HighOs = 0x6fffffff
    LowProc = 0x70000000
    HighProc = 0x7fffffff
}

#[repr(u8)]
enum ProgramSegmentType {
    Executable = 0x1,
    Readable = 0x3,
    Writable = 0x2,
}

#[repr(u8)]
enum Abi
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
}

struct Header {
	u32 magic;
    /// 32 (1) or 64 (2)
	u8 bits; 
    /// little (1) or big (2)
	u8 endianness; 
	u8 version;
	ElfABI abi;
	u8 abi_version;
	u8 padding[7]; // reserved
	u16 ty;
	u16 isa;
	u32 version1;
	u64 entry; // program entry point
	u64 p_header; // program header offset
	u64 s_header; // section header offset
	u32 flags;
	u16 h_size; // size of this header
	u16 ph_size; // size of program header
	u16 ph_num; // number of program header table entries
	u16 she_size; // section header table entry size
	u16 sh_num; // number of section header table entries
	u16 shstrndx; // index of the section header table entry containing the section names
}

#[repr(C)]
struct ProgramHeader {
    u32 ty;
	u32 flags;
	u64 offset;
	u64 vaddr;
	u64 paddr;
	u64 file_size;
	u64 mem_size;
	u64 align;
}

#[repr(C)]
struct SectionHeader {
    u32 name;
	u32 ty;
	u64 flags;
	u64 vaddr;
	u64 offset;
	u64 size;
	u32 link;
	u32 info;
	u64 addralign;
	u64 entsize;
}

fn check_magic(magic: u32) -> bool {
    let data: [u8; 4] = bytemuck::cast(magic);
    data[0] == 0x7f && data[1] == 'E' && data[2] == 'L' && data[3] == 'F'
}

fn elf_load(data: Vec<u8>) {
    
}
