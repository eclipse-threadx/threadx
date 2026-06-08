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

#ifndef PROPERTIES_H
#define PROPERTIES_H

#define PULP

/* Peripheral bus clock (clk_per): PER_CLK_PERIOD_NS=200 → 5 MHz */
#define ARCHI_FPGA_FREQUENCY 5000000U
/* FC/SOC clock (clk_soc): FC_CLK_PERIOD_NS=100 → 10 MHz */
#define ARCHI_SOC_FREQUENCY  10000000U
#define ARCHI_NUM_TIMER      1U
#define ARCHI_NUM_FLL        2U

#define ARCHI_REF_CLOCK_LOG2 15U
#define ARCHI_REF_CLOCK      (1U << ARCHI_REF_CLOCK_LOG2)

#define ARCHI_NB_FLL 3U

#define __RT_FLL_CL     2U
#define __RT_FLL_PERIPH 1U
#define __RT_FLL_FC     0U

#define __RT_FREQ_DOMAIN_FC     0U
#define __RT_FREQ_DOMAIN_CL     2U
#define __RT_FREQ_DOMAIN_PERIPH 1U
#define RT_FREQ_NB_DOMAIN       3U

#define DEFAULT_SYSTEM_CLOCK 50000000U

#endif /* PROPERTIES_H */
