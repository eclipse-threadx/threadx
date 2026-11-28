/***************************************************************************
 * Copyright (c) 2026-present Eclipse ThreadX contributors
 *
 * This program and the accompanying materials are made available under the
 * terms of the MIT License which is available at
 * https://opensource.org/licenses/MIT.
 *
 * SPDX-License-Identifier: MIT
 **************************************************************************/

#include <stdio.h>
#include <string.h>

#include "tx_api.h"
#include "tx_thread.h"

/* Globals expected by utility/execution_profile_kit/tx_execution_profile.c.  */
volatile ULONG  _tx_thread_system_state;
TX_THREAD       *_tx_thread_current_ptr;
TX_THREAD       *_tx_thread_execute_ptr;
TX_THREAD       *_tx_thread_created_ptr;
ULONG           _tx_thread_created_count;
volatile UINT   _tx_thread_preempt_disable;

/* Test clock and interrupt-lock accounting.  */
static ULONG    epk_clock;
static ULONG    test_checks;
static UINT     test_failures;
static UINT     int_disable_depth;
static UINT     int_underflows;

unsigned long  test_epk_clock_read(void)
{
    return((unsigned long) epk_clock);
}

UINT  _tx_thread_interrupt_disable(VOID)
{
UINT    prev = int_disable_depth;

    int_disable_depth++;
    return(prev);
}

VOID  _tx_thread_interrupt_restore(UINT previous_posture)
{
    if (int_disable_depth == 0U)
    {
        int_underflows++;
    }
    else
    {
        int_disable_depth = previous_posture;
    }
}

static VOID  test_expect(const char *description, ULONG actual, ULONG expected)
{
    test_checks++;
    if (actual != expected)
    {
        printf("FAIL: %s (expected %lu, got %lu)\n", description,
               (unsigned long) expected, (unsigned long) actual);
        test_failures++;
    }
}

static VOID  test_expect64(const char *description, EXECUTION_TIME actual, EXECUTION_TIME expected)
{
    test_checks++;
    if (actual != expected)
    {
        printf("FAIL: %s (expected %llu, got %llu)\n", description,
               (unsigned long long) expected, (unsigned long long) actual);
        test_failures++;
    }
}

