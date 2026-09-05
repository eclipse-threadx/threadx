/*
 * SPDX-FileCopyrightText: 2021-2024 Espressif Systems (Shanghai) CO LTD
 * Modifications Copyright (c) 2026 Eclipse ThreadX contributors
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "esp32c6_example.h"
#include "esp_rom_serial_output.h"

void esp32c6_console_init(void)
{
    /* Console UART is initialized by the Espressif 2nd-stage bootloader */
}

void esp32c6_console_putc(char c)
{
    if (c == '\n') {
        esp_rom_output_tx_one_char('\r');
    }
    esp_rom_output_tx_one_char(c);
}

void esp32c6_console_write(const char *str)
{
    if (!str) {
        return;
    }
    while (*str) {
        esp32c6_console_putc(*str++);
    }
}
