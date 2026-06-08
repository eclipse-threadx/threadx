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
#include "hwtimer.h"
#include <tx_port.h>

#define CLINT               (0x02000000L)
#define CLINT_TIME          (CLINT + 0xBFF8)
#define CLINT_TIMECMP(id)   (CLINT + 0x4000 + 8 * (id))

int hwtimer_init(void)
{
    uintptr_t hart = riscv_get_core();
    uint64_t time = *((volatile uint64_t *)CLINT_TIME);
    *((volatile uint64_t *)CLINT_TIMECMP(hart)) = time + TICKNUM_PER_TIMER;
    return 0;
}

int hwtimer_handler(void)
{
    uintptr_t hart = riscv_get_core();
    uint64_t time = *((volatile uint64_t *)CLINT_TIME);
    *((volatile uint64_t *)CLINT_TIMECMP(hart)) = time + TICKNUM_PER_TIMER;
    return 0;
}
