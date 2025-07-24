const Serial = @This();
const std = @import("std");
const Writer = std.Io.Writer;

writer: Writer = .{
    .vtable = &.{.drain = Serial.drain},
    .buffer = &.{},
},
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

fn drain(w: *Writer, data: []const []const u8, splat: usize) error{WriteFailed}!usize {
    _ = splat;
    const self: *Serial = @fieldParentPtr("writer", w);
    for (data) |s| {
        self.putChar(s[0]) catch return error.WriteFailed;
    }

    return data.len;
}
