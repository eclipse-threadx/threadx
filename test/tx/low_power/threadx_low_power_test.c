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
#include "tx_timer.h"
#include "tx_low_power.h"

/* Timer wheel and state globals required by utility/low_power/tx_low_power.c.  */
TX_TIMER_INTERNAL       *_tx_timer_list[TX_TIMER_ENTRIES];
TX_TIMER_INTERNAL       **_tx_timer_list_start;
TX_TIMER_INTERNAL       **_tx_timer_list_end;
TX_TIMER_INTERNAL       **_tx_timer_current_ptr;
volatile ULONG          _tx_timer_system_clock;
ULONG                   _tx_timer_time_slice;
UINT                    _tx_timer_expired;
UINT                    _tx_timer_expired_time_slice;
volatile UINT           _tx_thread_preempt_disable;

/* Hook and interrupt-lock accounting.  */
static ULONG    test_checks;
static UINT     test_failures;
static UINT     int_disable_depth;
static UINT     int_underflows;

static ULONG    setup_calls;
static ULONG    last_setup_expiration;
static ULONG    user_enter_calls;
static ULONG    user_exit_calls;
static ULONG    adjust_calls;
static ULONG    configured_adjust_ticks;
static ULONG    expired_callbacks;

/* Stand-in interrupt control routines matching Linux port prototypes.  */
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

void  test_low_power_timer_setup(unsigned long next_expiration)
{
    setup_calls++;
    last_setup_expiration = (ULONG) next_expiration;
}

void  test_low_power_user_enter(void)
{
    user_enter_calls++;
}

void  test_low_power_user_exit(void)
{
    user_exit_calls++;
}

