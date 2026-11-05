/***************************************************************************
 * Copyright (c) 2026 Eclipse ThreadX contributors
 *
 * This program and the accompanying materials are made available under the
 * terms of the MIT License which is available at
 * https://opensource.org/licenses/MIT.
 *
 * SPDX-License-Identifier: MIT
 **************************************************************************/

#ifndef TEST_COMMON_H
#define TEST_COMMON_H

#include <stdint.h>

#define TEST_PRIO_HIGH 3U
#define TEST_PRIO_MID  8U

void test_stack_init(void);
void test_stack_report(void);
uintptr_t test_sys_stack_bottom(void);
uintptr_t test_sys_stack_top(void);
uintptr_t test_sp(void);
void test_spin(unsigned int n);

#endif
