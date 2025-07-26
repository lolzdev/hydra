// SPDX-License-Identifier: GPL-3.0
const std = @import("std");

pub fn build(b: *std.Build) void {
    const target = b.resolveTargetQuery(.{
        .cpu_arch = .riscv64,
        .abi = .none,
        .os_tag = .freestanding
    });

    const optimize = b.standardOptimizeOption(.{});
    const platform = b.option([]const u8, "platform", "Target platform (e.g., virt, qemu-sifive)") orelse "virt";
    const dtb_path = std.fmt.allocPrint(b.allocator, "dtb/{s}.dtb", .{platform}) catch @panic("OOM");
    const dts_path = std.fmt.allocPrint(b.allocator, "dtb/{s}.dts", .{platform}) catch @panic("OOM");

    const exe_mod = b.createModule(.{
        .root_source_file = b.path("src/main.zig"),
        .target = target,
        .optimize = optimize,
        .code_model = .medium
    });


    const exe = b.addExecutable(.{
        .name = "hydra",
        .root_module = exe_mod,
    });

    const root_mod = b.createModule(.{
        .root_source_file = b.path("src/root.zig"),
        .target = target,
        .optimize = optimize,
        .code_model = .medium
    });

    root_mod.addAnonymousImport("platform_dtb", .{
        .root_source_file = b.path(dtb_path),
    });
    exe_mod.addImport("hydra", root_mod);

    const dtb_cmd = b.addSystemCommand(&.{
        "dtc", "-I", "dts", "-O", "dtb", "-o", dtb_path, dts_path
    });
    exe.step.dependOn(&dtb_cmd.step);

    exe.setLinkerScript(b.path("src/linker.ld"));

    const run_cmd = b.addSystemCommand(&.{
        "qemu-system-riscv64", "-nographic", "-kernel", "zig-out/bin/hydra", "-M", "virt", "-no-reboot", "-d", "int", "-d", "cpu_reset"
    });
    run_cmd.step.dependOn(b.default_step);

    const run = b.step("run", "Run the kernel in qemu");
    run.dependOn(&run_cmd.step);

    b.installArtifact(exe);
}
