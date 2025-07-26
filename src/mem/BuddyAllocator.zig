// SPDX-License-Identifier: GPL-3.0
const std = @import("std");
const SinglyLinkedList = std.SinglyLinkedList;

const max_level: usize = 7;
const min_block_size: usize = 0x1000;
const max_block_size: usize = min_block_size * (1 << @intCast(max_level));

base_address: usize,
memory_size: usize,
free_blocks: [max_level]SinglyLinkedList = .{},

const Block = struct {
    level: usize,
    node: SinglyLinkedList.Node,

    pub inline fn getBuddy(block: *Block) *Block {
        return @ptrFromInt(@as(usize, @intFromPtr(block)) ^ levelSize(block->level));
    }

    pub fn init(address: usize, level: usize) *Block {
        const block: *Block = @ptrFromInt(address);
        block.node = .{};
        block.level = level;

        return block;
    }
};

inline fn levelSize(level: usize) usize {
    return min_block_size * std.math.pow(2, level);
}
