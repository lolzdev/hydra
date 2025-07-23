const std = @import("std");
const SinglyLinkedList = std.SinglyLinkedList;
const DeviceTree = @This();

pub const Property = struct {
    const Self = @This();
    pub const Value = union {
        string: []const u8,
        int: u64,
        reg: struct {
            address: usize,
            size: usize
        },
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
    address_cells: u32 = 0,
    size_cells: u32 = 0,
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

    pub fn getProperty(self: *const Self, name: []const u8) ?*Property {
        var prop = self.properties.first;
        while (prop) |property| {
            const node_prop: *DeviceTree.Property = @fieldParentPtr("node", property);
            if (std.mem.eql(u8, node_prop.name, name)) {
                return node_prop;
            }
            prop = property.next;
        }
        return null;
    }
};

pub fn getDevices(device_tree: *const DeviceTree, allocator: std.mem.Allocator, path: []const u8, ty: []const u8) SinglyLinkedList {
    var linked_list = SinglyLinkedList {};
    const root_node = device_tree.getNode(path) orelse return linked_list;
    var node = root_node.children.first;
    while (node) |n| {
        const devtree_node: *Node = @fieldParentPtr("node", n);
        
        if (devtree_node.getProperty("device_type")) |device_type| {
            if (std.mem.eql(u8, device_type.value.string, ty)) {
                const node_copy: *Node = Node.init(allocator, devtree_node.name);
                node_copy.children.first = devtree_node.children.first;
                linked_list.prepend(&node_copy.node);
            }
        }
        node = n.next;
    }

    return linked_list;
}

pub fn getDevice(device_tree: *const DeviceTree, path: []const u8, ty: []const u8) ?*Node {
    return device_tree.withProperty(path, "device_type", .{.string = ty});
}

pub fn compatibleWith(device_tree: *const DeviceTree, path: []const u8, device: []const u8) ?*Node {
    const root_node = device_tree.getNode(path) orelse return null;
    var node = root_node.children.first;
    while (node) |n| {
        const devtree_node: *Node = @fieldParentPtr("node", n);
        
        if (devtree_node.getProperty("compatible")) |compat| {
            var compatibilities = std.mem.splitSequence(u8, compat.value.string, ",");
            
            while (compatibilities.next()) |compatibility| {
                if (std.mem.eql(u8, compatibility, device)) {
                    return devtree_node;
                }
            }
        }
        node = n.next;
    }

    return null;
}

pub fn withProperty(device_tree: *const DeviceTree, path: []const u8, property: []const u8, value: Property.Value) ?*Node {
    const root_node = device_tree.getNode(path) orelse return null;
    var node = root_node.children.first;
    while (node) |n| {
        const devtree_node: *Node = @fieldParentPtr("node", n);
        
        if (devtree_node.getProperty(property)) |device_type| {
            if (std.mem.eql(Property.Value, device_type.value, value)) {
                return devtree_node;
            }
        }
        node = n.next;
    }

    return null;
}

pub fn getNode(device_tree: *const DeviceTree, path: []const u8) ?*Node {
    const root = device_tree.root.children.first orelse @panic("Malformed device tree");
    const root_node: *Node = @fieldParentPtr("node", root);
    if (path.len == 0) return root_node;
    var node = root_node.children.first;
    var dirs = std.mem.splitSequence(u8, path, "/");
    while (node) |n| {
        const devtree_node: *Node = @fieldParentPtr("node", n);

        if (dirs.peek()) |dir| {
            if (std.mem.eql(u8, devtree_node.name, dir)) {
                _ = dirs.next();
                if (dirs.peek()) |_| {
                } else {
                    return devtree_node;
                }
                node = devtree_node.children.first;
            } else {
                node = n.next;
            }
        } else {
            return null;
        }
    }

    return null;
}

root: *Node,
