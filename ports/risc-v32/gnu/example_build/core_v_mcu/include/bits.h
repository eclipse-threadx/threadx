/***************************************************************************/
/* Copyright (c) 2011-2014, Wind River Systems, Inc.
 * Copyright 2020 ETH Zurich
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

#ifndef BITS_H
#define BITS_H

#include <stdbool.h>
#include <stdint.h>

/* Helper to pass an integer as a pointer or vice versa. */
#define POINTER_TO_UINT(x) ((uintptr_t)(x))
#define UINT_TO_POINTER(x) ((void *)(uintptr_t)(x))
#define POINTER_TO_INT(x)  ((intptr_t)(x))
#define INT_TO_POINTER(x)  ((void *)(intptr_t)(x))

#if !(defined(__CHAR_BIT__) && defined(__SIZEOF_LONG__))
#error Missing required predefined macros for BITS_PER_LONG calculation
#endif

#define BITS_PER_LONG (__CHAR_BIT__ * __SIZEOF_LONG__)
#define GENMASK(h, l) \
    (((~0UL) - (1UL << (l)) + 1UL) & (~0UL >> (BITS_PER_LONG - 1UL - (h))))

#define KB(x) ((x) << 10)
#define MB(x) (KB(x) << 10)
#define GB(x) (MB(x) << 10)

#define KHZ(x) ((x) * 1000)
#define MHZ(x) (KHZ(x) * 1000)

#ifndef BIT
#ifdef _ASMLANGUAGE
#define BIT(n) (1 << (n))
#else
#define BIT(n) (1UL << (n))
#endif
#endif

#define WRITE_BIT(var, bit, set) \
    ((var) = ((set) != 0) ? ((var) | BIT(bit)) : ((var) & ~BIT(bit)))

#define BIT_MASK(n) (BIT(n) - 1UL)

#define REG_SET(FIELD, v) (((uint32_t)(v) << FIELD##_SHIFT) & FIELD##_MASK)
#define REG_GET(FIELD, v) (((uint32_t)(v) & FIELD##_MASK) >> FIELD##_SHIFT)

#endif /* BITS_H */
