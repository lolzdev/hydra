const std = @import("std");

const UartConsole = @This();

const Registers = enum(usize) {
    line_status = 0x5,
    line_control = 0x3,
    fifo_control = 0x2,
    line_status_data_ready = 0x1,
};

address: [*]volatile u8 = @ptrFromInt(0x10000000),

pub fn init() UartConsole {
    const console: UartConsole = .{};
    const lcr = (1 << 0) | (1 << 1);
    console.address[3] = lcr;
    console.address[2] = 1 << 0;
    console.address[1] = 1 << 0;
    console.address[3] = lcr | (1 << 7);
    const divisor: u16 = 592;
    const divisor_least: u8 = divisor & 0xff;
    const divisor_most:  u8 = divisor >> 8;
    console.address[0] = divisor_least;
    console.address[1] = divisor_most;
    console.address[3] = lcr;

    return console;
}

pub fn printChar(console: *const UartConsole, char: u8) void {
    while ((console.address[5] & (0x1 << 6)) == 0) {}
    console.address[0] = char;
}

pub fn printString(console: *const UartConsole, string: []const u8) void {
    for (string) |c| {
        console.printChar(c);
    }
}

pub fn printZeroedString(console: *const UartConsole, string: [*]u8) void {
    var index = @as(usize, 0);
    while (string[index] != 0) {
        console.printChar(string[index]);
        index += 1;
    }
}
