pub inline fn readScause() usize {
    return asm volatile(
        "csrr %[ret], scause"
        :
        [ret] "={a0}" (-> usize)
    );
}
