// SPDX-License-Identifier: GPL-3.0
const BuddyAllocator = @This();
const std = @import("std");
const SinglyLinkedList = std.SinglyLinkedList;
const hydra = @import("hydra");
const DeviceTree = hydra.hw.DeviceTree;

const max_level: usize = 7;
const min_block_size: usize = 0x1000;
const max_block_size: usize = min_block_size * (1 << @intCast(max_level));

const __heap_start = @extern(*u64, .{
    .name = "__heap_start",
    .linkage = .strong,
});

var  free_blocks: [max_level+1]SinglyLinkedList = .{
    .{},
    .{},
    .{},
    .{},
    .{},
    .{},
    .{},
    .{},
};

base_address: usize,
memory_size: usize,

const Block = struct {
    level: usize,
    node: SinglyLinkedList.Node,

    pub inline fn getBuddy(block: *Block) *Block {
        return @ptrFromInt((@as(usize, @intFromPtr(block)) ^ levelSize(block.level)));
    }

    pub fn init(address: usize, level: usize) *Block {
        const block: *Block = @ptrFromInt(address);
        block.node = .{};
        block.level = level;

        return block;
    }
};

pub fn init(device_tree: DeviceTree) void {
    if (device_tree.getDevice("", "memory")) |node| {
        const reg = node.getProperty("reg") orelse unreachable;
        const memory_start = @intFromPtr(__heap_start);
        const offset = memory_start - reg.value.reg.address;
        const base_address = reg.value.reg.address + offset;
        const memory_size = reg.value.reg.size - offset;

        var level: usize = 0;
        while (levelSize(level) < memory_size) {
            level += 1;
        }

        if (level > max_level) {
            level = max_level;
        }

        var address = base_address;
        while (address < (base_address + memory_size)) {
            if (address > (base_address + memory_size) or (address + levelSize(level)) > (base_address + memory_size)) { break; }
            const block = Block.init(address, level);
            free_blocks[level].prepend(&block.node);
            address += levelSize(level);
        }
    } else {
        @panic("Invalid device tree.");
    }
}

inline fn levelSize(level: usize) usize {
    return min_block_size * std.math.pow(usize, 2, level);
}

fn split(block: *Block) void {
    block.level -= 1;
    const buddy = Block.init(@intFromPtr(block.getBuddy()), block.level);
    free_blocks[block.level].prepend(&buddy.node);
}

fn free(self: *anyopaque, memory: []u8, alignment: std.mem.Alignment, ret_addr: usize) void {
    _ = ret_addr;
    _ = alignment;
    _ = self;
    const size = memory.len;
    const pointer = memory.ptr;
    var level: usize = 0;
    while (levelSize(level) < size) {
        level += 1;
    }

    if (level > max_level) {
        level = max_level;
    }

    const block = Block.init(@intFromPtr(pointer), level);
    var result = merge(block);
    while (result) {
        result = merge(block);
    }
}

fn isFree(block: *Block) bool {
    if (@intFromPtr(block) == 0) {
        return false;
    }
    if (block.level > max_level) {
        return false;
    }
    var node = free_blocks[block.level].first;
    while (node) |n| {
        if (@intFromPtr(n) == @intFromPtr(&block.node)) {
            return true;
        }
        node = n.next;
    }

    return false;
}

fn merge(block: *Block) bool {
    const buddy = block.getBuddy();
    if (isFree(buddy)) {
        var node = free_blocks[buddy.level].first;
        var previous: ?*SinglyLinkedList.Node = null;

        while (node) |n| {
            if (@intFromPtr(n) == @intFromPtr(&buddy.node)) {
                if (previous) |prev| {
                    prev.next = n.next;
                } else {
                    free_blocks[buddy.level].first = n.next;
                }
            }

            previous = node;
            node = n.next;
        }

        block.level += 1;
        if (block.level > max_level) {
            return false;
        }
        return true;
    }

    return false;
}

fn alloc(self: *anyopaque, size: usize, alignment: std.mem.Alignment, ret_addr: usize) ?[*]u8 {
    _ = self;
    _ = alignment;
    _ = ret_addr;

    var level: usize = 0;
    while (levelSize(level) < size) {
        level += 1;
    }

    if (level > max_level) {
        level = max_level;
    }

    if (free_blocks[level].popFirst()) |node| {
        const block: *Block = @fieldParentPtr("node", node);
        return @ptrCast(block);
    }

    for (level..max_level+1) |i| {
        if (free_blocks[i].popFirst()) |node| {
            const block: *Block = @fieldParentPtr("node", node);
            while (block.level > level) {
                split(block);
            }

            return @ptrCast(block);
        }
    }

    return null;
}

fn resize(self: *anyopaque, memory: []u8, alignment: std.mem.Alignment, new_len: usize, ret_addr: usize) bool {
    _ = self;
    _ = memory;
    _ = alignment;
    _ = new_len;
    _ = ret_addr;

    return false;
}

fn remap(self: *anyopaque, memory: []u8, alignment: std.mem.Alignment, new_len: usize, ret_addr: usize) ?[*]u8 {
    _ = self;
    _ = memory;
    _ = alignment;
    _ = new_len;
    _ = ret_addr;

    return null;
}

pub fn allocator() std.mem.Allocator {
    return .{
        .ptr = undefined,
        .vtable = &.{
            .alloc = alloc,
            .free = free,
            .resize = resize,
            .remap = remap,
        },
    };
}
