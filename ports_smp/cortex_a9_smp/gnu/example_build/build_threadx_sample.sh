#!/bin/sh
set -e
cd "$(dirname "$0")"

# this script builds the ThreadX sample image for the Cortex-A9 SMP port using
# GNU toolchain or llvm/clang toolchain (defaults to GNU).
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

"${CC}" ${TARGET_FLAGS} -c -g -I../../../../common_smp/inc -I../inc -mcpu=cortex-a9 sample_threadx.c
"${CC}" ${TARGET_FLAGS} -c -g -mcpu=cortex-a9 startup.S
"${CC}" ${TARGET_FLAGS} -c -g -mcpu=cortex-a9 crt0.S
"${CC}" ${TARGET_FLAGS} -c -g -mcpu=cortex-a9 MP_GIC.S
"${CC}" ${TARGET_FLAGS} -c -g -mcpu=cortex-a9 MP_SCU.S
"${CC}" ${TARGET_FLAGS} -c -g -mcpu=cortex-a9 MP_Mutexes.S
"${CC}" ${TARGET_FLAGS} -c -g -mcpu=cortex-a9 MP_PrivateTimer.S
"${CC}" ${TARGET_FLAGS} -c -g -mcpu=cortex-a9 v7.S
"${CC}" ${TARGET_FLAGS} -g -mcpu=cortex-a9 -nostartfiles \
    -T sample_threadx.ld \
    ${ENTRY_FLAG} \
    ${SYSCALL_LIB} \
    -o sample_threadx.out MP_PrivateTimer.o MP_GIC.o MP_Mutexes.o MP_SCU.o \
    sample_threadx.o startup.o crt0.o v7.o tx.a \
    -Wl,-M > sample_threadx.map
