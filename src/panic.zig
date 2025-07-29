// SPDX-License-Identifier: GPL-3.0
const sbi = hydra.sbi;
const isa = @import("isa.zig");
const hydra = @import("hydra");

pub fn kernelPanic(msg: []const u8, first_trace_addr: ?usize) noreturn {
    hydra.console.print(
        \\{s}Kernel panic: {s}
        \\at: 0x{?}
        \\Register values:
        \\stack pointer: 0x{x}
        \\global pointer: 0x{x}
        \\thread pointer: 0x{x}
        \\
        ,
        .{
            "\x1b[31m",
            msg,
            first_trace_addr,
            isa.readRegister(2),
            isa.readRegister(3),
            isa.readRegister(4),
        }
    );

    inline for (10..18) |r| {
        hydra.console.print("a{d}: 0x{x}\n", .{r-10, isa.readRegister(r)});
    }

    hydra.console.print("\x1b[0m", .{});

    while (true) {}
}
