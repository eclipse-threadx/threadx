/* clz.c — portable fallback for __clzsi2 (count leading zeros, 32-bit)
 *
 * The riscv-collab bare-metal toolchain (riscv64-unknown-elf from /opt/riscv)
 * is built without multilib and does not ship an rv32/ilp32 libgcc.  As a
 * result __clzsi2, which GCC emits for __builtin_clz() on targets without a
 * hardware CLZ instruction, is missing at link time.
 *
 * This file provides a weak definition so that the build is self-contained
 * with any RISC-V bare-metal toolchain.  When a toolchain does ship the symbol
 * in libgcc (e.g. the Ubuntu gcc-riscv64-unknown-elf package), the libgcc copy
 * will take precedence over this weak one.
 */

#include <stdint.h>

__attribute__((weak))
int __clzsi2(uint32_t x)
{
    if (x == 0U)
        return 32;

    int n = 0;
    if ((x & 0xFFFF0000U) == 0U) { n += 16; x <<= 16; }
    if ((x & 0xFF000000U) == 0U) { n +=  8; x <<=  8; }
    if ((x & 0xF0000000U) == 0U) { n +=  4; x <<=  4; }
    if ((x & 0xC0000000U) == 0U) { n +=  2; x <<=  2; }
    if ((x & 0x80000000U) == 0U) { n +=  1; }
    return n;
}
