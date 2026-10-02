/***************************************************************************/
/* Copyright (c) 2024 Microsoft Corporation                                */
/* Copyright (c) 2026 Eclipse ThreadX contributors                         */
/*                                                                         */
/* This program and the accompanying materials are made available under    */
/* the terms of the MIT License which is available at                      */
/* https://opensource.org/licenses/MIT.                                    */
/*                                                                         */
/* SPDX-License-Identifier: MIT                                            */
/***************************************************************************/

#include "tx_api.h"
#include "csr.h"
#include "hwtimer.h"

#define CLINT		       (0x02000000L)
#define CLINT_TIME             (CLINT+0xBFF8)
#define CLINT_TIMECMP(hart_id) (CLINT+0x4000+8*(hart_id))

/* RV64: naturally aligned 64-bit MMIO accesses are single loads/stores;
   volatile keeps the compiler from caching or reordering them.  */
#define MTIME               (*(volatile uint64_t *)CLINT_TIME)
#define MTIMECMP(hart)      (*(volatile uint64_t *)CLINT_TIMECMP(hart))

int hwtimer_init(void)
{
    int hart = riscv_get_core();

    MTIMECMP(hart) = MTIME + TICKNUM_PER_TIMER;
    return 0;
}

int hwtimer_handler(void)
{
    int hart = riscv_get_core();

    /* Advance from the previous compare value, so trap latency 
    does not accumulate as tick drift.  */
    uint64_t next = MTIMECMP(hart) + TICKNUM_PER_TIMER;
    uint64_t now = MTIME;

    if (next <= now)
        next = now + TICKNUM_PER_TIMER;

    MTIMECMP(hart) = next;
    return 0;
}
