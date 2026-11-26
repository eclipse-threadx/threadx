/***************************************************************************
 * Copyright (c) 2026 Eclipse ThreadX contributors
 *
 * This program and the accompanying materials are made available under the
 * terms of the MIT License which is available at
 * https://opensource.org/licenses/MIT.
 *
 * SPDX-License-Identifier: MIT
 **************************************************************************/

#ifndef RH850_TEST_EXIT_H
#define RH850_TEST_EXIT_H

#include <stdint.h>

/* Test device of QEMU rh850-u2c (test-exit=on), not a U2C peripheral */
void test_exit_pass(uintptr_t base) __attribute__((noreturn));
void test_exit_fail(uintptr_t base, uint16_t code) __attribute__((noreturn));

#endif
