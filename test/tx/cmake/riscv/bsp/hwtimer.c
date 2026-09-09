/***************************************************************************
 * Copyright (c) 2026 10xEngineers
 *
 * This program and the accompanying materials are made available under the
 * terms of the MIT License which is available at
 * https://opensource.org/licenses/MIT.
 *
 * SPDX-License-Identifier: MIT
 **************************************************************************/

#include "csr.h"
#include "tx_api.h"
#include "hwtimer.h"

#define CLINT               (0x02000000L)
#define CLINT_TIME          (CLINT + 0xBFF8)
#define CLINT_TIMECMP(id)   (CLINT + 0x4000 + 8 * (id))

#if __riscv_xlen == 32

/* RV32 has no 64-bit load/store, so the 64-bit CLINT registers are
   accessed as two volatile 32-bit MMIO words.  */
#define MTIME_LO            (*(volatile uint32_t *)(CLINT_TIME))
#define MTIME_HI            (*(volatile uint32_t *)(CLINT_TIME + 4))
#define MTIMECMP_LO(id)     (*(volatile uint32_t *)(CLINT_TIMECMP(id)))
#define MTIMECMP_HI(id)     (*(volatile uint32_t *)(CLINT_TIMECMP(id) + 4))

/* RV32 mtime read: re-read the high word until it is stable, so a
   low-word rollover between the two loads cannot tear the value.  */
static uint64_t clint_time_read(void)
{
    uint32_t hi;
    uint32_t lo;

    do
    {
        hi = MTIME_HI;
        lo = MTIME_LO;
    } while (hi != MTIME_HI);

    return ((uint64_t)hi << 32) | lo;
}

/* Only this hart writes its own mtimecmp, so a plain two-word read
   cannot tear.  */
static uint64_t clint_timecmp_read(uintptr_t hart)
{
    uint32_t lo = MTIMECMP_LO(hart);
    uint32_t hi = MTIMECMP_HI(hart);

    return ((uint64_t)hi << 32) | lo;
}

/* RV32 mtimecmp write, safe sequence: park the low word at all-ones
   first, so no intermediate 64-bit value compares below mtime and
   raises a spurious timer interrupt.  */
static void clint_timecmp_write(uintptr_t hart, uint64_t value)
{
    MTIMECMP_LO(hart) = 0xFFFFFFFF;
    MTIMECMP_HI(hart) = (uint32_t)(value >> 32);
    MTIMECMP_LO(hart) = (uint32_t)value;
}

#else /* __riscv_xlen == 64 */

/* RV64: naturally aligned 64-bit MMIO accesses are single loads/stores.  */
static uint64_t clint_time_read(void)
{
    return *(volatile uint64_t *)CLINT_TIME;
}

static uint64_t clint_timecmp_read(uintptr_t hart)
{
    return *(volatile uint64_t *)CLINT_TIMECMP(hart);
}

static void clint_timecmp_write(uintptr_t hart, uint64_t value)
{
    *(volatile uint64_t *)CLINT_TIMECMP(hart) = value;
}

#endif /* __riscv_xlen */

int hwtimer_init(void)
{
    uintptr_t hart = riscv_get_core();

    clint_timecmp_write(hart, clint_time_read() + TICKNUM_PER_TIMER);
    return 0;
}

int hwtimer_handler(void)
{
    uintptr_t hart = riscv_get_core();

    /* Absolute re-arm: advance from the previous compare value, so trap
       latency does not accumulate as tick drift; catch up if the next
       compare already passed.  */
    uint64_t next = clint_timecmp_read(hart) + TICKNUM_PER_TIMER;
    uint64_t now = clint_time_read();

    if (next <= now)
        next = now + TICKNUM_PER_TIMER;

    clint_timecmp_write(hart, next);
    return 0;
}
