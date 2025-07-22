const DeviceTree = @import("device_tree/DeviceTree.zig");

pub const Serial = @import("console/Serial.zig");
pub const Ns16550a = @import("console/Ns16550a.zig");

pub fn serial(device_tree: DeviceTree) ?Serial {
    if (device_tree.compatibleWith("soc", "ns16550a")) |node|{
        if (node.getProperty("reg")) |reg| {
            return Ns16550a.init(reg.value.reg.address).serial();
        }
    }

    return null;
}
