pub const Property = struct {
    name: []const u8,
    value: []const u8,
};

pub const Node = struct {
    name: []const u8,
    properties: []Property,
    children: []Node,
};
