// SPDX-License-Identifier: GPL-3.0
const DeviceTree = @import("device_tree/DeviceTree.zig");

pub const Serial = @import("console/Serial.zig");
pub const Ns16550a = @import("console/Ns16550a.zig");

var serial: Serial = undefined;

pub fn init(device_tree: DeviceTree) void {
    if (getSerial(device_tree)) |s| {
        serial = s;
    } else {
        @panic("Failed to initialize serial console");
    }
}

pub fn getSerial(device_tree: DeviceTree) ?Serial {
    if (device_tree.compatibleWith("soc", "ns16550a,snps")) |node|{
        if (node.getProperty("reg")) |reg| {
            return Ns16550a.init(reg.value.reg.address).serial();
        }
    }

    return null;
}

pub inline fn print(comptime format: []const u8, args: anytype) void {
    serial.writer.print(format, args) catch @panic("Failed to print on the serial console.");
}
