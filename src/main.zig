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
        \\la t0, trap
        \\csrw stvec, t0
        \\call main
    );
}

const Machine = @import("Machine.zig");
const FlattenedDeviceTree = @import("device_tree/FlattenedDeviceTree.zig");
const UartConsole = @import("console.zig");
const DeviceTree = @import("device_tree/DeviceTree.zig");
const console = @import("console.zig");
const std = @import("std");

const __heap_start =  @extern(*u64, .{
    .name = "__heap_start",
    .linkage = .strong,
});

const __heap_size =  @extern(*u64, .{
    .name = "__heap_size",
    .linkage = .strong,
});

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
        if (console.serial(device_tree)) |serial| {
            serial.printString("Booting Hydra\n") catch @panic("error with console\n");
        }
    } else |err| switch(err) {
        FlattenedDeviceTree.ParsingError.InvalidHeader => {
        }
    }

    while (true) {}
}

export fn trap() callconv(.c) void {
    while (true) {}
}
