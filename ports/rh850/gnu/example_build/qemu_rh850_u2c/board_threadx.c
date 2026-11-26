/***************************************************************************
 * Copyright (c) 2026 Eclipse ThreadX contributors
 *
 * This program and the accompanying materials are made available under the
 * terms of the MIT License which is available at
 * https://opensource.org/licenses/MIT.
 *
 * SPDX-License-Identifier: MIT
 **************************************************************************/

#include <stddef.h>
#include <stdint.h>
#include "tx_api.h"
#include "board.h"
#include "rh850_intc.h"
#include "rh850_ostm.h"

UCHAR tx_port_free_memory[TX_PORT_FREE_MEMORY_SIZE] __attribute__((aligned(8)));

#ifdef TX_REGRESSION_TEST
void test_interrupt_dispatch(void) __attribute__((weak));
#endif

#ifdef TX_LOW_POWER
#define CYCLES_PER_TICK (BOARD_TIMER_CLK_HZ / TX_TIMER_TICKS_PER_SECOND)
/* Counter differences stay below 2^31 */
#define MAX_SLEEP_TICKS (0x7fffffffUL / CYCLES_PER_TICK)

static uint32_t last_tick;
static ULONG sleep_ticks;
ULONG tx_port_low_power_ticks;

/* A compare the counter has passed waits a whole wrap, so raise the request */
static void set_compare(uint32_t cmp)
{
	rh850_ostm_set_compare(BOARD_TICK_OSTM_BASE, cmp);
	if ((int32_t)(rh850_ostm_count(BOARD_TICK_OSTM_BASE) - cmp) >= 0) {
		rh850_intc_raise(BOARD_TICK_IRQ);
	}
}

static void tick_isr(void *arg)
{
	TX_INTERRUPT_SAVE_AREA
	ULONG n;

	(void)arg;
	TX_DISABLE
	n = (rh850_ostm_count(BOARD_TICK_OSTM_BASE) - last_tick) / CYCLES_PER_TICK;
	last_tick += n * CYCLES_PER_TICK;
	set_compare(last_tick + CYCLES_PER_TICK);
	TX_RESTORE
#ifdef TX_REGRESSION_TEST
	tx_trace_isr_enter_insert(0);
	if (test_interrupt_dispatch != NULL) {
		test_interrupt_dispatch();
	}
#endif
	while (n-- != 0U) {
		_tx_timer_interrupt();
	}
#ifdef TX_REGRESSION_TEST
	tx_trace_isr_exit_insert(0);
#endif
}

VOID tx_port_setup_timer_interrupt(VOID)
{
	rh850_intc_connect(BOARD_TICK_IRQ, tick_isr, NULL);
	rh850_intc_set_priority(BOARD_TICK_IRQ, RH850_INTC_PRIO_LOWEST);
	rh850_intc_enable(BOARD_TICK_IRQ);
	rh850_ostm_start_freerun(BOARD_TICK_OSTM_BASE, true);
	last_tick = rh850_ostm_count(BOARD_TICK_OSTM_BASE);
	set_compare(last_tick + CYCLES_PER_TICK);
}

VOID tx_port_low_power_timer_setup(ULONG ticks)
{
	if (ticks > MAX_SLEEP_TICKS) {
		ticks = MAX_SLEEP_TICKS;
	}
	sleep_ticks = ticks;
	if (ticks != 0U) {
		set_compare(last_tick + ticks * CYCLES_PER_TICK);
	}
}

ULONG tx_port_low_power_adjust(VOID)
{
	ULONG n = (rh850_ostm_count(BOARD_TICK_OSTM_BASE) - last_tick) / CYCLES_PER_TICK;

	if (n > sleep_ticks) {
		n = sleep_ticks;
	}
	sleep_ticks = 0;
	last_tick += n * CYCLES_PER_TICK;
	tx_port_low_power_ticks += n;
	set_compare(last_tick + CYCLES_PER_TICK);
	return n;
}
#else
static void tick_isr(void *arg)
{
	(void)arg;
#ifdef TX_REGRESSION_TEST
	tx_trace_isr_enter_insert(0);
	if (test_interrupt_dispatch != NULL) {
		test_interrupt_dispatch();
	}
#endif
	_tx_timer_interrupt();
#ifdef TX_REGRESSION_TEST
	tx_trace_isr_exit_insert(0);
#endif
}

VOID tx_port_setup_timer_interrupt(VOID)
{
	rh850_intc_connect(BOARD_TICK_IRQ, tick_isr, NULL);
	rh850_intc_set_priority(BOARD_TICK_IRQ, RH850_INTC_PRIO_LOWEST);
	rh850_intc_enable(BOARD_TICK_IRQ);
	rh850_ostm_start_interval(BOARD_TICK_OSTM_BASE,
				  BOARD_TIMER_CLK_HZ / TX_TIMER_TICKS_PER_SECOND - 1U, true);
}
#endif
