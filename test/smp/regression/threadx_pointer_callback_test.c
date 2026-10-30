/***************************************************************************
 * Copyright (c) 2024 Microsoft Corporation
 * Copyright (c) 2026-present Eclipse ThreadX contributors
 *
 * This program and the accompanying materials are made available under the
 * terms of the MIT License which is available at
 * https://opensource.org/licenses/MIT.
 *
 * SPDX-License-Identifier: MIT
 **************************************************************************/

#include   <stdio.h>
#include   <stdlib.h>
#include   "tx_api.h"
#include   "tx_thread.h"
#include   "tx_timer.h"

/* Define test control globals for standalone execution.  */
#ifndef CTEST
void  test_control_return(UINT status);
#else
extern void  test_control_return(UINT status);
#endif

static unsigned long   error = 0;

static TX_THREAD       thread_0;
static TX_THREAD       thread_ptr_test;
static TX_THREAD       thread_ulong_test;

static TX_TIMER        timer_ptr_test;
static TX_TIMER        timer_ulong_test;

static void           *received_thread_ptr_ctx = TX_NULL;
static void           *received_timer_ptr_ctx = TX_NULL;
static ULONG           received_thread_ulong_ctx = 0;
static ULONG           received_timer_ulong_ctx = 0;

static volatile UINT   thread_ptr_executed = 0;
static volatile UINT   thread_ulong_executed = 0;
static volatile UINT   timer_ptr_executed = 0;
static volatile UINT   timer_ulong_executed = 0;

#if (defined(__x86_64__) || defined(_WIN64) || defined(__aarch64__) || (defined(__riscv_xlen) && (__riscv_xlen == 64)) || defined(__LP64__) || defined(_LP64))
#define TEST_PTR_VAL_1    ((VOID *)(ALIGN_TYPE) 0x123456789ABCDEF0ULL)
#define TEST_PTR_VAL_2    ((VOID *)(ALIGN_TYPE) 0xDEADBEEFCAFEBABEULL)
#else
#define TEST_PTR_VAL_1    ((VOID *)(ALIGN_TYPE) 0x12345678UL)
#define TEST_PTR_VAL_2    ((VOID *)(ALIGN_TYPE) 0xDEADBEEFUL)
#endif
#define TEST_ULONG_VAL_1  ((ULONG) 0x55AA1234UL)
#define TEST_ULONG_VAL_2  ((ULONG) 0x99887766UL)

static void thread_ptr_entry(VOID *entry_input)
{
    received_thread_ptr_ctx = entry_input;
    thread_ptr_executed = 1;
    while (1)
    {
        tx_thread_sleep(100);
    }
}

static void thread_ulong_entry(ULONG entry_input)
{
    received_thread_ulong_ctx = entry_input;
    thread_ulong_executed = 1;
    while (1)
    {
        tx_thread_sleep(100);
    }
}

static void timer_ptr_expiration(VOID *expiration_input)
{
    received_timer_ptr_ctx = expiration_input;
    timer_ptr_executed = 1;
}

static void timer_ulong_expiration(ULONG expiration_input)
{
    received_timer_ulong_ctx = expiration_input;
    timer_ulong_executed = 1;
}

