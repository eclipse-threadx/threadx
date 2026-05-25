/***************************************************************************
 * Copyright (c) 2025 10xEngineers
 *
 * This program and the accompanying materials are made available under the
 * terms of the MIT License which is available at
 * https://opensource.org/licenses/MIT.
 *
 * SPDX-License-Identifier: MIT
 **************************************************************************/

#include "plic.h"
#include <stddef.h>

static irq_callback callbacks[MAX_CALLBACK_NUM];

#define PLIC_ENABLE(hart)    PLIC_SENABLE(hart)
#define PLIC_PRIORITY_REG(hart) PLIC_SPRIORITY(hart)
#define PLIC_CLAIM_REG(hart) PLIC_SCLAIM(hart)
#define PLIC_COMPLETE_REG(hart) PLIC_SCOMPLETE(hart)

void plic_irq_enable(int irqno)
{
    int hart = (int)riscv_get_core();
    uint32_t word = irqno / 32;
    uint32_t bit  = irqno % 32;
    volatile uint32_t *en = (volatile uint32_t *)(PLIC_ENABLE(hart) + word * 4);
    *en |= (1u << bit);
}

void plic_irq_disable(int irqno)
{
    int hart = (int)riscv_get_core();
    uint32_t word = irqno / 32;
    uint32_t bit  = irqno % 32;
    volatile uint32_t *en = (volatile uint32_t *)(PLIC_ENABLE(hart) + word * 4);
    *en &= ~(1u << bit);
}

void plic_prio_set(int irqno, int prio)
{
    PLIC_SET_PRIO(irqno, prio);
}

int plic_prio_get(int irqno)
{
    return PLIC_GET_PRIO(irqno);
}

int plic_register_callback(int irqno, irq_callback callback)
{
    if (!(irqno >= 0 && irqno < MAX_CALLBACK_NUM))
        return -1;
    callbacks[irqno] = callback;
    return 0;
}

int plic_unregister_callback(int irqno)
{
    return plic_register_callback(irqno, NULL);
}

int plic_init(void)
{
    int hart = (int)riscv_get_core();

    for (int i = 0; i < MAX_CALLBACK_NUM; i++)
        callbacks[i] = NULL;

    /* Mask everything for this hart. */
    for (int word = 0; word < (MAX_CALLBACK_NUM + 31) / 32; word++)
        *(volatile uint32_t *)(PLIC_ENABLE(hart) + word * 4) = 0;

    /* Set hart threshold to 0 so any non-zero priority IRQ can fire. */
    *(volatile uint32_t *)PLIC_PRIORITY_REG(hart) = 0;

    /*
     * Drain stale pending interrupts left over from a prior boot stage
     * (BootROM / OpenSBI / U-Boot).  We temporarily enable every source
     * so claim returns the actual highest-priority pending ID, then
     * complete whatever was claimed.  Loop until claim returns 0
     * (no more pending).  This follows the PLIC spec: claim returns 0
     * when nothing is pending for this context.
     */
    for (int word = 0; word < (PLIC_NUM_SOURCES + 31) / 32; word++)
        *(volatile uint32_t *)(PLIC_ENABLE(hart) + word * 4) = 0xFFFFFFFFu;

    for (;;) {
        uint32_t id = *(volatile uint32_t *)PLIC_CLAIM_REG(hart);
        if (id == 0)
            break;
        *(volatile uint32_t *)PLIC_COMPLETE_REG(hart) = id;
    }

    /* Re-mask everything; individual drivers will enable their sources. */
    for (int word = 0; word < (PLIC_NUM_SOURCES + 31) / 32; word++)
        *(volatile uint32_t *)(PLIC_ENABLE(hart) + word * 4) = 0;

    /*
     * Set default priority for every source to PLIC_DEFAULT_PRIORITY (2),
     * Priority 0 means "never pending" per the SiFive PLIC spec, so any 
     * source that should be active must have priority >= 1.  Individual 
     * drivers may override this with plic_prio_set() later.
     */
    for (int i = 1; i <= PLIC_NUM_SOURCES; i++)
        PLIC_SET_PRIO(i, PLIC_DEFAULT_PRIORITY);

    return 0;
}

int plic_claim(void)
{
    int hart = (int)riscv_get_core();
    return (int)*(volatile uint32_t *)PLIC_CLAIM_REG(hart);
}

void plic_complete(int irqno)
{
    int hart = (int)riscv_get_core();
    *(volatile uint32_t *)PLIC_COMPLETE_REG(hart) = (uint32_t)irqno;
}

int plic_irq_intr(void)
{
    int ret = -1;
    int irqno = plic_claim();
    if (irqno == 0)
        return ret;
    if (irqno < MAX_CALLBACK_NUM && callbacks[irqno] != NULL)
        ret = (callbacks[irqno])(irqno);
    plic_complete(irqno);
    return ret;
}
