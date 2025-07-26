// SPDX-License-Identifier: GPL-3.0
/// Flattened device tree parsing (https://devicetree-specification.readthedocs.io/en/stable/index.html)

const std = @import("std");
const FlattenedDeviceTree = @This();
const DeviceTree = @import("DeviceTree.zig");

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

pub fn parse(allocator: std.mem.Allocator) !DeviceTree {
    const dtb = @embedFile("platform_dtb");
    const address = @as(usize, @intFromPtr(dtb.ptr));
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

    const structure_offset = @as(u32, @byteSwap(header.structure_offset));
    const structure_address = address + @as(usize, @intCast(structure_offset));
    const structure = @as([*]u8, @ptrFromInt(structure_address));

    var index = @as(usize, 0);
    var parent_stack: std.SinglyLinkedList = .{};
    var current_node: *DeviceTree.Node = DeviceTree.Node.init(allocator, "/");
    while (true) {
        if (readU32(structure[index..index+4]) == @intFromEnum(TokenType.end)) break;

        switch (@as(TokenType, @enumFromInt(readU32(structure[index..index+4])))) {
            TokenType.begin_node => {
                parent_stack.prepend(&current_node.node);
                index += 4;

                const name = @as([*]u8, structure[index..]);
                var j = @as(usize, 1);
                while (name[j] != 0) {
                    j += 1;
                }
                current_node = DeviceTree.Node.init(allocator, name[0..j]);

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

                var j = @as(usize, 0);
                while (name[j] != 0) {
                    j += 1;
                }

                if (length % 4 > 0) {
                    length = length + (4 - length % 4);
                }

                const value = @as([*]u8, @ptrCast(&structure[index]));
                if (std.mem.eql(u8, name[0..j], "#address-cells")) {
                    for (0..4) |i| {
                        current_node.address_cells |= (@as(u32, @intCast(value[i])) << (8 * @as(u5, @intCast(i))));
                    }
                    current_node.address_cells = @byteSwap(current_node.address_cells);
                } else if (std.mem.eql(u8, name[0..j], "#size-cells")) {
                    for (0..4) |i| {
                        current_node.size_cells |= (@as(u32, @intCast(value[i])) << (8 * @as(u5, @intCast(i))));
                    }
                    current_node.size_cells = @byteSwap(current_node.size_cells);
                }  else if (std.mem.eql(u8, name[0..j], "reg")) {
                    var register_address = @as(usize, 0);
                    var size = @as(usize, 0);
                    var current_val = @as(usize, 0);
                    if (parent_stack.first) |parent| {
                        const parent_node: *DeviceTree.Node = @fieldParentPtr("node", parent);

                        for (0..parent_node.address_cells) |cell| {
                            for ((cell * 4)..(cell * 4 + 4)) |i| {
                                current_val |= (@as(usize, @intCast(value[i])) << (8 * @as(u6, @intCast(i % 4))));
                            }

                            register_address >>= 32;
                            register_address |= (current_val << (32 * @as(u6, @intCast(cell))));
                            current_val = 0;
                        }
                        register_address = @byteSwap(register_address);
                        
                        current_val = 0;

                        for (0..parent_node.size_cells) |cell| {
                            for ((cell * 4 + (parent_node.address_cells * 4))..(cell * 4 + 4 + (parent_node.address_cells * 4))) |i| {
                                current_val |= (@as(usize, @intCast(value[i])) << (8 * @as(u6, @intCast(i % 4))));
                            }
                            size |= (current_val << (32 * @as(u6, @intCast(cell))));
                        }

                        size = @byteSwap(size);

                        const property = DeviceTree.Property.init(allocator, name[0..j], .{ .reg = .{ .address = register_address, .size = size } });
                        current_node.properties.prepend(&property.node);
                    }
                } else if (std.mem.eql(u8, name[0..j], "compatible") or std.mem.eql(u8, name[0..j], "device_type")) {

                    var k = @as(usize, 0);
                    while (value[k] != 0) {
                        k += 1;
                    }

                    const property = DeviceTree.Property.init(allocator, name[0..j], .{ .string = value[0..k] });
                    current_node.properties.prepend(&property.node);
                } else {
                    const property = DeviceTree.Property.init(allocator, name[0..j], .{ .string = "" });
                    current_node.properties.prepend(&property.node);
                }

                index += length;
            },
            TokenType.end_node => {
                index += 4;
                if (parent_stack.popFirst()) |parent| {
                    const parent_node: *DeviceTree.Node = @fieldParentPtr("node", parent);
                    parent_node.children.prepend(&current_node.node);
                    current_node = parent_node;
                } else {
                    break;
                }
            },
            TokenType.nop => {
                index += 4;
            },
            else => {
            }
        }
    }

    _ = reservation_list;

    return .{
        .root = current_node,
    };
}
