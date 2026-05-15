/***************************************************************************/
/* Copyright 2020 ETH Zurich
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

#ifndef IO_H
#define IO_H

#include <stdint.h>

#ifndef CORE_V_MCU_MMIO_MOCK
static inline void writeb(uint8_t val, uintptr_t addr)
{
    __asm__ volatile("sb %0, 0(%1)" : : "r"(val), "r"((volatile uint8_t *)addr));
}

static inline void writeh(uint16_t val, uintptr_t addr)
{
    __asm__ volatile("sh %0, 0(%1)" : : "r"(val), "r"((volatile uint16_t *)addr));
}

static inline void writew(uint32_t val, uintptr_t addr)
{
    __asm__ volatile("sw %0, 0(%1)" : : "r"(val), "r"((volatile uint32_t *)addr));
}

static inline uint8_t readb(const uintptr_t addr)
{
    uint8_t val;

    __asm__ volatile("lb %0, 0(%1)" : "=r"(val) : "r"((const volatile uint8_t *)addr));
    return val;
}

static inline uint16_t readh(const uintptr_t addr)
{
    uint16_t val;

    __asm__ volatile("lh %0, 0(%1)" : "=r"(val) : "r"((const volatile uint16_t *)addr));
    return val;
}

static inline uint32_t readw(const uintptr_t addr)
{
    uint32_t val;

    __asm__ volatile("lw %0, 0(%1)" : "=r"(val) : "r"((const volatile uint32_t *)addr));
    return val;
}
#endif /* CORE_V_MCU_MMIO_MOCK */

#endif /* IO_H */
