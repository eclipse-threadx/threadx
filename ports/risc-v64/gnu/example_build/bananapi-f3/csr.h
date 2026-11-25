/***************************************************************************
 * Copyright (c) 2026 10xEngineers
 *
 * This program and the accompanying materials are made available under the
 * terms of the MIT License which is available at
 * https://opensource.org/licenses/MIT.
 *
 * SPDX-License-Identifier: MIT
 **************************************************************************/

/* RISC-V S-mode CSR helpers
 *
 * Privilege level:  Supervisor (S-mode).
 * Reference:        RISC-V Privileged Specification
 *
 * Bit positions per the RISC-V Privileged Specification:
 *
 *   sstatus:  SIE(1)  SPIE(5)  SPP(8)  FS(13:14)  SUM(18)  MXR(19)
 *   sie:      SSIE(1) STIE(5)  SEIE(9)
 *   scause:   Interrupt bit = 63;  Code 1=SSI, 5=STI, 9=SEI
 */

#ifndef RISCV_CSR_H
#define RISCV_CSR_H

#define SSTATUS_SIE             (1L << 1)    /* Supervisor Interrupt Enable     */
#define SSTATUS_SPIE            (1L << 5)    /* Previous SIE (saved on trap)    */
#define SSTATUS_SPP_MASK        (1L << 8)
#define SSTATUS_SPP_S           (1L << 8)    /* SPP = Supervisor                */
#define SSTATUS_SPP_U           (0L << 8)    /* SPP = User                      */
#define SSTATUS_FS              (3L << 13)   /* FP unit state (Off/Init/Clean/Dirty) */

#define SIE_SSIE                (1L << 1)    /* S-mode software interrupt       */
#define SIE_STIE                (1L << 5)    /* S-mode timer interrupt          */
#define SIE_SEIE                (1L << 9)    /* S-mode external interrupt       */

#ifndef __ASSEMBLER__

#include <stdint.h>

/*
 * Return the hart ID of the running core.
 *
 * mhartid is an M-mode CSR and cannot be read from S-mode.  When booted
 * from U-Boot only hart 0 is active (secondary harts remain parked in
 * OpenSBI HSM), so we return 0.  An SMP extension would need to pass
 * the hart ID through a0 or shared memory at boot.
 */
static inline uint64_t riscv_get_core(void)
{
    return 0;
}


static inline uint64_t riscv_read_sstatus(void)
{
    uint64_t x;
    asm volatile("csrr %0, sstatus" : "=r" (x));
    return x;
}

static inline void riscv_write_sstatus(uint64_t x)
{
    asm volatile("csrw sstatus, %0" : : "r" (x));
}


static inline void riscv_sintr_on(void)
{
    riscv_write_sstatus(riscv_read_sstatus() | SSTATUS_SIE);
}

static inline void riscv_sintr_off(void)
{
    riscv_write_sstatus(riscv_read_sstatus() & ~SSTATUS_SIE);
}

static inline int riscv_sintr_get(void)
{
    return (riscv_read_sstatus() & SSTATUS_SIE) != 0;
}

static inline void riscv_sintr_restore(int enabled)
{
    if (enabled)
        riscv_sintr_on();
    else
        riscv_sintr_off();
}

/* Unified names used by BSP drivers (uart.c, etc.). */
#define riscv_intr_on       riscv_sintr_on
#define riscv_intr_off      riscv_sintr_off
#define riscv_intr_get      riscv_sintr_get
#define riscv_intr_restore  riscv_sintr_restore

#endif /* __ASSEMBLER__ */
#endif /* RISCV_CSR_H */