static void thread_0_entry(ULONG thread_input)
{
    UINT    status;
    VOID   *expected_thread_ptr = TEST_PTR_VAL_1;
    VOID   *expected_timer_ptr = TEST_PTR_VAL_2;

    TX_PARAMETER_NOT_USED(thread_input);

    printf("Running Pointer Callback Regression Test............................ ");

    /* Resume pointer thread and standard thread.  */
    tx_thread_resume(&thread_ptr_test);
    tx_thread_resume(&thread_ulong_test);

    /* Sleep to allow threads and timers to execute.  */
    tx_thread_sleep(5);

    /* Verify pointer thread context reception without 32-bit truncation.  */
    if (thread_ptr_executed != 1)
    {
        error++;
    }
    if (received_thread_ptr_ctx != expected_thread_ptr)
    {
        error++;
    }

    /* Verify standard ULONG thread context reception.  */
    if (thread_ulong_executed != 1)
    {
        error++;
    }
    if (received_thread_ulong_ctx != TEST_ULONG_VAL_1)
    {
        error++;
    }

    /* Verify pointer timer context reception without 32-bit truncation.  */
    if (timer_ptr_executed != 1)
    {
        error++;
    }
    if (received_timer_ptr_ctx != expected_timer_ptr)
    {
        error++;
    }

    /* Verify standard ULONG timer context reception.  */
    if (timer_ulong_executed != 1)
    {
        error++;
    }
    if (received_timer_ulong_ctx != TEST_ULONG_VAL_2)
    {
        error++;
    }

#ifndef TX_DISABLE_ERROR_CHECKING
    TX_THREAD   dummy_thread;
    TX_TIMER    dummy_timer;
    CHAR        dummy_stack[1024];

    dummy_thread.tx_thread_id = 0;
    dummy_timer.tx_timer_id = 0;

    /* Test error checking paths for pointer thread create.  */
    status = tx_thread_create_ptr(TX_NULL, "bad", thread_ptr_entry, expected_thread_ptr,
                                  dummy_stack, sizeof(dummy_stack), 16, 16, TX_NO_TIME_SLICE, TX_DONT_START);
    if (status != TX_THREAD_ERROR)
    {
        error++;
    }

    status = tx_thread_create_ptr(&dummy_thread, "bad", TX_NULL, expected_thread_ptr,
                                  dummy_stack, sizeof(dummy_stack), 16, 16, TX_NO_TIME_SLICE, TX_DONT_START);
    if (status != TX_PTR_ERROR)
    {
        error++;
    }

    status = tx_thread_create_ptr(&dummy_thread, "bad", thread_ptr_entry, expected_thread_ptr,
                                  TX_NULL, sizeof(dummy_stack), 16, 16, TX_NO_TIME_SLICE, TX_DONT_START);
    if (status != TX_PTR_ERROR)
    {
        error++;
    }

    status = tx_thread_create_ptr(&dummy_thread, "bad", thread_ptr_entry, expected_thread_ptr,
                                  dummy_stack, 0, 16, 16, TX_NO_TIME_SLICE, TX_DONT_START);
    if (status != TX_SIZE_ERROR)
    {
        error++;
    }

    /* Test error checking paths for pointer timer create.  */
    status = tx_timer_create_ptr(TX_NULL, "bad", timer_ptr_expiration, expected_timer_ptr,
                                 1, 0, TX_NO_ACTIVATE);
    if (status != TX_TIMER_ERROR)
    {
        error++;
    }

    status = tx_timer_create_ptr(&dummy_timer, "bad", timer_ptr_expiration, expected_timer_ptr,
                                 0, 0, TX_NO_ACTIVATE);
    if (status != TX_TICK_ERROR)
    {
        error++;
    }

    status = tx_timer_create_ptr(&dummy_timer, "bad", timer_ptr_expiration, expected_timer_ptr,
                                 1, 0, 0xFF);
    if (status != TX_ACTIVATE_ERROR)
    {
        error++;
    }
#endif

    /* Clean up created objects.  */
    tx_timer_deactivate(&timer_ptr_test);
    tx_timer_delete(&timer_ptr_test);
    tx_timer_deactivate(&timer_ulong_test);
    tx_timer_delete(&timer_ulong_test);

    tx_thread_terminate(&thread_ptr_test);
    tx_thread_delete(&thread_ptr_test);
    tx_thread_terminate(&thread_ulong_test);
    tx_thread_delete(&thread_ulong_test);

    if (error)
    {
        printf("ERROR!\n");
        test_control_return(1);
    }
    else
    {
        printf("SUCCESS!\n");
        test_control_return(0);
    }
}

#ifdef CTEST
void test_application_define(void *first_unused_memory)
#else
void threadx_pointer_callback_application_define(void *first_unused_memory)
#endif
{
    UINT    status;
    CHAR   *pointer = (CHAR *) first_unused_memory;

    /* Create primary test thread.  */
    status = tx_thread_create(&thread_0, "thread 0", thread_0_entry, 0,
                              pointer, 2048,
                              10, 10, TX_NO_TIME_SLICE, TX_AUTO_START);
    pointer += 2048;

    /* Create pointer callback thread.  */
    status += tx_thread_create_ptr(&thread_ptr_test, "ptr thread", thread_ptr_entry, TEST_PTR_VAL_1,
                                   pointer, 2048,
                                   12, 12, TX_NO_TIME_SLICE, TX_DONT_START);
    pointer += 2048;

    /* Create standard ULONG callback thread.  */
    status += tx_thread_create(&thread_ulong_test, "ulong thread", thread_ulong_entry, TEST_ULONG_VAL_1,
                               pointer, 2048,
                               12, 12, TX_NO_TIME_SLICE, TX_DONT_START);
    pointer += 2048;

    /* Create pointer timer.  */
    status += tx_timer_create_ptr(&timer_ptr_test, "ptr timer", timer_ptr_expiration, TEST_PTR_VAL_2,
                                  2, 0, TX_AUTO_ACTIVATE);

    /* Create standard ULONG timer.  */
    status += tx_timer_create(&timer_ulong_test, "ulong timer", timer_ulong_expiration, TEST_ULONG_VAL_2,
                              2, 0, TX_AUTO_ACTIVATE);

    if (status != TX_SUCCESS)
    {
        printf("Running Pointer Callback Regression Test............................ INITIALIZATION ERROR!\n");
        test_control_return(1);
    }
}

#ifndef CTEST
void test_control_return(UINT status)
{
    exit((int) status);
}

int main(void)
{
    tx_kernel_enter();
    return 0;
}

void tx_application_define(void *first_unused_memory)
{
    threadx_pointer_callback_application_define(first_unused_memory);
}
#endif
