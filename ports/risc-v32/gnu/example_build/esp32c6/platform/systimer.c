/*
 * SPDX-FileCopyrightText: 2021-2024 Espressif Systems (Shanghai) CO LTD
 * Modifications Copyright (c) 2026 Eclipse ThreadX contributors
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "esp32c6_example.h"
#include "hal/systimer_hal.h"
#include "hal/systimer_ll.h"

extern void _tx_timer_interrupt(void);

#define SYSTIMER_COUNTER_ID     0
#define SYSTIMER_ALARM_ID       0
#define SYSTIMER_TICKS_PER_10MS (ESP32C6_SYSTIMER_CLK_FREQ_HZ / TX_TIMER_TICKS_PER_SECOND)

static systimer_hal_context_t s_systimer_hal;
static uint64_t s_next_alarm_target = 0;

static void esp32c6_systimer_isr(void *arg)
{
    (void)arg;

    /* Clear hardware alarm interrupt status bit */
    systimer_ll_clear_alarm_int(&SYSTIMER, SYSTIMER_ALARM_ID);

    /* Advance next periodic 10ms target & set hardware comparator */
    s_next_alarm_target += SYSTIMER_TICKS_PER_10MS;
    systimer_ll_enable_alarm(&SYSTIMER, SYSTIMER_ALARM_ID, false);
    systimer_ll_set_alarm_target(&SYSTIMER, SYSTIMER_ALARM_ID, s_next_alarm_target);
    systimer_ll_apply_alarm_value(&SYSTIMER, SYSTIMER_ALARM_ID);
    systimer_ll_enable_alarm(&SYSTIMER, SYSTIMER_ALARM_ID, true);

    /* Invoke ThreadX kernel timer tick handler */
    _tx_timer_interrupt();
}

void esp32c6_systimer_init(void)
{
    /* Initialize Pinned ESP-IDF SYSTIMER HAL */
    systimer_hal_init(&s_systimer_hal);

    /* Enable Counter 0 & connect Alarm 0 to Counter 0 */
    systimer_ll_enable_counter(&SYSTIMER, SYSTIMER_COUNTER_ID, true);
    systimer_ll_connect_alarm_counter(&SYSTIMER, SYSTIMER_ALARM_ID, SYSTIMER_COUNTER_ID);

    /* Configure Alarm 0 initial target for 10ms period */
    uint64_t now = systimer_hal_get_counter_value(&s_systimer_hal, SYSTIMER_COUNTER_ID);
    s_next_alarm_target = now + SYSTIMER_TICKS_PER_10MS;

    systimer_ll_enable_alarm_period(&SYSTIMER, SYSTIMER_ALARM_ID);
    systimer_ll_set_alarm_target(&SYSTIMER, SYSTIMER_ALARM_ID, s_next_alarm_target);
    systimer_ll_apply_alarm_value(&SYSTIMER, SYSTIMER_ALARM_ID);

    /* Attach ISR to CPU Interrupt Line 1 */
    esp32c6_interrupt_attach(ESP32C6_SYSTIMER_CPU_INTR_NUM,
                            ESP32C6_SYSTIMER_TICK_INTR_SRC,
                            esp32c6_systimer_isr,
                            NULL);

    /* Enable Alarm 0 hardware & interrupt */
    systimer_ll_enable_alarm_int(&SYSTIMER, SYSTIMER_ALARM_ID, true);
    systimer_ll_enable_alarm(&SYSTIMER, SYSTIMER_ALARM_ID, true);
}

void esp32c6_systimer_clear_interrupt(void)
{
    systimer_ll_clear_alarm_int(&SYSTIMER, SYSTIMER_ALARM_ID);
}
