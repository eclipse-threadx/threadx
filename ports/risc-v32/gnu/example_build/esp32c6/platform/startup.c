/***************************************************************************
 * Copyright (c) 2026 Eclipse ThreadX contributors
 *
 * This program and the accompanying materials are made available under the
 * terms of the MIT License which is available at
 * https://opensource.org/licenses/MIT.
 *
 * Inspired by and derived in part from architectural patterns in:
 * - Espressif Systems ESP-IDF (Apache-2.0)
 * - Zephyr RTOS Project (Apache-2.0)
 **************************************************************************/

#include "esp32c6_example.h"
#include "esp_app_desc.h"

/*
 * Espressif 2nd-Stage Bootloader Application Description Header
 * Placed at the very top of segment 0 (.rodata_desc) via link.ld.
 */
const esp_app_desc_t esp_app_desc __attribute__((section(".rodata_desc"))) = {
    .magic_word = ESP_APP_DESC_MAGIC_WORD,
    .secure_version = 0,
    .version = "1.0.0",
    .project_name = "esp32c6_threadx",
    .time = __TIME__,
    .date = __DATE__,
    .idf_ver = "v6.0.1",
    .app_elf_sha256 = {0},
    .reserv2 = {0}
};

static void esp32c6_disable_wdt(void)
{
    /* Unlock LP_WDT write protection using key 0x50D83AA1 */
    REG_WRITE(0x600B1C1C, 0x50D83AA1);
    /* Disable LP_WDT to prevent standalone hardware reset loops */
    REG_WRITE(0x600B1C00, 0);
}

void esp32c6_platform_init(void)
{
    /* 0. Disable hardware LP Watchdog Timer */
    esp32c6_disable_wdt();

    /* 1. Initialize Console UART */
    esp32c6_console_init();

    /* 2. Initialize Interrupt Matrix & PLIC */
    esp32c6_interrupt_init();

    /* 3. Initialize SYSTIMER */
    esp32c6_systimer_init();
}

void c6_startup(void)
{
    /* Hand off directly to ThreadX kernel initialization */
    tx_kernel_enter();
}
