# AArch64 GNU example build

Builds the ThreadX library and a sample image for the AArch64 port this
directory belongs to.

```sh
./build_threadx.sh          # the ThreadX library, tx.a
./build_threadx_sample.sh   # the sample image, sample_threadx.out
```

Set `TOOLCHAIN=atfe` to build with Arm Toolchain for Embedded, which is LLVM
based, instead of the GNU toolchain:

```sh
TOOLCHAIN=atfe ATFE_CLANG=/path/to/clang ./build_threadx.sh
TOOLCHAIN=atfe ATFE_CLANG=/path/to/clang ./build_threadx_sample.sh
```

Both scripts derive the `-mcpu` value and the kernel source directory from the
port directory they sit in, so one pair of scripts serves every AArch64 port,
SMP included. Before they existed these examples could only be built through the
Arm Development Studio project files beside them, which is why the `.cproject`
and `.launch` files remain.

## What the sample targets

A fixed virtual platform: a GICv3 interrupt controller and an SP804 timer at the
Base platform addresses, as set up by `startup.S`, `vectors.S` and the GICv3
sources here. It will not run on a model or board that places those peripherals
elsewhere without changing the addresses.

## Semihosting

`startup.S` calls `initialise_monitor_handles` to open the standard file handles
over a debugger connection. The GNU toolchain provides it in libgloss, pulled in
by `--specs=rdimon.specs`. picolibc, which Arm Toolchain for Embedded ships, has
no equivalent and neither does its semihosting library, so `semihost_stub.S`
supplies a weak no-op for that toolchain only. A real definition always wins
over it.
