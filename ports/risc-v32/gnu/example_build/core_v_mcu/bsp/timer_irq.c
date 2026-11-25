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
#include "timer.h"
#include "timer_irq.h"

static uint32_t last_count;

int timer_irq_init(uint32_t ticks)
{
    (void)timer_irq_set_timeout(ticks);

    writew(TIMER_CFG_LO_ENABLE_MASK |
           TIMER_CFG_LO_RESET_MASK |
           TIMER_CFG_LO_MODE_MASK |
           TIMER_CFG_LO_IRQEN_MASK,
           (uintptr_t)(PULP_FC_TIMER_ADDR + TIMER_CFG_LO_OFFSET));

    last_count = 0U;
    return 0;
}

int timer_irq_set_timeout(uint32_t ticks)
{
    writew(1U, (uintptr_t)(PULP_FC_TIMER_ADDR + TIMER_RESET_LO_OFFSET));
    writew(ticks, (uintptr_t)(PULP_FC_TIMER_ADDR + TIMER_CMP_LO_OFFSET));
    last_count = 0U;
    return 0;
}

uint32_t timer_irq_clock_elapsed(void)
{
    uint32_t current = timer_irq_cycle_get_32();
    uint32_t elapsed = current - last_count;

    last_count = current;
    return elapsed;
}

uint32_t timer_irq_cycle_get_32(void)
{
    return readw((uintptr_t)(PULP_FC_TIMER_ADDR + TIMER_CNT_LO_OFFSET));
}
