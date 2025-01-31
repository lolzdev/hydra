# Hydra ![GitHub Actions Workflow Status](https://img.shields.io/github/actions/workflow/status/lolzdev/hydra/build.yml) ![Progress](https://img.shields.io/badge/progress-wip-orange)

## Introduction
Hydra is a microkernel with the main goal of keeping the codebase [as small and simple as possible](https://suckless.org/philosophy/).
A smaller codebase can be easily maintained and is less prone to bugs or issues, and modern monolithic kernels tend to be tremendously big (e.g. Linux, FreeBSD).
Being a microkernel drivers can be written in any language by exposing the kernel API and then run in userspace, this keeps all the system a lot more modular.

## Building
To build Hydra you need `clang` and `lld`. If you want to generate an ISO bootable image you also need `xorriso` and for testing in a virtual machine `qemu` is also needed.

After all the requirements are satisfied you can just fire the `make` command. The `Makefile` supports these targets:
- `all` (compile and link libc and hydra)
- `sys` (just compile the kernel)
- `libc` (just compile libc)
- `test` (compile all and run the os in `qemu`)
- `debug` (same as test, but let `qemu` wait for a `gdb` connection)
- `gdb` (run `gdb` with a configuration so that it automatically connects to `qemu`)
- `clean` (remove generated files)
