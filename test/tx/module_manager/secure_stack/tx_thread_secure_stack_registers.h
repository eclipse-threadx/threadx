/* Copyright (c) 2026 Eclipse ThreadX contributors
 * SPDX-License-Identifier: MIT
 * Portions of this file were generated with AI assistance.
 */

#ifndef TEST_TX_THREAD_SECURE_STACK_REGISTERS_H
#define TEST_TX_THREAD_SECURE_STACK_REGISTERS_H

/* Register indices for the native test's simulated register bank.  */
typedef enum TEST_SECURE_REGISTER_ENUM
{
    TEST_REGISTER_IPSR,
    TEST_REGISTER_CONTROL,
    TEST_REGISTER_PSPLIM_NS,
    TEST_REGISTER_PSPLIM,
    TEST_REGISTER_PSP,
    TEST_REGISTER_COUNT
} TEST_SECURE_REGISTER;

extern ULONG test_secure_registers[TEST_REGISTER_COUNT];

VOID test_secure_register_write(TEST_SECURE_REGISTER reg, ULONG value);
VOID test_secure_stack_clear(VOID);

#define TX_SECURE_REGISTER_READ(reg, value)  ((value) = test_secure_registers[TEST_REGISTER_ ## reg])
#define TX_SECURE_REGISTER_WRITE(reg, value) test_secure_register_write(TEST_REGISTER_ ## reg, (value))
#define TX_SECURE_STACK_CLEAR_BEFORE_FREE()  test_secure_stack_clear()

#endif
