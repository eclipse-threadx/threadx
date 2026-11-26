/***************************************************************************
 * Copyright (c) 2026 Eclipse ThreadX contributors
 *
 * This program and the accompanying materials are made available under the
 * terms of the MIT License which is available at
 * https://opensource.org/licenses/MIT.
 *
 * SPDX-License-Identifier: MIT
 **************************************************************************/

/* The two 384 KB blocks of testcontrol.c go to cluster RAM; local RAM is 128 KB */
#ifndef RH850_MEMORY_H
#define RH850_MEMORY_H

#include "tx_api.h"

extern UCHAR test_control_memory[] __attribute__((section(".bss.rh850_large.test_control_memory")));
extern UCHAR tests_memory[] __attribute__((section(".bss.rh850_large.tests_memory")));

#endif