int main(void)
{
TX_THREAD       t1;
TX_THREAD       t2;
EXECUTION_TIME  val = 0ULL;

    memset((VOID *) &t1, 0, sizeof(t1));
    memset((VOID *) &t2, 0, sizeof(t2));
    t1.tx_thread_id = TX_THREAD_ID;
    t2.tx_thread_id = TX_THREAD_ID;
    t1.tx_thread_created_next = &t2;
    t1.tx_thread_created_previous = &t2;
    t2.tx_thread_created_next = &t1;
    t2.tx_thread_created_previous = &t1;
    _tx_thread_created_ptr = &t1;
    _tx_thread_created_count = 2UL;

    epk_clock = 100UL;
    _tx_thread_system_state = 0UL;
    _tx_thread_preempt_disable = 0U;
    _tx_execution_initialize();

    /* 1. Basic thread enter/exit accumulates thread and total thread execution time.  */
    _tx_thread_current_ptr = &t1;
    _tx_thread_execute_ptr = &t1;
    _tx_execution_thread_enter();
    epk_clock = 250UL;
    _tx_thread_execute_ptr = &t2;
    _tx_execution_thread_exit();

    _tx_execution_thread_time_get(&t1, &val);
    test_expect64("t1 accumulated 150 ticks", val, 150ULL);
    _tx_execution_thread_total_time_get(&val);
    test_expect64("total thread time is 150 ticks", val, 150ULL);

    /* 2. Counter wrap-around on thread exit (last_start = 0xFFFFFFF0, current = 0x00000010 -> delta 0x20).  */
    epk_clock = 0xFFFFFFF0UL;
    _tx_thread_current_ptr = &t2;
    _tx_thread_execute_ptr = &t2;
    _tx_execution_thread_enter();
    epk_clock = 0x00000010UL;
    /* Exit to idle (_tx_thread_execute_ptr == TX_NULL) starts idle timer.  */
    _tx_thread_execute_ptr = TX_NULL;
    _tx_execution_thread_exit();

    _tx_execution_thread_time_get(&t2, &val);
    test_expect64("t2 wrap-around delta is 32 ticks", val, 0x20ULL);
    _tx_execution_thread_total_time_get(&val);
    test_expect64("total thread time is 182 ticks", val, 182ULL);

    /* 3. Idle measurement finishes when next thread enters.  */
    epk_clock = 0x00000060UL;
    _tx_thread_current_ptr = &t1;
    _tx_thread_execute_ptr = &t1;
    _tx_execution_thread_enter();
    _tx_execution_idle_time_get(&val);
    test_expect64("idle accumulated 80 ticks before t1 resumed", val, 0x50ULL);

    /* 4. ISR enter/exit suspends t1, measures ISR duration, and resumes t1.  */
    epk_clock = 0x00000090UL;
    _tx_thread_system_state = 1UL;
    _tx_execution_isr_enter();
    _tx_execution_thread_time_get(&t1, &val);
    test_expect64("t1 gained 48 ticks before ISR entry", val, 150ULL + 0x30ULL);

    /* Nested ISR (system_state == 2) must not reset ISR start timestamp.  */
    epk_clock = 0x000000A0UL;
    _tx_thread_system_state = 2UL;
    _tx_execution_isr_enter();
    _tx_execution_isr_exit();

    epk_clock = 0x000000B0UL;
    _tx_thread_system_state = 1UL;
    _tx_execution_isr_exit();
    _tx_thread_system_state = 0UL;
    _tx_execution_isr_time_get(&val);
    test_expect64("outer ISR measured 32 ticks across nested call", val, 0x20ULL);

    /* Interrupted t1 received refreshed last_start at 0xB0 and continues cleanly.  */
    epk_clock = 0x000000D0UL;
    _tx_execution_thread_exit();
    _tx_execution_thread_time_get(&t1, &val);
    test_expect64("t1 resumed timing after ISR exit (+32 ticks)", val, 150ULL + 0x30ULL + 0x20ULL);

    /* 5. Reset APIs clear individual thread, all threads total, ISR, and idle counters.  */
    test_expect("_tx_execution_thread_time_reset succeeds",
                _tx_execution_thread_time_reset(&t1), TX_SUCCESS);
    _tx_execution_thread_time_get(&t1, &val);
    test_expect64("t1 reset to 0", val, 0ULL);

    test_expect("_tx_execution_thread_total_time_reset succeeds",
                _tx_execution_thread_total_time_reset(), TX_SUCCESS);
    _tx_execution_thread_total_time_get(&val);
    test_expect64("total thread time reset to 0", val, 0ULL);
    _tx_execution_thread_time_get(&t2, &val);
    test_expect64("t2 also reset by total_time_reset", val, 0ULL);

    test_expect("_tx_execution_isr_time_reset succeeds",
                _tx_execution_isr_time_reset(), TX_SUCCESS);
    _tx_execution_isr_time_get(&val);
    test_expect64("ISR time reset to 0", val, 0ULL);

    test_expect("_tx_execution_idle_time_reset succeeds",
                _tx_execution_idle_time_reset(), TX_SUCCESS);
    _tx_execution_idle_time_get(&val);
    test_expect64("idle time reset to 0", val, 0ULL);

    test_expect("interrupt disable depth balanced to 0", int_disable_depth, 0UL);
    test_expect("interrupt restore underflows == 0", int_underflows, 0UL);

    if (test_failures == 0U)
    {
        printf("SUCCESS! %lu expectations\n", (unsigned long) test_checks);
        return(0);
    }

    printf("ERROR: %lu expectation(s) failed of %lu\n",
           (unsigned long) test_failures, (unsigned long) test_checks);
    return(1);
}
