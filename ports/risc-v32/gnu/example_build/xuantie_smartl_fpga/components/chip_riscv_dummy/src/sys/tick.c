 /*
 * Copyright (C) 2017-2024 Alibaba Group Holding Limited
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <soc.h>
#include <csi_core.h>
#include <csi_config.h>
#include <sys_clk.h>
#include <drv/common.h>
#include <drv/irq.h>
#include <drv/tick.h>
#include <drv/porting.h>
#include <drv/timer.h>

#define __WEAK         __attribute__((weak))

// from 1970-01-01 00:00:00 UTC
static volatile uint64_t timestamp_us_offset;

#if defined(CONFIG_SMP) && CONFIG_SMP
static volatile uint32_t csi_tick[CONFIG_NR_CPUS] = {0U};
#else
static volatile uint32_t csi_tick = 0U;
#endif
static volatile uint32_t last_time_ms = 0U;
static volatile uint64_t last_time_us = 0U;

#ifdef CONFIG_TIMER_FOR_TICK
static csi_timer_t tick_timer;
#ifndef CONFIG_TICK_TIMER_IDX
#define CONFIG_TICK_TIMER_IDX   0U
#endif
#else /* !CONFIG_TIMER_FOR_TICK */
static csi_dev_t tick_dev;
static volatile uint64_t timer_init_value = 0U;
#endif

void csi_tick_increase(void)
{
#if defined(CONFIG_SMP) && CONFIG_SMP
    csi_tick[csi_get_cpu_id()]++;
#else
    csi_tick++;
#endif
}

uint32_t csi_tick_get(void)
{
#if defined(CONFIG_SMP) && CONFIG_SMP
    return csi_tick[csi_get_cpu_id()];
#else
    return csi_tick;
#endif
}

#ifdef CONFIG_TIMER_FOR_TICK
void tick_event_cb(csi_timer_t *timer_handle, void *arg)
{
    csi_tick_increase();
#if CONFIG_AOS_OSAL
    extern void aos_sys_tick_handler(void);
    aos_sys_tick_handler();
#else
#ifdef CONFIG_KERNEL_FREERTOS
    extern void xPortSysTickHandler(void);
    xPortSysTickHandler();
#elif defined(CONFIG_KERNEL_RTTHREAD)
    extern void rt_tick_increase(void);
    rt_tick_increase();
#elif defined(CONFIG_KERNEL_THREADX)
    extern void _tx_timer_interrupt(void);
    _tx_timer_interrupt();
#else
#endif
#endif /* end CONFIG_AOS_OSAL */
}
#else /* !CONFIG_TIMER_FOR_TICK */
void tick_irq_handler(void *arg)
{
    csi_tick_increase();
    csi_coret_config((soc_get_coretim_freq() / CONFIG_SYSTICK_HZ), tick_dev.irq_num);
#if CONFIG_AOS_OSAL
    extern void aos_sys_tick_handler(void);
    aos_sys_tick_handler();
#else
#ifdef CONFIG_KERNEL_FREERTOS
    extern void xPortSysTickHandler(void);
    xPortSysTickHandler();
#elif defined(CONFIG_KERNEL_RTTHREAD)
    extern void rt_tick_increase(void);
    rt_tick_increase();
#elif defined(CONFIG_KERNEL_THREADX)
    extern void _tx_timer_interrupt(void);
    _tx_timer_interrupt();
#else
#endif
#endif /* end CONFIG_AOS_OSAL */

}
#endif /* CONFIG_TIMER_FOR_TICK */

csi_error_t csi_tick_init(void)
{
#if defined(CONFIG_SMP) && CONFIG_SMP
    csi_tick[csi_get_cpu_id()] = 0;
#else
    csi_tick = 0U;
#endif

#ifdef CONFIG_TIMER_FOR_TICK
    csi_error_t ret = csi_timer_init(&tick_timer, CONFIG_TICK_TIMER_IDX);
    if (ret == CSI_OK) {
        ret = csi_timer_attach_callback(&tick_timer, tick_event_cb, NULL);
        if (ret == CSI_OK) {
            ret = csi_timer_start(&tick_timer, (1000000U / CONFIG_SYSTICK_HZ));
        }
    }
    return ret;
#else /* !CONFIG_TIMER_FOR_TICK */
#if CONFIG_RISCV_SMODE
    tick_dev.irq_num = Supervisor_Timer_IRQn;
#else
    tick_dev.irq_num = CORET_IRQn;
#endif
#if CONFIG_CPU_XUANTIE_E9XX || CONFIG_INTC_CLIC || CONFIG_INTC_CLIC_PLIC || CONFIG_INTC_CLIC_APLIC
    csi_vic_set_prio(tick_dev.irq_num, 2);
    csi_irq_attach(tick_dev.irq_num, &tick_irq_handler, &tick_dev);
#endif /* CONFIG_CPU_XUANTIE_E9XX || CONFIG_INTC_CLIC || CONFIG_INTC_CLIC_PLIC || CONFIG_INTC_CLIC_APLIC */
    timer_init_value = csi_coret_get_value2();
    csi_coret_reset_value2();
    csi_coret_config((soc_get_coretim_freq() / CONFIG_SYSTICK_HZ), tick_dev.irq_num);
    csi_coret_irq_enable();
#endif /* CONFIG_TIMER_FOR_TICK */
    return CSI_OK;
}

