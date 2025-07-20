const std = @import("std");

pub fn build(b: *std.Build) void {
    const target = b.resolveTargetQuery(.{
        .cpu_arch = .riscv64,
        .abi = .none,
        .os_tag = .freestanding
    });

    const optimize = b.standardOptimizeOption(.{});

    const exe_mod = b.createModule(.{
        .root_source_file = b.path("src/main.zig"),
        .target = target,
        .optimize = optimize,
        .code_model = .medium
    });

    const exe = b.addExecutable(.{
        .name = "sbi",
        .root_module = exe_mod,
    });
    exe.setLinkerScript(b.path("src/linker.ld"));

    const run_cmd = b.addSystemCommand(&.{
        "qemu-system-riscv64", "-nographic", "-bios", "zig-out/bin/sbi", "-M", "virt"
    });
    const run = b.step("run", "Run the kernel in qemu");
    run.dependOn(&run_cmd.step);

    b.installArtifact(exe);
}
