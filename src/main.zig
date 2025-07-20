export fn _start() callconv(.Naked) void {
    asm volatile(
        \\bnez a0, loop
        \\csrw satp, zero
        \\la gp, __global_pointer
        \\la t0, __bss_start
        \\la t1, __bss_end
        \\1:
        \\sd zero, (t0)
        \\addi t0, t0, 8
        \\bltu t0, t1, 1b
        \\la sp, __stack
        \\li t0, (0b11 << 11) | (1 << 7) | (1 << 3)
        \\csrw mstatus, t0
        \\la t2, trap
        \\csrw stvec, t2
        \\li t3, (1 << 3) | (1 << 11)
        \\csrw sie, t3
        \\j main
        \\j loop
    );
}

export fn loop() callconv(.c) void {
    while (true) {}
}

const Machine = @import("Machine.zig");
const FlattenedDeviceTree = @import("device_tree/FlattenedDeviceTree.zig");
const UartConsole = @import("console/UartConsole.zig");
const console = UartConsole {};

export fn main(a0: usize, fdt_base: usize) callconv(.c) void {
    _ = a0;
    _ = Machine {
        .hearts = 65,
        .vendor_id = 0,
        .arch_id = 0
    };

    if (FlattenedDeviceTree.parse(fdt_base)) |device_tree| {
        _ = device_tree;
        console.printString("Found valid device tree\n");
    } else |err| switch(err) {
        FlattenedDeviceTree.ParsingError.InvalidHeader => {
            console.printString("Failed to parse the device tree\n");
        }
    }
    
    while (true) {}
}

export fn trap() callconv(.c) void {
    const cause = asm volatile("csrr t0, scause" : [ret] "={t0}" (-> usize));
    if ((cause >> 63) & 0x1 == 1) {
        console.printString("interrupt");
    } else {
        switch (cause) {
            0 => console.printString("Instruction address misaligned\n"),
            1 => console.printString("Instruction access fault\n"),
            2 => console.printString("Illegal instruction\n"),
            3 => console.printString("Breakpoint\n"),
            4 => console.printString("Load address misaligned\n"),
            5 => console.printString("Load access fault\n"),
            6 => console.printString("Store/AMO address misaligned\n"),
            7 => console.printString("Store/AMO access fault\n"),
            8 => console.printString("Environment call from U-mode\n"),
            9 => console.printString("Environment call from S-mode\n"),
            10 => console.printString("Reserved\n"),
            11 => console.printString("Environment call from M-mode\n"),
            12 => console.printString("Instruction page fault\n"),
            13 => console.printString("Load page fault\n"),
            14 => console.printString("Reserved\n"),
            15 => console.printString("Store/AMO page fault\n"),
            16 => console.printString("Double trap\n"),
            17 => console.printString("Reserved\n"),
            18 => console.printString("Software check\n"),
            19 => console.printString("Hardware error\n"),
            else => console.printString("unknown trap\n"),
        }
    }
}
