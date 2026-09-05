/***************************************************************************
 * Copyright (c) 2026 Eclipse ThreadX contributors
 *
 * This program and the accompanying materials are made available under the
 * terms of the MIT License which is available at
 * https://opensource.org/licenses/MIT.
 *
 * SPDX-License-Identifier: MIT
 **************************************************************************/

#include "esp32c6_example.h"

#define DEMO_STACK_SIZE         2048

static TX_THREAD         thread_0;
static TX_THREAD         thread_1;
static TX_MUTEX          demo_mutex;

/* 16-byte aligned static stack buffers guaranteed for RISC-V ABI */
static uint8_t           thread_0_stack[DEMO_STACK_SIZE] __attribute__((aligned(16)));
static uint8_t           thread_1_stack[DEMO_STACK_SIZE] __attribute__((aligned(16)));

static ULONG             thread_0_counter = 0;
static ULONG             thread_1_counter = 0;

/* ------------------------------------------------------------------------------
 * Thread Functions
 * ------------------------------------------------------------------------------
 */

static void thread_0_entry(ULONG thread_input)
{
    (void)thread_input;

    esp32c6_console_write("[ThreadX Demo] Thread 0 Started.\r\n");

    while (1) {
        /* Acquire Mutex */
        if (tx_mutex_get(&demo_mutex, TX_WAIT_FOREVER) == TX_SUCCESS) {
            thread_0_counter++;
            esp32c6_console_write("[Thread 0] Acquired Mutex, Counter: ");
            
            /* Simple decimal print */
            char buf[16];
            uint32_t val = (uint32_t)thread_0_counter;
            int idx = 0;
            if (val == 0) {
                buf[idx++] = '0';
            } else {
                char temp[16];
                int t_idx = 0;
                while (val > 0) {
                    temp[t_idx++] = '0' + (val % 10);
                    val /= 10;
                }
                while (t_idx > 0) {
                    buf[idx++] = temp[--t_idx];
                }
            }
            buf[idx++] = '\r';
            buf[idx++] = '\n';
            buf[idx] = '\0';
            esp32c6_console_write(buf);

            /* Release Mutex */
            tx_mutex_put(&demo_mutex);
        }

        /* Sleep for 50 ticks (500 ms) */
        tx_thread_sleep(50);
    }
}

static void thread_1_entry(ULONG thread_input)
{
    (void)thread_input;

    esp32c6_console_write("[ThreadX Demo] Thread 1 Started.\r\n");

    while (1) {
        /* Acquire Mutex */
        if (tx_mutex_get(&demo_mutex, TX_WAIT_FOREVER) == TX_SUCCESS) {
            thread_1_counter++;
            esp32c6_console_write("[Thread 1] Acquired Mutex, Counter: ");
            
            char buf[16];
            uint32_t val = (uint32_t)thread_1_counter;
            int idx = 0;
            if (val == 0) {
                buf[idx++] = '0';
            } else {
                char temp[16];
                int t_idx = 0;
                while (val > 0) {
                    temp[t_idx++] = '0' + (val % 10);
                    val /= 10;
                }
                while (t_idx > 0) {
                    buf[idx++] = temp[--t_idx];
                }
            }
            buf[idx++] = '\r';
            buf[idx++] = '\n';
            buf[idx] = '\0';
            esp32c6_console_write(buf);

            /* Release Mutex */
            tx_mutex_put(&demo_mutex);
        }

        /* Sleep for 100 ticks (1000 ms) */
        tx_thread_sleep(100);
    }
}

/* ------------------------------------------------------------------------------
 * ThreadX Application Definition Entry Point
 * ------------------------------------------------------------------------------
 */
void tx_application_define(void *first_unused_memory)
{
    (void)first_unused_memory;

    esp32c6_console_write("\r\n======================================================\r\n");
    esp32c6_console_write(" Eclipse ThreadX RTOS on ESP32-C6 (RISC-V32 Bare-Metal)\r\n");
    esp32c6_console_write("======================================================\r\n\r\n");

    /* Create ThreadX Mutex */
    tx_mutex_create(&demo_mutex, "Demo Mutex", TX_NO_INHERIT);

    /* Create Thread 0 (Priority 16) with 16-byte aligned static stack */
    tx_thread_create(&thread_0, "Thread 0", thread_0_entry, 0,
                     thread_0_stack, DEMO_STACK_SIZE,
                     16, 16, TX_NO_TIME_SLICE, TX_AUTO_START);

    /* Create Thread 1 (Priority 17) with 16-byte aligned static stack */
    tx_thread_create(&thread_1, "Thread 1", thread_1_entry, 0,
                     thread_1_stack, DEMO_STACK_SIZE,
                     17, 17, TX_NO_TIME_SLICE, TX_AUTO_START);
}
