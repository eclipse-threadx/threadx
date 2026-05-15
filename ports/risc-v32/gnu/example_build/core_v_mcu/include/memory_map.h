/***************************************************************************/
/* Copyright (C) 2019 ETH Zurich, University of Bologna and GreenWaves Technologies
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

#ifndef MEMORY_MAP_H
#define MEMORY_MAP_H

#include "pulp_mem_map.h"

#define L2_BASE        0x1C000000UL
#define L2_SIZE        0x80000UL
#define L2_SHARED_ADDR L2_BASE
#define L2_SHARED_SIZE L2_SIZE
#define ROM_ADDR       0x1A000000UL
#define ROM_SIZE       0x00002000UL

#define SOC_PERIPHERALS_ADDR PULP_SOC_PERIPHERALS_ADDR
#define SOC_FLL_ADDR         PULP_FLL_ADDR
#define GPIO_ADDR            PULP_GPIO_ADDR
#define UDMA_CTRL_ADDR       PULP_UDMA_ADDR
#define APB_SOC_CTRL_ADDR    PULP_APB_SOC_CTRL_ADDR
#define ADV_TIMER_ADDR       PULP_ADV_TIMER_ADDR
#define SOC_EU_ADDR          PULP_SOC_EU_ADDR
#define FC_IRQ_ADDR          PULP_FC_IRQ_ADDR
#define FC_TIMER_ADDR        PULP_FC_TIMER_ADDR
#define FC_HWPE_ADDR         PULP_FC_HWPE_ADDR
#define STDOUT_ADDR          PULP_STDOUT_ADDR

#endif /* MEMORY_MAP_H */
