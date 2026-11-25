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

#include <stdlib.h>
#include <stdio.h>
#include <board.h>

void board_init(void)
{
    /* some board preconfig */
    // board_xxx();

#if defined(BOARD_GPIO_PIN_NUM) && BOARD_GPIO_PIN_NUM > 0
    board_gpio_pin_init();
#endif

#if defined(BOARD_UART_NUM) && BOARD_UART_NUM > 0
#if !defined(RT_DEBUG_INIT) || !RT_DEBUG_INIT
    board_uart_init();
#endif
#endif

#if defined(BOARD_PWM_NUM) && BOARD_PWM_NUM > 0
    board_pwm_init();
#endif

#if defined(BOARD_ADC_NUM) && BOARD_ADC_NUM > 0
    board_adc_init();
#endif

#if defined(BOARD_BUTTON_NUM) && BOARD_BUTTON_NUM > 0
    board_button_init();
#endif

#if defined(BOARD_LED_NUM) && BOARD_LED_NUM > 0
    board_led_init();
#endif

#if defined(BOARD_WIFI_SUPPORT) && BOARD_WIFI_SUPPORT > 0
    board_wifi_init();
#endif

#if defined(BOARD_BT_SUPPORT) && BOARD_BT_SUPPORT > 0
    board_bt_init();
#endif

#if defined(BOARD_AUDIO_SUPPORT) && BOARD_AUDIO_SUPPORT > 0
    board_audio_init();
#endif
}

#ifdef CONFIG_KERNEL_THREADX
#include <tx_api.h>

extern unsigned long g_heap_start;
extern unsigned long g_heap_end;

TX_BYTE_POOL tx_byte_pool_0;
UCHAR *tx_memory_area;

void tx_mem_pool_init(void)
{
    ULONG pool_size = g_heap_end - g_heap_start;
    tx_memory_area = (UCHAR *)g_heap_start;

    tx_byte_pool_create(&tx_byte_pool_0, "byte pool sys 0", tx_memory_area, pool_size);
}
#endif
