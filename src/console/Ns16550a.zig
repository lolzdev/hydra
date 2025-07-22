const Serial = @import("Serial.zig");
const Self = @This();

pub var instance = Self { .address = @ptrFromInt(0xdeadbeef) };

const Registers = enum(usize) {
    line_status = 0x5,
    line_control = 0x3,
    fifo_control = 0x2,
    line_status_data_ready = 0x1,
};

address: [*]volatile u8,

pub fn init(address: usize) *Self {
    const console: Self = .{
        .address = @ptrFromInt(address),
    };
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
    instance = console;

    return &instance;
}

pub fn printChar(self: *const anyopaque, char: u8) anyerror!void {
    const console: *const Self = @ptrCast(@alignCast(self));
    while ((console.address[5] & (0x1 << 6)) == 0) {}
    console.address[0] = char;
}

pub fn printString(self: *const anyopaque, string: []const u8) anyerror!void {
    const console: *const Self = @ptrCast(@alignCast(self));
    for (string) |c| {
        try printChar(console, c);
    }
}

pub fn printZeroedString(self: *const anyopaque, string: [*]u8) anyerror!void {
    const console: *const Self = @ptrCast(@alignCast(self));
    var index = @as(usize, 0);
    while (string[index] != 0) {
        try printChar(console, string[index]);
        index += 1;
    }
}

pub fn serial(self: *const Self) Serial {
    return .{
        .ptr = self,
        .putCharFn = printChar,
        .printStringFn = printString,
        .printZeroedStringFn = printZeroedString,
    };
}
