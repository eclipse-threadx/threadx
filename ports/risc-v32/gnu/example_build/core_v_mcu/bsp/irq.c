/***************************************************************************/
/* Copyright (C) 2019 ETH Zurich and University of Bologna
 * Copyright (C) 2026 Eclipse ThreadX contributors
 *
 * Derived from core-v-freertos (https://github.com/openhwgroup/core-v-freertos)
 * Original work licensed under the Apache License, Version 2.0.
 * See https://www.apache.org/licenses/LICENSE-2.0
 * Modifications licensed under MIT (https://opensource.org/licenses/MIT).
 *
 * AI Disclosure: Some portions generated or modified by Copilot (Sonnet 4.6).
 *
 * SPDX-License-Identifier: Apache-2.0 AND MIT
 ***************************************************************************/

#include <stdint.h>

#include "pulp_mem_map.h"
#include "io.h"
#include "irq.h"
#include "csr.h"

void irq_mask(uint32_t mask)
{
    writew(mask, (uintptr_t)(PULP_FC_IRQ_ADDR + IRQ_REG_MASK_OFFSET));
}

void irq_enable(uint32_t mask)
{
    writew(mask, (uintptr_t)(PULP_FC_IRQ_ADDR + IRQ_REG_MASK_SET_OFFSET));
}

void irq_disable(uint32_t mask)
{
    writew(mask, (uintptr_t)(PULP_FC_IRQ_ADDR + IRQ_REG_MASK_CLEAR_OFFSET));
}

uint32_t irq_clint_disable(void)
{
    return (uint32_t)csr_read_clear(CSR_MSTATUS, MSTATUS_IE);
}

uint32_t irq_clint_enable(void)
{
    return (uint32_t)csr_read_set(CSR_MSTATUS, MSTATUS_IE);
}

void pulp_irq_init(void)
{
    irq_disable(0xFFFFFFFFUL);
}