unsigned long  test_low_power_user_timer_adjust(void)
{
    adjust_calls++;
    return((unsigned long) configured_adjust_ticks);
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

static VOID  insert_internal_timer(TX_TIMER_INTERNAL *t, TX_TIMER_INTERNAL **slot)
{
    if (*slot == TX_NULL)
    {
        *slot = t;
        t -> tx_timer_internal_active_next = t;
        t -> tx_timer_internal_active_previous = t;
    }
    else
    {
        TX_TIMER_INTERNAL *head = *slot;
        t -> tx_timer_internal_active_next = head;
        t -> tx_timer_internal_active_previous = head -> tx_timer_internal_active_previous;
        head -> tx_timer_internal_active_previous -> tx_timer_internal_active_next = t;
        head -> tx_timer_internal_active_previous = t;
    }
    t -> tx_timer_internal_list_head = slot;
}

VOID  _tx_timer_system_activate(TX_TIMER_INTERNAL *timer_ptr)
{
ULONG   ticks = timer_ptr -> tx_timer_internal_remaining_ticks;
ULONG   offset;

    if (ticks == 0UL)
    {
        return;
    }
    if (ticks <= TX_TIMER_ENTRIES)
    {
        offset = (ULONG) ((_tx_timer_current_ptr - _tx_timer_list_start) + (ticks - 1UL)) % TX_TIMER_ENTRIES;
    }
    else
    {
        offset = (ULONG) ((_tx_timer_current_ptr - _tx_timer_list_start) + (TX_TIMER_ENTRIES - 1UL)) % TX_TIMER_ENTRIES;
        if (offset == (ULONG) (_tx_timer_current_ptr - _tx_timer_list_start))
        {
            offset = (offset + TX_TIMER_ENTRIES - 1UL) % TX_TIMER_ENTRIES;
        }
    }
    insert_internal_timer(timer_ptr, &_tx_timer_list_start[offset]);
}

static VOID  reset_timer_wheel(VOID)
{
    memset((VOID *) _tx_timer_list, 0, sizeof(_tx_timer_list));
    _tx_timer_list_start = &_tx_timer_list[0];
    _tx_timer_list_end = &_tx_timer_list[TX_TIMER_ENTRIES];
    _tx_timer_current_ptr = _tx_timer_list_start;
    _tx_timer_system_clock = 0UL;
    _tx_timer_time_slice = 0UL;
    _tx_timer_expired = TX_FALSE;
    _tx_timer_expired_time_slice = TX_FALSE;
    setup_calls = 0UL;
    last_setup_expiration = 0UL;
    user_enter_calls = 0UL;
    user_exit_calls = 0UL;
    adjust_calls = 0UL;
    configured_adjust_ticks = 0UL;
    expired_callbacks = 0UL;
}

static VOID  timer_timeout_cb(ULONG id)
{
    (VOID) id;
    expired_callbacks++;
}

int main(void)
{
ULONG               next_ticks = 999UL;
TX_TIMER_INTERNAL   t_short;
TX_TIMER_INTERNAL   t_second;
TX_TIMER_INTERNAL   t_long;

    reset_timer_wheel();

    /* 1. Empty timer wheel and no time slice.  */
    test_expect("empty wheel tx_timer_get_next returns TX_FALSE",
                (ULONG) tx_timer_get_next(&next_ticks), TX_FALSE);

    /* Calling tx_low_power_exit before tx_low_power_enter is a safe no-op.  */
    tx_low_power_exit();
    test_expect("exit without enter does not call user exit", user_exit_calls, 0UL);
    test_expect("exit without enter does not adjust clock", adjust_calls, 0UL);

    /* Entering low power on empty wheel sets sentinel 0xFFFFFFFF.  */
    tx_low_power_enter();
    test_expect("enter on empty wheel calls timer setup once", setup_calls, 1UL);
    test_expect("enter on empty wheel passes 0xFFFFFFFF", last_setup_expiration, 0xFFFFFFFFUL);
    test_expect("enter calls user_enter hook", user_enter_calls, 1UL);

    configured_adjust_ticks = 4UL;
    tx_low_power_exit();
    test_expect("exit calls adjust and user_exit",
                (ULONG) ((adjust_calls == 1UL) && (user_exit_calls == 1UL)), 1UL);
    test_expect("system clock advanced by 4 ticks", _tx_timer_system_clock, 4UL);

    /* 2. Time slice decrements cleanly in tx_time_increment without wrapping.  */
    reset_timer_wheel();
    _tx_timer_time_slice = 5UL;
    test_expect("empty wheel with time slice still returns TX_FALSE from tx_timer_get_next",
                (ULONG) tx_timer_get_next(&next_ticks), TX_FALSE);

    /* Time slice > time_increment decrements cleanly without expiring.  */
    tx_time_increment(3UL);
    test_expect("time slice decremented from 5 to 2", _tx_timer_time_slice, 2UL);
    test_expect("time slice <= time_increment sets time slice to 1",
                (tx_time_increment(3UL), _tx_timer_time_slice), 1UL);

    /* 3. Short & multi-slot timers: find nearest slot across wheel + expire cleanly.  */
    reset_timer_wheel();
    memset((VOID *) &t_short, 0, sizeof(t_short));
    t_short.tx_timer_internal_remaining_ticks = 5UL;
    t_short.tx_timer_internal_re_initialize_ticks = 0UL;
    t_short.tx_timer_internal_timeout_function = timer_timeout_cb;
    _tx_timer_system_activate(&t_short);

    memset((VOID *) &t_second, 0, sizeof(t_second));
    t_second.tx_timer_internal_remaining_ticks = 12UL;
    t_second.tx_timer_internal_re_initialize_ticks = 0UL;
    t_second.tx_timer_internal_timeout_function = timer_timeout_cb;
    _tx_timer_system_activate(&t_second);

    test_expect("tx_timer_get_next sees earliest active timer",
                (ULONG) tx_timer_get_next(&next_ticks), TX_TRUE);
    test_expect("nearest active timer is <= 5 ticks",
                (ULONG) ((next_ticks > 0UL) && (next_ticks <= 5UL)), 1UL);

    /* Advance 3 ticks via enter/exit (t_short shifted, neither expired yet).  */
    tx_low_power_enter();
    test_expect("timer setup got nearest tick count",
                (ULONG) ((last_setup_expiration > 0UL) && (last_setup_expiration <= 5UL)), 1UL);
    configured_adjust_ticks = 3UL;
    tx_low_power_exit();
    test_expect("clock advanced by 3 ticks", _tx_timer_system_clock, 3UL);
    test_expect("t_short still armed on wheel", (ULONG) (t_short.tx_timer_internal_list_head != TX_NULL), 1UL);

    /* 4. Long timer (> TX_TIMER_ENTRIES) stays armed after partial increment > 0.  */
    reset_timer_wheel();
    memset((VOID *) &t_long, 0, sizeof(t_long));
    t_long.tx_timer_internal_remaining_ticks = (ULONG) (TX_TIMER_ENTRIES + 10U);
    t_long.tx_timer_internal_re_initialize_ticks = 0UL;
    t_long.tx_timer_internal_timeout_function = timer_timeout_cb;
    _tx_timer_system_activate(&t_long);

    tx_time_increment(5UL);
    test_expect("long timer still active after 5 tick advance",
                (ULONG) (t_long.tx_timer_internal_list_head != TX_NULL), 1UL);
    test_expect("system clock at 5 after long timer advance", _tx_timer_system_clock, 5UL);

    /* Interrupt balance verification.  */
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
