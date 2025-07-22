const Serial = @This();

ptr: *const anyopaque,
putCharFn: *const fn (context: *const anyopaque, char: u8) anyerror!void,
printStringFn: *const fn (context: *const anyopaque, string: []const u8) anyerror!void,
printZeroedStringFn: *const fn (context: *const anyopaque, string: [*]u8) anyerror!void,

pub fn putChar(self: Serial, char: u8) anyerror!void {
    try self.putCharFn(self.ptr, char);
}

pub fn printString(self: Serial, string: []const u8) anyerror!void {
    try self.printStringFn(self.ptr, string);
}

pub fn printZeroedString(self: Serial, string: [*]u8) anyerror!void {
    try self.printZeroedStringFn(self.ptr, string);
}
