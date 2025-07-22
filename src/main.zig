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
const UartConsole = @import("console/UartConsole.zig");
const DeviceTree = @import("device_tree/DeviceTree.zig");
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
    const console = UartConsole.init();

    var allocator: std.heap.FixedBufferAllocator = .{
        .end_index = 0,
        .buffer = getHeapBuffer(),
    };

    console.printString("Booting Hydra\n");
    console.printString("Booting Hydra2\n");
    
    if (FlattenedDeviceTree.parse(allocator.allocator())) |device_tree| {
        //console.printString("Found valid device tree\n");
        device_tree.dump(allocator.allocator());
    } else |err| switch(err) {
        FlattenedDeviceTree.ParsingError.InvalidHeader => {
            console.printString("Failed to parse the device tree\n");
        }
    }

    console.printString("Done\n");
    
    while (true) {}
}

export fn trap() callconv(.c) void {
    while (true) {}
}
