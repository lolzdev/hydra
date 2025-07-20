const FlattenedDeviceTree = @This();
const UartConsole = @import("../console/UartConsole.zig");
const std = @import("std");
const console = UartConsole {};

pub const ParsingError = error {
    InvalidHeader
};

pub const Header = packed struct {
    magic: u32,
    total_size: u32,
    structure_offset: u32,
    strings_offset: u32,
    reservation_offset: u32,
    version: u32,
    compatibility: u32,
    boot_cpuid: u32,
    strings_size: u32,
    structure_size: u32
};

pub const ReservationEntry = packed struct {
    address: usize,
    size: usize,
};

pub const TokenType = enum(u32) {
    begin_node = 0x1,
    end_node = 0x2,
    property = 0x3,
    nop = 0x4,
    end = 0x9,
};

pub const Property = packed struct {
    length: u32,
    name_offset: u32,
};

header: *Header,
reservation_list: []ReservationEntry,

fn readU32(slice: []u8) u32 {
    return (@as(u32, @intCast(slice[3])) | (@as(u32, @intCast(slice[2])) << 8) | (@as(u32, @intCast(slice[1])) << 16) | (@as(u32, @intCast(slice[0])) << 24));
}

pub fn parse() !FlattenedDeviceTree {
    const dtb = @embedFile("platform_dtb");
    const address = @as(usize, @intFromPtr(dtb.ptr));
    //const address = @as(usize, 0x100000);
    const header = @as(*Header, @alignCast(@constCast(@ptrCast(dtb.ptr))));
    
    // Validate the table header
    if (header.magic != @byteSwap(@as(u32, 0xd00dfeed))) {
        return ParsingError.InvalidHeader;
    }

    // Count how many reservation entries there are
    var reservation_list_length = @as(usize, 0);
    const reservation_offset = @as(u32, @byteSwap(header.reservation_offset));
    var entry_address = address + (@as(usize, @intCast(reservation_offset)) >> 32);
    while (true) {
        const reservation_entry = @as(*ReservationEntry, @ptrFromInt(entry_address));
        if (reservation_entry.address == 0 and reservation_entry.size == 0) {
            break;
        }
        
        reservation_list_length += 1;

        entry_address += @sizeOf(ReservationEntry);
        break;
    }

    const reservation_list = @as([*]ReservationEntry, @ptrFromInt(address + (@as(usize, @intCast(reservation_offset)) >> 32)))[0..reservation_list_length];

    const strings_offset = @as(u32, @byteSwap(header.strings_offset));
    const strings_address = address + (@as(usize, @intCast(strings_offset)));
    const strings = @as([*]u8, @ptrFromInt(strings_address));


    const structure_offset = @as(u32, @byteSwap(header.structure_offset));
    const structure_address = address + @as(usize, @intCast(structure_offset));
    const structure = @as([*]u8, @ptrFromInt(structure_address));
    //const data = @as([*]u8, @ptrFromInt(structure_address));

    var index = @as(usize, 0);
    while (true) {
        if (readU32(structure[index..index+4]) == @intFromEnum(TokenType.end)) break;

        switch (@as(TokenType, @enumFromInt(readU32(structure[index..index+4])))) {
            TokenType.begin_node => {
                index += 4;

                const name = @as([*]u8, structure[index..]);
                console.printZeroedString(name);
                console.printString(" ->\n");
                var j = @as(usize, 1);
                while (name[j] != 0) {
                    j += 1;
                }

                index += j+1;
                if (index % 4 > 0) {
                    index += 4 - index % 4;
                }
            },
            TokenType.property => {
                index += 4;
                var length = @as(usize, @intCast(readU32(structure[index..index+4])));
                index += 4;
                const name_offset = readU32(structure[index..index+4]);
                const name = @as([*]u8, @ptrFromInt(strings_address+name_offset));
                index += 4;

                console.printZeroedString(name);

                if (length % 4 > 0) {
                    length = length + (4 - length % 4);
                }

                if (name[0] != '#') {
                    console.printString(" = ");
                    console.printZeroedString(@ptrCast(&structure[index]));
                    console.printString("\n");
                } else {
                    console.printString("\n");
                }

                index += length;
            },
            TokenType.end_node => {
                index += 4;
            },
            TokenType.nop => {
                index += 4;
            },
            else => {
            }
        }
    }

    console.printString(strings[0..10]);

    return .{
        .header = header,
        .reservation_list = reservation_list
    };
}
