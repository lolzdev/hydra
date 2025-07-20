const UartConsole = @This();

address: [*]u8 = @ptrFromInt(0x10000000),

pub fn printString(console: *const UartConsole, string: []const u8) void {
    for (0..string.len) |index| {
        console.address[0] = string[index];
    }
}

pub fn printZeroedString(console: *const UartConsole, string: [*]u8) void {
    var index = @as(usize, 0);
    while (string[index] != 0) {
        console.address[0] = string[index];
        index += 1;
    }
}
