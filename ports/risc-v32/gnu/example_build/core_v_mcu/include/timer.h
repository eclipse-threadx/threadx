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

#ifndef TIMER_H
#define TIMER_H

#include "bits.h"

#define TIMER_CFG_LO_OFFSET   0x0U
#define TIMER_CFG_HI_OFFSET   0x4U
#define TIMER_CNT_LO_OFFSET   0x8U
#define TIMER_CNT_HI_OFFSET   0xCU
#define TIMER_CMP_LO_OFFSET   0x10U
#define TIMER_CMP_HI_OFFSET   0x14U
#define TIMER_START_LO_OFFSET 0x18U
#define TIMER_START_HI_OFFSET 0x1CU
#define TIMER_RESET_LO_OFFSET 0x20U
#define TIMER_RESET_HI_OFFSET 0x24U

#define TIMER_CFG_LO_ENABLE_BIT   0U
#define TIMER_CFG_LO_ENABLE_WIDTH 1U
#define TIMER_CFG_LO_ENABLE_MASK  0x1U
#define TIMER_CFG_LO_RESET_BIT    1U
#define TIMER_CFG_LO_RESET_WIDTH  1U
#define TIMER_CFG_LO_RESET_MASK   0x2U
#define TIMER_CFG_LO_IRQEN_BIT    2U
#define TIMER_CFG_LO_IRQEN_WIDTH  1U
#define TIMER_CFG_LO_IRQEN_MASK   0x4U
#define TIMER_CFG_LO_IEM_BIT      3U
#define TIMER_CFG_LO_IEM_WIDTH    1U
#define TIMER_CFG_LO_IEM_MASK     0x8U
#define TIMER_CFG_LO_MODE_BIT     4U
#define TIMER_CFG_LO_MODE_WIDTH   1U
#define TIMER_CFG_LO_MODE_MASK    0x10U
#define TIMER_CFG_LO_ONE_S_BIT    5U
#define TIMER_CFG_LO_ONE_S_WIDTH  1U
#define TIMER_CFG_LO_ONE_S_MASK   0x20U
#define TIMER_CFG_LO_PEN_BIT      6U
#define TIMER_CFG_LO_PEN_WIDTH    1U
#define TIMER_CFG_LO_PEN_MASK     0x40U
#define TIMER_CFG_LO_CCFG_BIT     7U
#define TIMER_CFG_LO_CCFG_WIDTH   1U
#define TIMER_CFG_LO_CCFG_MASK    0x80U
#define TIMER_CFG_LO_PVAL_BIT     8U
#define TIMER_CFG_LO_PVAL_WIDTH   8U
#define TIMER_CFG_LO_PVAL_MASK    0xFF00U
#define TIMER_CFG_LO_CASC_BIT     31U
#define TIMER_CFG_LO_CASC_WIDTH   1U
#define TIMER_CFG_LO_CASC_MASK    0x80000000UL

#define TIMER_CFG_HI_ENABLE_BIT   0U
#define TIMER_CFG_HI_ENABLE_WIDTH 1U
#define TIMER_CFG_HI_ENABLE_MASK  0x1U
#define TIMER_CFG_HI_RESET_BIT    1U
#define TIMER_CFG_HI_RESET_WIDTH  1U
#define TIMER_CFG_HI_RESET_MASK   0x2U
#define TIMER_CFG_HI_IRQEN_BIT    2U
#define TIMER_CFG_HI_IRQEN_WIDTH  1U
#define TIMER_CFG_HI_IRQEN_MASK   0x4U
#define TIMER_CFG_HI_IEM_BIT      3U
#define TIMER_CFG_HI_IEM_WIDTH    1U
#define TIMER_CFG_HI_IEM_MASK     0x8U
#define TIMER_CFG_HI_MODE_BIT     4U
#define TIMER_CFG_HI_MODE_WIDTH   1U
#define TIMER_CFG_HI_MODE_MASK    0x10U
#define TIMER_CFG_HI_ONE_S_BIT    5U
#define TIMER_CFG_HI_ONE_S_WIDTH  1U
#define TIMER_CFG_HI_ONE_S_MASK   0x20U
#define TIMER_CFG_HI_PEN_BIT      6U
#define TIMER_CFG_HI_PEN_WIDTH    1U
#define TIMER_CFG_HI_PEN_MASK     0x40U
#define TIMER_CFG_HI_CLKCFG_BIT   7U
#define TIMER_CFG_HI_CLKCFG_WIDTH 1U
#define TIMER_CFG_HI_CLKCFG_MASK  0x80U

#define TIMER_CNT_LO_CNT_LO_BIT   0U
#define TIMER_CNT_LO_CNT_LO_WIDTH 32U
#define TIMER_CNT_LO_CNT_LO_MASK  0xFFFFFFFFUL
#define TIMER_CNT_HI_CNT_HI_BIT   0U
#define TIMER_CNT_HI_CNT_HI_WIDTH 32U
#define TIMER_CNT_HI_CNT_HI_MASK  0xFFFFFFFFUL
#define TIMER_CMP_LO_CMP_LO_BIT   0U
#define TIMER_CMP_LO_CMP_LO_WIDTH 32U
#define TIMER_CMP_LO_CMP_LO_MASK  0xFFFFFFFFUL
#define TIMER_CMP_HI_CMP_HI_BIT   0U
#define TIMER_CMP_HI_CMP_HI_WIDTH 32U
#define TIMER_CMP_HI_CMP_HI_MASK  0xFFFFFFFFUL

#define TIMER_START_LO_STRT_LO_BIT   0U
#define TIMER_START_LO_STRT_LO_WIDTH 1U
#define TIMER_START_LO_STRT_LO_MASK  0x1U
#define TIMER_START_HI_STRT_HI_BIT   0U
#define TIMER_START_HI_STRT_HI_WIDTH 1U
#define TIMER_START_HI_STRT_HI_MASK  0x1U
#define TIMER_RESET_LO_RST_LO_BIT    0U
#define TIMER_RESET_LO_RST_LO_WIDTH  1U
#define TIMER_RESET_LO_RST_LO_MASK   0x1U
#define TIMER_RESET_HI_RST_HI_BIT    0U
#define TIMER_RESET_HI_RST_HI_WIDTH  1U
#define TIMER_RESET_HI_RST_HI_MASK   0x1U

struct pulp_timer
{
    unsigned int current_time;
    unsigned int flags;
    void *base;
};

#endif /* TIMER_H */
