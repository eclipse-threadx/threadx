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

#include <stddef.h>
#include <stdint.h>

#include "bits.h"
#include "csr.h"
#include "fll.h"
#include "irq.h"
#include "properties.h"
#include "system_core_v_mcu.h"

#define FLL_LOG2_MAXDCO 29U
#define FLL_LOG2_REFCLK ARCHI_REF_CLOCK_LOG2
#define FLL_LOG2_MAXM   (FLL_LOG2_MAXDCO - FLL_LOG2_REFCLK)

static volatile uint32_t flls_frequency[FLL_NUM];

static uint32_t fll_max_u32(uint32_t a, uint32_t b)
{
    return (a > b) ? a : b;
}

static uint32_t fll_fl1(uint32_t value)
{
    return (uint32_t)(31U - (uint32_t)__builtin_clz(value));
}

static uint32_t fll_get_mult_div_from_frequency(uint32_t freq, uint32_t *mult,
                                                uint32_t *div)
{
    uint32_t fref = FLL_REF_CLK;
    uint32_t log2_m;
    uint32_t d;
    uint32_t m;
    uint32_t fres;

    if ((freq == 0U) || (mult == NULL) || (div == NULL))
    {
        if (mult != NULL)
        {
            *mult = 0U;
        }
        if (div != NULL)
        {
            *div = 0U;
        }
        return 0U;
    }

    if (freq > fref)
    {
        log2_m = fll_fl1(freq) - fll_fl1(fref);
    }
    else
    {
        log2_m = 0U;
    }

    if (FLL_LOG2_MAXM > log2_m)
    {
        d = fll_max_u32(1U, (FLL_LOG2_MAXM - log2_m) >> 1U);
    }
    else
    {
        d = 1U;
    }

    m = (freq << d) / fref;
    fres = (fref * m + (1UL << (d - 1U))) >> d;

    *mult = m;
    *div = d + 1U;
    return fres;
}

static uint32_t fll_get_frequency_from_mult_div(uint32_t mult, uint32_t div)
{
    uint32_t fref = FLL_REF_CLK;

    if (div == 0U)
    {
        return fref * mult;
    }

    return (fref * mult) >> (div - 1U);
}

int pi_fll_set_frequency(fll_type_t which_fll, uint32_t frequency, int check)
{
    uint32_t mult;
    uint32_t div;
    uint32_t reg1;
    uint32_t saved_irq;

    (void)check;

    if ((uint32_t)which_fll >= FLL_NUM)
    {
        return -1;
    }

    if (frequency == 0U)
    {
        return -1;
    }

    saved_irq = irq_clint_disable();
    (void)fll_get_mult_div_from_frequency(frequency, &mult, &div);

    reg1 = FLL_CTRL[which_fll].FLL_CONF1;
    reg1 &= ~FLL_CTRL_CONF1_MULTI_FACTOR_MASK;
    reg1 |= REG_SET(FLL_CTRL_CONF1_MULTI_FACTOR, mult);
    reg1 &= ~FLL_CTRL_CONF1_CLK_OUT_DIV_MASK;
    reg1 |= REG_SET(FLL_CTRL_CONF1_CLK_OUT_DIV, div);
    FLL_CTRL[which_fll].FLL_CONF1 = reg1;

    flls_frequency[which_fll] = frequency;
    if (which_fll == FLL_SOC)
    {
        system_core_clock = frequency;
    }

    if ((saved_irq & MSTATUS_IE) != 0U)
    {
        (void)irq_clint_enable();
    }

    return (int)frequency;
}

void pi_fll_init(fll_type_t which_fll, uint32_t ret_state)
{
    uint32_t reg1;

    if ((uint32_t)which_fll >= FLL_NUM)
    {
        return;
    }

    if (ret_state != 0U)
    {
        (void)pi_fll_get_frequency(which_fll, 1U);
        return;
    }

    reg1 = FLL_CTRL[which_fll].FLL_CONF1;
    if (REG_GET(FLL_CTRL_CONF1_MODE, reg1) == 0U)
    {
        uint32_t reg2 = FLL_CTRL[which_fll].FLL_CONF2;
        uint32_t regint = FLL_CTRL[which_fll].FLL_INTEGRATOR;

        reg2 &= ~FLL_CTRL_CONF2_ASSERT_CYCLES_MASK;
        reg2 |= REG_SET(FLL_CTRL_CONF2_ASSERT_CYCLES, 0x6U);
        reg2 &= ~FLL_CTRL_CONF2_LOCK_TOLERANCE_MASK;
        reg2 |= REG_SET(FLL_CTRL_CONF2_LOCK_TOLERANCE, 0x50U);
        FLL_CTRL[which_fll].FLL_CONF2 = reg2;

        regint &= ~FLL_CTRL_INTEGRATOR_INT_PART_MASK;
        regint |= REG_SET(FLL_CTRL_INTEGRATOR_INT_PART, 332U);
        FLL_CTRL[which_fll].FLL_INTEGRATOR = regint;

        reg1 &= ~FLL_CTRL_CONF1_OUTPUT_LOCK_EN_MASK;
        reg1 |= REG_SET(FLL_CTRL_CONF1_OUTPUT_LOCK_EN, 1U);
        reg1 &= ~FLL_CTRL_CONF1_MODE_MASK;
        reg1 |= REG_SET(FLL_CTRL_CONF1_MODE, 1U);
        FLL_CTRL[which_fll].FLL_CONF1 = reg1;
    }

    if (flls_frequency[which_fll] != 0U)
    {
        (void)pi_fll_set_frequency(which_fll, flls_frequency[which_fll], 0);
    }
    else
    {
        flls_frequency[which_fll] = fll_get_frequency_from_mult_div(
            REG_GET(FLL_CTRL_CONF1_MULTI_FACTOR, reg1),
            REG_GET(FLL_CTRL_CONF1_CLK_OUT_DIV, reg1));
    }
}

int pi_fll_get_frequency(fll_type_t which_fll, uint8_t real)
{
    if ((uint32_t)which_fll >= FLL_NUM)
    {
        return -1;
    }

    if (real != 0U)
    {
        flls_frequency[which_fll] = fll_get_frequency_from_mult_div(
            FLL_CTRL[which_fll].FLL_STATUS,
            REG_GET(FLL_CTRL_CONF1_CLK_OUT_DIV, FLL_CTRL[which_fll].FLL_CONF1));
    }

    return (int)flls_frequency[which_fll];
}
