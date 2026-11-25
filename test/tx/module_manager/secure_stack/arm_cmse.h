/* Copyright (c) 2026 Eclipse ThreadX contributors
 * SPDX-License-Identifier: MIT
 * Portions of this file were generated with AI assistance.
 */

#ifndef TEST_ARM_CMSE_H
#define TEST_ARM_CMSE_H

#if !defined(__linux__) || defined(__arm__)
#error The simulated CMSE API is for the native Linux test only.
#endif

#include <stddef.h>

#define CMSE_MPU_READWRITE  1
#define CMSE_NONSECURE      18

void *cmse_check_address_range(void *address, size_t size, int flags);

#endif
