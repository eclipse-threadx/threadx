/***************************************************************************
 * Copyright (c) 2026 Eclipse ThreadX contributors
 *
 * This program and the accompanying materials are made available under the
 * terms of the MIT License which is available at
 * https://opensource.org/licenses/MIT.
 *
 * SPDX-License-Identifier: MIT
 **************************************************************************/

#include "rh850_cpu.h"
#include "rh850_io.h"
#include "test_exit.h"

/* Status in bits 15:0, exit code in bits 31:16 */
#define TEST_EXIT_FAIL 0x3333
#define TEST_EXIT_PASS 0x5555

static void __attribute__((noreturn)) test_exit_write(uintptr_t base, uint32_t v)
{
	rh850_write32(base, v);
	for (;;) {
		rh850_di();
		rh850_halt();
	}
}

void test_exit_pass(uintptr_t base)
{
	test_exit_write(base, TEST_EXIT_PASS);
}

void test_exit_fail(uintptr_t base, uint16_t code)
{
	test_exit_write(base, ((uint32_t)code << 16) | TEST_EXIT_FAIL);
}
