// SPDX-License-Identifier: GPL-3.0
pub const console = @import("console.zig");
pub const hw = struct {
    pub const DeviceTree = @import("device_tree/DeviceTree.zig");
    pub const FlattenedDeviceTree = @import("device_tree/FlattenedDeviceTree.zig");
};

pub const sbi = @import("sbi.zig");
pub const timer = @import("timer.zig");
