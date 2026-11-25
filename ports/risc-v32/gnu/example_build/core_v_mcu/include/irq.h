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

#ifndef IRQ_H
#define IRQ_H

#include "pulp_mem_map.h"
#include "io.h"
#include "bits.h"

#define IRQ_REG_MASK_OFFSET       0x000U
#define IRQ_REG_MASK_SET_OFFSET   0x004U
#define IRQ_REG_MASK_CLEAR_OFFSET 0x008U
#define IRQ_REG_INT_OFFSET        0x00CU
#define IRQ_REG_INT_SET_OFFSET    0x010U
#define IRQ_REG_INT_CLEAR_OFFSET  0x014U
#define IRQ_REG_ACK_OFFSET        0x018U
#define IRQ_REG_ACK_SET_OFFSET    0x01CU
#define IRQ_REG_ACK_CLEAR_OFFSET  0x020U
#define IRQ_REG_FIFO_OFFSET       0x024U

#define IRQ_FC_EVT_SW0          BIT(0)
#define IRQ_FC_EVT_SW1          BIT(1)
#define IRQ_FC_EVT_SW2          BIT(2)
#define IRQ_FC_EVT_SW3          BIT(3)
#define IRQ_FC_EVT_SW4          BIT(4)
#define IRQ_FC_EVT_SW5          BIT(5)
#define IRQ_FC_EVT_SW6          BIT(6)
#define IRQ_FC_EVT_SW7          BIT(7)
#define IRQ_FC_EVT_DMA_PE_EVT   BIT(8)
#define IRQ_FC_EVT_DMA_PE_IRQ   BIT(9)
#define IRQ_FC_EVT_TIMER0_LO    BIT(10)
#define IRQ_FC_EVT_TIMER0_HI    BIT(11)
#define IRQ_FC_EVT_PF           BIT(12)
#define IRQ_FC_EVT_CLK_REF      BIT(14)
#define IRQ_FC_EVT_GPIO         BIT(15)
#define IRQ_FC_EVT_ADV_TIMER0   BIT(17)
#define IRQ_FC_EVT_ADV_TIMER1   BIT(18)
#define IRQ_FC_EVT_ADV_TIMER2   BIT(19)
#define IRQ_FC_EVT_ADV_TIMER3   BIT(20)
#define IRQ_FC_EVT_SOC_EVT      BIT(26)
#define IRQ_FC_EVT_QUIRQE_ERROR BIT(29)
#define IRQ_FC_EVT_PERIPH0      BIT(30)
#define IRQ_FC_EVT_PERIPH1      BIT(31)

void irq_mask(uint32_t mask);
void irq_enable(uint32_t mask);
void irq_disable(uint32_t mask);
uint32_t irq_clint_disable(void);
uint32_t irq_clint_enable(void);
void pulp_irq_init(void);

#endif /* IRQ_H */