void csi_tick_uninit(void)
{
#ifdef CONFIG_TIMER_FOR_TICK
    csi_timer_stop(&tick_timer);
    csi_timer_uninit(&tick_timer);
#else
    csi_coret_irq_disable();
#if CONFIG_CPU_XUANTIE_E9XX || CONFIG_INTC_CLIC_PLIC
    csi_irq_detach(tick_dev.irq_num);
#endif
#endif /* CONFIG_TIMER_FOR_TICK */
}

#ifdef CONFIG_TIMER_FOR_TICK
uint32_t csi_tick_get_ms(void)
{
    uint32_t time = last_time_ms, freq;
    freq = csi_timer_get_load_value(&tick_timer) * CONFIG_SYSTICK_HZ;

    while (freq) {
        time = (csi_tick * (1000U / CONFIG_SYSTICK_HZ)) + ((csi_timer_get_load_value(&tick_timer) - csi_timer_get_remaining_value(&tick_timer)) / (freq / 1000U));

        if (time >= last_time_ms) {
            break;
        }
    }

    last_time_ms = time;
    return time;
}

uint64_t csi_tick_get_us(void)
{
    uint64_t time, freq;
    uint32_t temp;
    freq = soc_get_timer_freq(CONFIG_TICK_TIMER_IDX);

    while (1) {
        /* the time of coretim pass */
        temp = csi_timer_get_load_value(&tick_timer) - csi_timer_get_remaining_value(&tick_timer);
        time = ((uint64_t)temp * 1000U) / (freq / 1000U);
        /* the time of csi_tick */
        time += ((uint64_t)csi_tick * (1000000U / CONFIG_SYSTICK_HZ));

        if (time >= last_time_us) {
            break;
        }
    }

    last_time_us = time;
    return time;
}

static void _mdelay(void)
{
    uint32_t load = csi_timer_get_load_value(&tick_timer);
    uint32_t start_r = csi_timer_get_remaining_value(&tick_timer);
    uint32_t cur_r;
    uint32_t cnt   = (soc_get_timer_freq(CONFIG_TICK_TIMER_IDX) / 1000U);

    while (1) {
        cur_r = csi_timer_get_remaining_value(&tick_timer);

        if (start_r > cur_r) {
            if ((start_r - cur_r) >= cnt) {
                break;
            }
        } else {
            if (((load - cur_r) + start_r) >= cnt) {
                break;
            }
        }
    }
}

static void _10udelay(void)
{
    uint32_t load = csi_timer_get_load_value(&tick_timer);
    uint32_t start_r = csi_timer_get_remaining_value(&tick_timer);
    uint32_t cur_r;
    uint32_t cnt   = (soc_get_timer_freq(CONFIG_TICK_TIMER_IDX) / 100000U);

    while (1) {
        cur_r = csi_timer_get_remaining_value(&tick_timer);

        if (start_r > cur_r) {
            if ((start_r - cur_r) >= cnt) {
                break;
            }
        } else {
            if (((load - cur_r) + start_r) >= cnt) {
                break;
            }
        }
    }
}

#else /* !CONFIG_TIMER_FOR_TICK */

uint32_t csi_tick_get_ms(void)
{
    uint32_t time;

    time = (uint32_t)((csi_coret_get_value2() - timer_init_value) * 1000U / (uint64_t)soc_get_coretim_freq());
    last_time_ms = time;
    return time;
}

uint64_t csi_tick_get_us(void)
{
    uint64_t time;

    time = (csi_coret_get_value2() - timer_init_value) * 1000U * 1000U / (uint64_t)soc_get_coretim_freq();
    last_time_us = time;
    return time;
}

static void _mdelay(void)
{
    uint64_t start = csi_coret_get_value2();
    uint64_t cur;
    uint32_t cnt = (soc_get_coretim_freq() / 1000U);

    while (1) {
        cur = csi_coret_get_value2();

        if (start > cur) {
            if ((start - cur) >= cnt) {
                break;
            }
        } else {
            if (cur - start >= cnt) {
                break;
            }
        }
    }
}

static void _10udelay(void)
{
    uint64_t cur;
    uint64_t start = csi_coret_get_value2();
    uint32_t cnt = (soc_get_coretim_freq() / 1000U / 100U);

    while (1) {
        cur = csi_coret_get_value2();

        if (start > cur) {
            if ((start - cur) >= cnt) {
                break;
            }
        } else {
            if (cur - start >= cnt) {
                break;
            }
        }
    }
}
#endif

void csi_set_calendar_us(uint64_t timestamp)
{
    timestamp_us_offset = timestamp;
}

uint64_t csi_get_calendar_us(void)
{
    return csi_tick_get_us() + timestamp_us_offset;
}

__WEAK void mdelay(uint32_t ms)
{
    while (ms) {
        ms--;
        _mdelay();
    }
}

/**
 * Ps: At least delay over 10us
*/
void udelay(uint32_t us)
{
    us /= 10U;

    while (us) {
        us--;
        _10udelay();
    }
}

