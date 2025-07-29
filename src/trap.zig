// SPDX-License-Identifier: GPL-3.0
const hydra = @import("hydra");
const timer = hydra.timer;
const sbi = hydra.sbi;
const isa = @import("isa.zig");

const TrapCause = enum(usize) {
    software = 1,
    timer = 5,
    external = 9,
    counter_overflow = 13,
};

const ExceptionCause = enum(usize) {
    instruction_address_misaligned = 0,
    instruction_access_fault = 1,
    illegal_instruction = 2,
    breakpoint = 3,
    load_address_misaligned = 4,
    load_access_fault = 5,
    store_address_misaligned = 6,
    store_access_fault = 7,
    user_call = 8,
    supervisor_call = 9,
    instruction_page_fault = 12,
    load_page_fault = 13,
    store_page_fault = 15,
    software_check = 18,
    hardware_error = 19,
};

pub fn trap_handler() align(4) callconv(.{.riscv64_interrupt = .{ .mode = .supervisor }}) void {
    const cause = isa.readScause();
    const code_mask = ~(@as(usize, 0x1) << 63);
    const pc = isa.readSepc();
    hydra.console.print("Trap at: 0x{x}", .{pc});

    if (cause & (0x1 << 63) != 0) {
        switch (@as(TrapCause, @enumFromInt(cause & code_mask))) {
            .software => {
                sbi.DebugConsole.write("Software interrupt\n") catch @panic("");
            },
            .timer => {
                sbi.DebugConsole.write("Timer\n") catch @panic("");
                timer.addTime(timer.milliseconds(500)) catch @panic("");
                timer.clear();
            },
            .counter_overflow => {
                sbi.DebugConsole.write("Counter overflow\n") catch @panic("");
            },
            else => {}
        }
    } else {
        switch (@as(ExceptionCause, @enumFromInt(cause & code_mask))) {
            else => {
                sbi.DebugConsole.write("Exception\n") catch @panic("");
                while (true) {}
            }
        }
    }
}
