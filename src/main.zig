const hydra = @import("hydra");
const FlattenedDeviceTree = hydra.hw.FlattenedDeviceTree;
const DeviceTree = hydra.hw.DeviceTree;
const timer = hydra.timer;
const sbi = hydra.sbi;
const std = @import("std");
const trap = @import("trap.zig");
export const trap_handler = trap.trap_handler;

const __heap_start =  @extern(*u64, .{
    .name = "__heap_start",
    .linkage = .strong,
});

const __heap_size =  @extern(*u64, .{
    .name = "__heap_size",
    .linkage = .strong,
});

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
        if (hydra.console.serial(device_tree)) |serial| {
            serial.printString("Booting Hydra\n") catch @panic("error with console\n");
            
            timer.addTime(5000000) catch @panic("");
            timer.enable();
        }
    } else |err| switch(err) {
        FlattenedDeviceTree.ParsingError.InvalidHeader => {
        }
    }

    while (true) {}
}
