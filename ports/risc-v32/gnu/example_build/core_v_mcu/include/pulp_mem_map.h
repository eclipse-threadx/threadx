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

#ifndef PULP_MEM_MAP_H
#define PULP_MEM_MAP_H

#ifndef PULP_SOC_PERIPHERALS_ADDR
#define PULP_SOC_PERIPHERALS_ADDR 0x1A100000UL
#endif

#ifndef PULP_FC_TIMER_SIZE
#define PULP_FC_TIMER_SIZE 0x00000800UL
#endif

#ifndef PULP_FLL_OFFSET
#define PULP_FLL_OFFSET 0x00000000UL
#endif
#ifndef PULP_GPIO_OFFSET
#define PULP_GPIO_OFFSET 0x00001000UL
#endif
#ifndef PULP_UDMA_OFFSET
#define PULP_UDMA_OFFSET 0x00002000UL
#endif
#ifndef PULP_APB_SOC_CTRL_OFFSET
#define PULP_APB_SOC_CTRL_OFFSET 0x00004000UL
#endif
#ifndef PULP_ADV_TIMER_OFFSET
#define PULP_ADV_TIMER_OFFSET 0x00005000UL
#endif
#ifndef PULP_SOC_EU_OFFSET
#define PULP_SOC_EU_OFFSET 0x00006000UL
#endif
#ifndef PULP_FC_IRQ_OFFSET
#define PULP_FC_IRQ_OFFSET 0x00009800UL
#endif
#ifndef PULP_FC_TIMER_OFFSET
#define PULP_FC_TIMER_OFFSET 0x0000B000UL
#endif
#ifndef PULP_FC_HWPE_OFFSET
#define PULP_FC_HWPE_OFFSET 0x0000C000UL
#endif
#ifndef PULP_STDOUT_OFFSET
#define PULP_STDOUT_OFFSET 0x0000F000UL
#endif
#ifndef PULP_DEBUG_OFFSET
#define PULP_DEBUG_OFFSET 0x00010000UL
#endif

#ifndef PULP_FLL_ADDR
#define PULP_FLL_ADDR (PULP_SOC_PERIPHERALS_ADDR + PULP_FLL_OFFSET)
#endif
#ifndef PULP_GPIO_ADDR
#define PULP_GPIO_ADDR (PULP_SOC_PERIPHERALS_ADDR + PULP_GPIO_OFFSET)
#endif
#ifndef PULP_UDMA_ADDR
#define PULP_UDMA_ADDR (PULP_SOC_PERIPHERALS_ADDR + PULP_UDMA_OFFSET)
#endif
#ifndef PULP_APB_SOC_CTRL_ADDR
#define PULP_APB_SOC_CTRL_ADDR (PULP_SOC_PERIPHERALS_ADDR + PULP_APB_SOC_CTRL_OFFSET)
#endif
#ifndef PULP_ADV_TIMER_ADDR
#define PULP_ADV_TIMER_ADDR (PULP_SOC_PERIPHERALS_ADDR + PULP_ADV_TIMER_OFFSET)
#endif
#ifndef PULP_SOC_EU_ADDR
#define PULP_SOC_EU_ADDR (PULP_SOC_PERIPHERALS_ADDR + PULP_SOC_EU_OFFSET)
#endif
#ifndef PULP_FC_IRQ_ADDR
#define PULP_FC_IRQ_ADDR (PULP_SOC_PERIPHERALS_ADDR + PULP_FC_IRQ_OFFSET)
#endif
#ifndef PULP_FC_TIMER_ADDR
#define PULP_FC_TIMER_ADDR (PULP_SOC_PERIPHERALS_ADDR + PULP_FC_TIMER_OFFSET)
#endif
#ifndef PULP_FC_HWPE_ADDR
#define PULP_FC_HWPE_ADDR (PULP_SOC_PERIPHERALS_ADDR + PULP_FC_HWPE_OFFSET)
#endif
#ifndef PULP_STDOUT_ADDR
#define PULP_STDOUT_ADDR (PULP_SOC_PERIPHERALS_ADDR + PULP_STDOUT_OFFSET)
#endif

#define PULP_FLL_AREA_SIZE 0x00000010UL

/* UDMA */
#define UDMA_CH_ADDR_CTRL  (PULP_UDMA_ADDR)
#define UDMA_CH_ADDR_UART  (PULP_UDMA_ADDR + 0x80U)
#define UDMA_CH_ADDR_I2CM0 (PULP_UDMA_ADDR + 5U * UDMA_CH_SIZE)
#define UDMA_CH_ADDR_I2CM1 (PULP_UDMA_ADDR + 6U * UDMA_CH_SIZE)
#define UDMA_CTRL_I2CM0_CLKEN BIT(4)
#define UDMA_CTRL_I2CM1_CLKEN BIT(5)
#define UDMA_CH_SIZE       (0x80U)
#define N_UART             (2U)

#endif /* PULP_MEM_MAP_H */
