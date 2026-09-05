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

/* LP_WDT block, from soc/lp_wdt_reg.h with DR_REG_LP_WDT_BASE = 0x600B1C00.
   Both write-protect registers take the same key on the ESP32-C6; the C3 and
   S3 use a different one for the super watchdog, so a snippet copied from
   those targets will not unlock it here.  */
#define LP_WDT_BASE                 0x600B1C00UL
#define LP_WDT_CONFIG0_REG          (LP_WDT_BASE + 0x00)
#define LP_WDT_WPROTECT_REG         (LP_WDT_BASE + 0x18)
#define LP_WDT_SWD_CONFIG_REG       (LP_WDT_BASE + 0x1C)
#define LP_WDT_SWD_WPROTECT_REG     (LP_WDT_BASE + 0x20)
#define LP_WDT_WKEY_VALUE           0x50D83AA1UL
#define LP_WDT_SWD_DISABLE          (1UL << 30)
#define LP_WDT_SWD_AUTO_FEED_EN     (1UL << 18)

static void esp32c6_disable_wdt(void)
{

    /* The main LP (RTC) watchdog. Writes to CONFIG0 are dropped by the
       hardware unless write protection is lifted first, so the key has to go
       to WPROTECT at offset 0x18 and nowhere else.  */
    REG_WRITE(LP_WDT_WPROTECT_REG, LP_WDT_WKEY_VALUE);
    REG_WRITE(LP_WDT_CONFIG0_REG, 0);
    REG_WRITE(LP_WDT_WPROTECT_REG, 0);

    /* The super watchdog is a separate timer in the same block, enabled out of
       reset, and it resets the chip on its own period whatever LP_WDT is
       doing. Disable it and leave auto-feed on so a stray enable cannot bite.  */
    REG_WRITE(LP_WDT_SWD_WPROTECT_REG, LP_WDT_WKEY_VALUE);
    REG_WRITE(LP_WDT_SWD_CONFIG_REG,
              REG_READ(LP_WDT_SWD_CONFIG_REG) | LP_WDT_SWD_DISABLE
                                              | LP_WDT_SWD_AUTO_FEED_EN);
    REG_WRITE(LP_WDT_SWD_WPROTECT_REG, 0);
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
