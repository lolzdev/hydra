const Error = error {
    failed,
    not_supported,
    invalid_param,
    denied,
    invalid_address,
    already_available,
    already_started,
    already_stopped,
    no_shmem,
    invalid_state,
    bad_range,
    timeout,
    io,
    locked,
};

fn call(extension_id: usize, function_id: usize, arg1: usize, arg2: usize, arg3: usize, arg4: usize, arg5: usize, arg6: usize) Error!usize {
    const result = asm volatile(
        \\ecall
        \\mv %[ret], a1
        :
        [ret] "={t0}" (-> usize),
        : [extension_id] "{a7}" (extension_id),
        [function_id] "{a6}" (function_id),
        [arg1] "{a0}" (arg1),
        [arg2] "{a1}" (arg2),
        [arg3] "{a2}" (arg3),
        [arg4] "{a3}" (arg4),
        [arg5] "{a4}" (arg5),
        [arg6] "{a5}" (arg6),
    );
    const err = asm volatile(
        \\mv %[ret], a0
        :
        [ret] "={t0}" (-> i64),
        :
    );

    return switch (err) {
        -1 => Error.failed,
        -2 => Error.not_supported,
        -3 => Error.invalid_param,
        -4 => Error.denied,
        -5 => Error.invalid_address,
        -6 => Error.already_available,
        -7 => Error.already_started,
        -8 => Error.already_stopped,
        -9 => Error.no_shmem,
        -10 => Error.invalid_state,
        -11 => Error.bad_range,
        -12 => Error.timeout,
        -13 => Error.io,
        -14 => Error.locked,
        else => result
    };
}

pub fn setTimer(value: u64) Error!void {
    const b = 1000;
    const a = value + b;
    _ = try call(0x54494d45, 0, a, 0, 0, 0, 0, 0);
}

pub const DebugConsole = struct {
    pub fn write(message: []const u8) !void {
        const address: usize = @intFromPtr(message.ptr);
        const low: usize = address & 0xffffffff;
        const high: usize = (address >> 32) & 0xffffffff;

        _ = try call(0x4442434e, 0x0, message.len, low, high, 0, 0, 0);
    }
};

pub fn probeExtension(id: usize) Error!usize {
    const identifier = id & 0xffffffffffffffff;
    return try call(0x10, 0x3, identifier, 0, 0, 0, 0, 0);
}
