const std = @import("std");
const SinglyLinkedList = std.SinglyLinkedList;
const UartConsole = @import("../console/UartConsole.zig");
const DeviceTree = @This();

pub const Property = struct {
    const Self = @This();
    pub const Value = union {
        string: []const u8,
        int: u64,
    };

    name: []const u8,
    value: Value,
    node: SinglyLinkedList.Node,

    pub fn init(allocator: std.mem.Allocator, name: []const u8, value: Value) *Self {
        const property = allocator.create(Self) catch @panic("OOM");
        property.* = .{
            .name = name,
            .value = value,
            .node = .{}
        };

        return property;
    }
};

pub const Node = struct {
    const Self = @This();
    name: []const u8,
    properties: SinglyLinkedList,
    children: SinglyLinkedList,
    node: SinglyLinkedList.Node,

    pub fn init(allocator: std.mem.Allocator, name: []const u8) *Self {
        const node: *Self = allocator.create(Self) catch @panic("OOM");
        node.* = .{
            .name = name,
            .properties = .{},
            .children = .{},
            .node = .{},
        };

        return node;
    }

    pub fn prepend_child(self: *Self, child: *Self) void {
        self.children.prepend(&child.node);
    }
};

pub fn dump(device_tree: *const DeviceTree, allocator: std.mem.Allocator) void {
    const console = UartConsole {};
    var node = device_tree.root.children.first;
    while (node) |n| {
        const devtree_node: *DeviceTree.Node = @fieldParentPtr("node", n);
        var prop = devtree_node.properties.first;
        while (prop) |property| {
            const node_prop: *DeviceTree.Property = @fieldParentPtr("node", property);
            console.printString(node_prop.name);
            const name = node_prop.name;
            const name2 = allocator.alloc(u8, name.len) catch @panic("OOM");
            @memcpy(name2, "model");
            if (std.mem.eql(u8, name, "model")) {
                console.printString(" = ");
                console.printString(node_prop.value.string);
            }
            console.printChar('\n');
            prop = property.next;
        }
        
        node = n.next;
    }
}

root: *Node,
