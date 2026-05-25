/***************************************************************************
 * Copyright (c) 2026 10xEngineers
 *
 * This program and the accompanying materials are made available under the
 * terms of the MIT License which is available at
 * https://opensource.org/licenses/MIT.
 *
 * SPDX-License-Identifier: MIT
 **************************************************************************/

#include "plic.h"
#include <stddef.h>

irq_callback callbacks[MAX_CALLBACK_NUM];

void plic_irq_enable(int irqno)
{
    uintptr_t hart = riscv_get_core();
    volatile uint32_t *reg = (volatile uint32_t *)(PLIC_MENABLE(hart) + (irqno / 32) * 4);
    *reg = *reg | (1U << (irqno % 32));
}

void plic_irq_disable(int irqno)
{
    uintptr_t hart = riscv_get_core();
    volatile uint32_t *reg = (volatile uint32_t *)(PLIC_MENABLE(hart) + (irqno / 32) * 4);
    *reg = *reg & ~(1U << (irqno % 32));
}

void plic_prio_set(int irqno, int prio)
{
    PLIC_SET_PRIO(irqno, prio);
}

int plic_prio_get(int irqno)
{
    return (int)PLIC_GET_PRIO(irqno);
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
    for (int i = 0; i < MAX_CALLBACK_NUM; i++)
    {
        callbacks[i] = NULL;
    }
    return 0;
}

int plic_claim(void)
{
    uintptr_t hart = riscv_get_core();
    return (int)(*(uint32_t *)PLIC_MCLAIM(hart));
}

void plic_complete(int irqno)
{
    uintptr_t hart = riscv_get_core();
    *(uint32_t *)(PLIC_MCOMPLETE(hart)) = (uint32_t)irqno;
}

int plic_irq_intr(void)
{
    int ret = -1;
    int irqno = plic_claim();
    if (irqno <= 0 || irqno >= MAX_CALLBACK_NUM) {
        plic_complete(irqno);
        return 0;  // spurious or out-of-range, not an error
    }
    if (callbacks[irqno] != NULL)
        ret = (callbacks[irqno])(irqno);
    
    plic_complete(irqno);
    return ret;
}
