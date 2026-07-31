/***************************************************************************
 * Copyright (c) 2026 Eclipse ThreadX contributors
 *
 * This program and the accompanying materials are made available under the
 * terms of the MIT License which is available at
 * https://opensource.org/licenses/MIT.
 *
 * SPDX-License-Identifier: MIT
 **************************************************************************/

#ifndef ESP32C6_EXAMPLE_H
#define ESP32C6_EXAMPLE_H

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

/* ThreadX API Header Inclusion */
#include "tx_api.h"

/* ESP-IDF ROM & Register Header Inclusions */
#include "soc/soc.h"
#include "soc/interrupts.h"
#include "soc/reg_base.h"
#include "soc/plic_reg.h"
#include "hal/interrupt_plic_ll.h"
#include "esp_rom_sys.h"

/* ------------------------------------------------------------------------------
 * RISC-V Register Bitmasks & Constants
 * ------------------------------------------------------------------------------
 */
#define RISCV_MCAUSE_INTERRUPT_FLAG         (1UL << 31)   /* Highest bit indicates hardware interrupt */
#define RISCV_MCAUSE_EXCEPTION_CODE_MASK    (0x7FFFFFFFUL)/* Low bits indicate interrupt/exception vector */

#define RISCV_MIE_MTIE                      (1UL << 7)    /* Machine Timer Interrupt Enable */
#define RISCV_MIE_MEIE                      (1UL << 11)   /* Machine External Interrupt Enable */

/* ------------------------------------------------------------------------------
 * Hardware & Clock Constants
 * ------------------------------------------------------------------------------
 */
#define ESP32C6_CPU_CLK_FREQ_HZ         (160000000UL)  /* CPU Clock: 160 MHz */
#define ESP32C6_SYSTIMER_CLK_FREQ_HZ    (16000000UL)   /* SYSTIMER Counter Clock: 16 MHz */
#define ESP32C6_CONSOLE_BAUDRATE        (115200)       /* Default Console Baudrate */

/* ------------------------------------------------------------------------------
 * System Interrupt Assignment Mapping (RISC-V Machine Mode PLIC)
 * ------------------------------------------------------------------------------
 */
/* Hardware peripheral interrupt sources from ESP-IDF interrupts.h */
#define ESP32C6_SYSTIMER_TICK_INTR_SRC  ETS_SYSTIMER_TARGET0_EDGE_INTR_SOURCE

/* CPU Interrupt Vector Line Numbers (Allocated for CPU Machine Interrupts) */
#define ESP32C6_SYSTIMER_CPU_INTR_NUM   1  /* Dedicated CPU Line for ThreadX 100Hz Tick */
#define ESP32C6_UART_CPU_INTR_NUM       2  /* Dedicated CPU Line for Console UART */

/* ------------------------------------------------------------------------------
 * Platform Hardware Driver API Prototypes
 * ------------------------------------------------------------------------------
 */

/**
 * @brief Early hardware platform initialization (Console, Interrupt Matrix, SYSTIMER).
 * Called during startup before launching the ThreadX kernel.
 */
void esp32c6_platform_init(void);

/**
 * @brief Initializes the ESP32-C6 UART console for serial diagnostic logging.
 */
void esp32c6_console_init(void);

/**
 * @brief Outputs a single character to the console UART.
 * @param c Character to output.
 */
void esp32c6_console_putc(char c);

/**
 * @brief Outputs a null-terminated string to the console UART.
 * @param str Pointer to string.
 */
void esp32c6_console_write(const char *str);

/**
 * @brief Initializes the ESP32-C6 64-bit SYSTIMER Alarm 0 for periodic 100 Hz interrupts.
 */
void esp32c6_systimer_init(void);

/**
 * @brief Clears the SYSTIMER Alarm 0 interrupt flag.
 */
void esp32c6_systimer_clear_interrupt(void);

/**
 * @brief Initializes the ESP32-C6 Interrupt Matrix and PLIC for RISC-V M-mode.
 */
void esp32c6_interrupt_init(void);

/**
 * @brief Attaches a handler to a specific CPU interrupt vector line.
 * @param cpu_intr_num CPU interrupt line (1-31).
 * @param intr_source Hardware peripheral interrupt source (ETS_*_INTR_SOURCE).
 * @param handler Function pointer to interrupt handler.
 * @param arg Pointer to user parameter passed to handler.
 */
typedef void (*esp32c6_isr_handler_t)(void *arg);
void esp32c6_interrupt_attach(uint32_t cpu_intr_num,
                              uint32_t intr_source,
                              esp32c6_isr_handler_t handler,
                              void *arg);

/**
 * @brief Main interrupt dispatcher called from vectors.S.
 * Queries RISC-V mcause / PLIC and invokes the attached handler.
 */
void esp32c6_interrupt_dispatch(void);

#endif /* ESP32C6_EXAMPLE_H */
