# Cortex-A15 GNU example build

This example demonstrates that ThreadX **builds and links** for the Cortex-A15.
It is not a runnable image.

`tx_initialize_low_level.S` here is the generic ARMv7-A skeleton: it provides
the interrupt handler shell but programs no interrupt controller and no timer,
and it does not install a vector table. ThreadX will therefore not receive a
timer tick on this example, so threads that rely on time slicing or on
`tx_thread_sleep()` will not run as intended.

For a model targeted example with a GIC, a private timer and a vector table
installed, see the Cortex-A5, A7, A8 or A9 example builds. Those carry the
matching support files (`MP_GIC`, `MP_PrivateTimer`, `v7.s`) and target the
Versatile Express fixed virtual platforms.

## Building

```sh
./build_threadx.sh          # the ThreadX library, tx.a
./build_threadx_sample.sh   # the sample image
```

Set `TOOLCHAIN=atfe` to build with Arm Toolchain for Embedded instead of the
GNU toolchain:

```sh
TOOLCHAIN=atfe ATFE_CLANG=/path/to/clang ./build_threadx.sh
TOOLCHAIN=atfe ATFE_CLANG=/path/to/clang ./build_threadx_sample.sh
```
