/***************************************************************************
 * Copyright (c) 2026 10xEngineers
 *
 * This program and the accompanying materials are made available under the
 * terms of the MIT License which is available at
 * https://opensource.org/licenses/MIT.
 *
 * SPDX-License-Identifier: MIT
 **************************************************************************/

#ifndef RISCV_CSR_H
#define RISCV_CSR_H

/* Machine Status Register, mstatus */
#define MSTATUS_MPP_MASK    (3L << 11)
#define MSTATUS_MPP_M       (3L << 11)
#define MSTATUS_MPP_S       (1L << 11)
#define MSTATUS_MPP_U       (0L << 11)
#define MSTATUS_MIE         (1L << 3)
#define MSTATUS_MPIE        (1L << 7)
#define MSTATUS_FS          (1L << 13)

/* Machine-mode Interrupt Enable */
#define MIE_MTIE            (1L << 7)
#define MIE_MSIE            (1L << 3)
#define MIE_MEIE            (1L << 11)
#define MIE_STIE            (1L << 5)
#define MIE_SSIE            (1L << 1)
#define MIE_SEIE            (1L << 9)

#ifndef __ASSEMBLER__

#include <stdint.h>

static inline uintptr_t riscv_get_core(void)
{
    uintptr_t x;
    __asm__ volatile("csrr %0, mhartid" : "=r" (x));
    return x;
}

static inline uintptr_t riscv_get_mstatus(void)
{
    uintptr_t x;
    __asm__ volatile("csrr %0, mstatus" : "=r" (x));
    return x;
}

static inline void riscv_writ_mstatus(uintptr_t x)
{
    __asm__ volatile("csrw mstatus, %0" : : "r" (x));
}

static inline void riscv_mintr_on(void)
{
    uintptr_t mstatus = riscv_get_mstatus();
    mstatus |= MSTATUS_MIE;
    riscv_writ_mstatus(mstatus);
}

static inline void riscv_mintr_off(void)
{
    uintptr_t mstatus = riscv_get_mstatus();
    mstatus &= (~MSTATUS_MIE);
    riscv_writ_mstatus(mstatus);
}

static inline int riscv_mintr_get(void)
{
    uintptr_t x = riscv_get_mstatus();
    return (x & MSTATUS_MIE) != 0;
}

static inline void riscv_mintr_restore(int x)
{
    if (x)
        riscv_mintr_on();
    else
        riscv_mintr_off();
}

#endif /* __ASSEMBLER__ */

#endif /* RISCV_CSR_H */
