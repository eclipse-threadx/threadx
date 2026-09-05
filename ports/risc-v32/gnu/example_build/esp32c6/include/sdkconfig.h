/*
 * Target Configuration Header for ESP32-C6 ThreadX BSP
 *
 * SPDX-License-Identifier: MIT
 */

#ifndef SDKCONFIG_H
#define SDKCONFIG_H

/* Target SoC Architecture & Family Definitions */
#define CONFIG_IDF_TARGET_ESP32C6       1
#define CONFIG_IDF_TARGET               "esp32c6"
#define CONFIG_IDF_TARGET_ARCH_RISCV    1
#define CONFIG_IDF_FIRMWARE_CHIP_ID     0x000D

/* Clock Configuration */
#define CONFIG_ESP_DEFAULT_CPU_FREQ_MHZ 160

#endif /* SDKCONFIG_H */
