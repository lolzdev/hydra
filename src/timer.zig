const sbi = @import("sbi.zig");

pub fn getTime() u64 {
    return asm volatile(
        \\rdtime t0
        \\mv %[ret], t0
        :
        [ret] "={t1}" (-> u64)
        :
    );
}

pub inline fn seconds(amount: u64) u64 {
    return 10000000 * amount;
}

pub inline fn milliseconds(amount: u64) u64 {
    return 10000 * amount;
}

pub fn addTime(amount: u64) !void {
    const time = getTime();
    try sbi.setTimer(time + amount);
}

pub fn enable() void {
    asm volatile(
        \\li t1, 32
        \\csrs sie, t1
        :
        :
    );
}

pub fn clear() void {
    asm volatile(
        \\li t0, 32
        \\csrc sip, t0
        :
        :
    );
}
