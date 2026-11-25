/***************************************************************************/
/* Copyright 2020 GreenWaves Technologies
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

#ifndef FLL_H
#define FLL_H

#include <stdint.h>

#include "properties.h"
#include "pulp_mem_map.h"

#define FLL_STATUS_OFFSET       0x000U
#define FLL_CONF1_OFFSET        0x004U
#define FLL_CONF2_OFFSET        0x008U
#define FLL_INTEGRATOR_OFFSET   0x00CU

typedef struct
{
    volatile uint32_t FLL_STATUS;
    volatile uint32_t FLL_CONF1;
    volatile uint32_t FLL_CONF2;
    volatile uint32_t FLL_INTEGRATOR;
} fll_ctrl_t;

typedef enum _fll_type
{
    FLL_SOC = 0,
    FLL_PERI = 1,
    FLL_CLUSTER = 2
} fll_type_t;

typedef enum
{
    PI_FREQ_DOMAIN_FC = 0,
    PI_FREQ_DOMAIN_CL = 1,
    PI_FREQ_DOMAIN_PERIPH = 2
} pi_freq_domain_e;

#define FLL_CTRL_STATUS_MULTI_FACTOR_MASK   0xFFFFU
#define FLL_CTRL_STATUS_MULTI_FACTOR_SHIFT  0U
#define FLL_CTRL_CONF1_MULTI_FACTOR_MASK    0xFFFFU
#define FLL_CTRL_CONF1_MULTI_FACTOR_SHIFT   0U
#define FLL_CTRL_CONF1_DCO_INPUT_MASK       0x03FF0000U
#define FLL_CTRL_CONF1_DCO_INPUT_SHIFT      16U
#define FLL_CTRL_CONF1_CLK_OUT_DIV_MASK     0x3C000000U
#define FLL_CTRL_CONF1_CLK_OUT_DIV_SHIFT    26U
#define FLL_CTRL_CONF1_OUTPUT_LOCK_EN_MASK  0x40000000U
#define FLL_CTRL_CONF1_OUTPUT_LOCK_EN_SHIFT 30U
#define FLL_CTRL_CONF1_MODE_MASK            0x80000000U
#define FLL_CTRL_CONF1_MODE_SHIFT           31U
#define FLL_CTRL_CONF2_LOOPGAIN_MASK        0xFU
#define FLL_CTRL_CONF2_LOOPGAIN_SHIFT       0U
#define FLL_CTRL_CONF2_DEASSERT_CYCLES_MASK 0x3F0U
#define FLL_CTRL_CONF2_DEASSERT_CYCLES_SHIFT 4U
#define FLL_CTRL_CONF2_ASSERT_CYCLES_MASK   0xFC00U
#define FLL_CTRL_CONF2_ASSERT_CYCLES_SHIFT  10U
#define FLL_CTRL_CONF2_LOCK_TOLERANCE_MASK  0x0FFF0000U
#define FLL_CTRL_CONF2_LOCK_TOLERANCE_SHIFT 16U
#define FLL_CTRL_CONF2_CONF_CLK_SEL_MASK    0x20000000U
#define FLL_CTRL_CONF2_CONF_CLK_SEL_SHIFT   29U
#define FLL_CTRL_CONF2_OPEN_LOOP_MASK       0x40000000U
#define FLL_CTRL_CONF2_OPEN_LOOP_SHIFT      30U
#define FLL_CTRL_CONF2_DITHERING_MASK       0x80000000U
#define FLL_CTRL_CONF2_DITHERING_SHIFT      31U
#define FLL_CTRL_INTEGRATOR_FRACT_PART_MASK 0x0000FFC0U
#define FLL_CTRL_INTEGRATOR_FRACT_PART_SHIFT 6U
#define FLL_CTRL_INTEGRATOR_INT_PART_MASK   0x03FF0000U
#define FLL_CTRL_INTEGRATOR_INT_PART_SHIFT  16U
#define FLL_CTRL_SOC_FLL_CONV_MASK          0x1U
#define FLL_CTRL_SOC_FLL_CONV_SHIFT         0U
#define FLL_CTRL_CLUSTER_FLL_CONV_MASK      0x2U
#define FLL_CTRL_CLUSTER_FLL_CONV_SHIFT     1U

#define FLL_NUM     ARCHI_NB_FLL
#define FLL_REF_CLK ARCHI_REF_CLOCK
#define FLL_CTRL    ((volatile fll_ctrl_t *)PULP_FLL_ADDR)

void pi_fll_init(fll_type_t which_fll, uint32_t ret_state);
int pi_fll_set_frequency(fll_type_t which_fll, uint32_t frequency, int check);
int pi_fll_get_frequency(fll_type_t which_fll, uint8_t real);

#endif /* FLL_H */
