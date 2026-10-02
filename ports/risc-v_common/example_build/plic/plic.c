/***************************************************************************
 * Copyright (c) 2024 Microsoft Corporation
 * Copyright (c) 2026-present Eclipse ThreadX contributors
 *
 * This program and the accompanying materials are made available under the
 * terms of the MIT License which is available at
 * https://opensource.org/licenses/MIT.
 *
 * SPDX-License-Identifier: MIT
 **************************************************************************/

/* PLIC driver for the QEMU virt machine (RV32 and RV64), 
    M-mode context. */

#include "plic.h"
#include <stddef.h>

irq_callback callbacks[MAX_CALLBACK_NUM];

/* Enable word that holds source irqno for this hart's M-mode context.  */
static volatile uint32_t *plic_enable_word(int hart, int irqno)
{
    return (volatile uint32_t *)(PLIC_MENABLE(hart) + ((unsigned int)irqno / 32u) * 4u);
}

void plic_irq_enable(int irqno)
{
    int hart = riscv_get_core();
    volatile uint32_t *reg;

    if ((irqno <= 0) || (irqno >= MAX_CALLBACK_NUM))
        return;

    reg = plic_enable_word(hart, irqno);
    *reg = *reg | (1u << ((unsigned int)irqno % 32u));
}

void plic_irq_disable(int irqno)
{
    int hart = riscv_get_core();
    volatile uint32_t *reg;

    if ((irqno <= 0) || (irqno >= MAX_CALLBACK_NUM))
        return;

    reg = plic_enable_word(hart, irqno);
    *reg = *reg & ~(1u << ((unsigned int)irqno % 32u));
}

void plic_prio_set(int irqno, int prio)
{
    PLIC_SET_PRIO(irqno, prio);
}

int  plic_prio_get(int irqno)
{
    return (int)PLIC_GET_PRIO(irqno);
}

int  plic_register_callback(int irqno, irq_callback callback)
{
    if (!(irqno >= 0 && irqno < MAX_CALLBACK_NUM))
        return -1;
    callbacks[irqno] = callback;
    return 0;
}

int  plic_unregister_callback(int irqno)
{
    return plic_register_callback(irqno, NULL);
}

int  plic_init(void)
{
    int hart = riscv_get_core();

    for (int i = 0; i < MAX_CALLBACK_NUM; i++)
        callbacks[i] = NULL;

    /* Do not depend on the reset state or on a prior boot stage: accept
       every priority (threshold 0) and start with all sources for this
       hart disabled.  Drivers enable their own sources.  */
    *(volatile uint32_t *)PLIC_MPRIORITY(hart) = 0;
    for (int w = 0; w < MAX_CALLBACK_NUM / 32; w++)
        *(volatile uint32_t *)(PLIC_MENABLE(hart) + (unsigned int)w * 4u) = 0;

    return 0;
}

int  plic_claim(void)
{
    int hart = riscv_get_core();
    return (int)(*(volatile uint32_t *)PLIC_MCLAIM(hart));
}

void plic_complete(int irqno)
{
    int hart = riscv_get_core();
    *(volatile uint32_t *)(PLIC_MCOMPLETE(hart)) = (uint32_t)irqno;
}

int  plic_irq_intr(void)
{
    int ret = -1;
    int irqno = plic_claim();

    if (irqno == 0)
        return 0;

    if ((irqno < 0) || (irqno >= MAX_CALLBACK_NUM))
    {
        if (irqno > 0)
            plic_complete(irqno);
        return -1;
    }

    if (callbacks[irqno] != NULL)
        ret = (callbacks[irqno])(irqno);

    plic_complete(irqno);
    return ret;
}
