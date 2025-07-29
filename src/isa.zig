// SPDX-License-Identifier: GPL-3.0
pub inline fn readScause() usize {
    return asm volatile(
        "csrr %[ret], scause"
        :
        [ret] "={a0}" (-> usize)
    );
}

pub inline fn readSepc() usize {
    return asm volatile(
        "csrr %[ret], sepc"
        :
        [ret] "={a0}" (-> usize)
    );
}

pub inline fn readRegister(reg: usize) usize {
    return asm volatile(
        "mv t0, x%[reg]"
        :
        [ret] "={t0}" (-> usize)
        :
        [reg] "I" (reg),
    );
}
