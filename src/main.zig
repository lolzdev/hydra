// SPDX-License-Identifier: GPL-3.0

const sbi = hydra.sbi;
const hydra = @import("hydra");
const FlattenedDeviceTree = hydra.hw.FlattenedDeviceTree;
const std = @import("std");
const kernelPanic = @import("panic.zig").kernelPanic;
const trap = @import("trap.zig");
const BuddyAllocator = @import("mem/BuddyAllocator.zig");

export const trap_handler = trap.trap_handler;

const __heap_start = @extern(*u64, .{
    .name = "__heap_start",
    .linkage = .strong,
});

const __heap_size = @extern(*u64, .{
    .name = "__heap_size",
    .linkage = .strong,
});

pub const panic = std.debug.FullPanic(kernelPanic);

export fn _start() linksection(".text.init") callconv(.naked) void {
    asm volatile(
        \\la gp, __global_pointer
        \\la t0, __bss_start
        \\la t1, __bss_end
        \\1:
        \\sd zero, (t0)
        \\addi t0, t0, 8
        \\bltu t0, t1, 1b
        \\la sp, __stack
        \\la t0, trap_handler
        \\csrw stvec, t0
        \\csrsi sstatus, 2
        \\call main
    );
}

pub fn getHeapBuffer() []u8 {
    const start = @as([*]u8, @ptrCast(__heap_start));
    return start[0..@intFromPtr(__heap_size)];
}

export fn main(heart: usize) callconv(.c) void {
    if (heart != 0) {
        while (true) {}
    }

    var allocator: std.heap.FixedBufferAllocator = .{
        .end_index = 0,
        .buffer = getHeapBuffer(),
    };

    if (FlattenedDeviceTree.parse(allocator.allocator())) |device_tree| {
        hydra.console.init(device_tree);
        hydra.console.print("Booting hydra\n", .{});

        BuddyAllocator.init(device_tree);
        const a = BuddyAllocator.allocator();
        const ptr = a.create(usize) catch @panic("OOM");
        hydra.console.print("Allocated: 0x{x}\n", .{@intFromPtr(ptr)});
    } else |err| switch(err) {
        FlattenedDeviceTree.ParsingError.InvalidHeader => {
        }
    }

    while (true) {}
}
