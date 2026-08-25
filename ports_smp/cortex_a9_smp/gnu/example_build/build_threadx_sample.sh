#!/bin/sh
set -e

# this script builds the ThreadX library for the Cortex-A5 SMP port using
# GNU toolchain or llvm/clang toolchain (defaults to gnu).
: "${TOOLCHAIN:=gnu}"
case "${TOOLCHAIN}" in
    gnu)
        CC="arm-none-eabi-gcc"
        TARGET_FLAGS="" # GNU and Clang use different driver syntax for selecting the program's
        # entry-point symbol.
        ENTRY_FLAG="-e Vectors"
        # Bare-metal C libraries require syscall support. GNU uses newlib's nosys stubs
        # while ATFE requires a semihost library.
        SYSCALL_LIB="--specs=nosys.specs"
        ;;
    atfe)
        CC="${ATFE_CLANG:-clang}"
        TARGET_FLAGS="--target=arm-none-eabi"
        ENTRY_FLAG="-Wl,--entry=Vectors"
        SYSCALL_LIB="-lsemihost"
        ;;
    *)
        echo "Unknown TOOLCHAIN: ${TOOLCHAIN}" >&2
        exit 1
        ;;
esac

arm-none-eabi-gcc -c -g -I../../../../common_smp/inc -I../inc -mcpu=cortex-a9 sample_threadx.c
arm-none-eabi-gcc -c -g -mcpu=cortex-a9 startup.S
arm-none-eabi-gcc -c -g -mcpu=cortex-a9 MP_GIC.S
arm-none-eabi-gcc -c -g -mcpu=cortex-a9 MP_SCU.S
arm-none-eabi-gcc -c -g -mcpu=cortex-a9 MP_Mutexes.S
arm-none-eabi-gcc -c -g -mcpu=cortex-a9 MP_PrivateTimer.S
arm-none-eabi-gcc -c -g -mcpu=cortex-a9 v7.S
arm-none-eabi-gcc -T sample_threadx.ld -e Vectors -mcpu=cortex-a9 --specs=nosys.specs -o sample_threadx.out MP_PrivateTimer.o MP_GIC.o MP_Mutexes.o MP_SCU.o sample_threadx.o startup.o v7.o tx.a -Wl,-M > sample_threadx.map
