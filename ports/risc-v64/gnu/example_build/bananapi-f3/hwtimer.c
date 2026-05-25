/***************************************************************************
 * Copyright (c) 2026 10xEngineers
 *
 * This program and the accompanying materials are made available under the
 * terms of the MIT License which is available at
 * https://opensource.org/licenses/MIT.
 *
 * SPDX-License-Identifier: MIT
 **************************************************************************/

#include "tx_port.h"
#include "csr.h"
#include "hwtimer.h"

/*
 * SBI legacy set_timer ecall
 *
 * EID (a7) = 0  (SBI_SET_TIMER)
 * a0       = absolute mtime compare value
 *
 * Programs mtimecmp for the current hart and clears sip.STIP.
 */
static inline void sbi_set_timer(uint64_t stime_value)
{
    register uint64_t a0 asm("a0") = stime_value;
    register uint64_t a7 asm("a7") = 0;  /* SBI_SET_TIMER */
    asm volatile("ecall"
                 : "+r"(a0)
                 : "r"(a7)
                 : "memory");
}

/*
 * Read the free-running mtime counter via the rdtime pseudo-instruction.
 * Accessible from S-mode per RISC-V Priv Spec §10.1 (Zicntr extension).
 */
static inline uint64_t read_time(void)
{
    uint64_t t;
    asm volatile("rdtime %0" : "=r"(t));
    return t;
}

int hwtimer_init(void)
{
    uint64_t now = read_time();
    sbi_set_timer(now + TICKNUM_PER_TIMER);
    return 0;
}

int hwtimer_handler(void)
{
    uint64_t now = read_time();
    sbi_set_timer(now + TICKNUM_PER_TIMER);
    return 0;
}
