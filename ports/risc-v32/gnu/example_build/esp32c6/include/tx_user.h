/*
 * Eclipse ThreadX User Configuration Header for ESP32-C6 (RISC-V32)
 *
 * SPDX-License-Identifier: MIT
 */

#ifndef TX_USER_H
#define TX_USER_H

/* ------------------------------------------------------------------------------
 * Kernel Timer & Tick Configuration
 * ------------------------------------------------------------------------------
 * System timer tick frequency in Hz.
 * 100 Ticks/Second = 10ms per tick interval.
 */
#ifndef TX_TIMER_TICKS_PER_SECOND
#define TX_TIMER_TICKS_PER_SECOND           ((ULONG)100)
#endif

/* ------------------------------------------------------------------------------
 * Thread Priority & Scheduling Settings
 * ------------------------------------------------------------------------------
 * Number of thread priority levels (0 to TX_MAX_PRIORITIES - 1).
 * 32 priority levels (0-31) provides an optimal balance between scheduler
 * flexibility and low RAM usage.
 */
#ifndef TX_MAX_PRIORITIES
#define TX_MAX_PRIORITIES                   32
#endif

/* Minimum stack size in bytes for ThreadX threads */
#ifndef TX_MINIMUM_STACK
#define TX_MINIMUM_STACK                    512
#endif

/* ------------------------------------------------------------------------------
 * Safety & Debugging Options
 * ------------------------------------------------------------------------------
 * Enable runtime stack checking to detect stack overflow conditions early.
 */
#ifndef TX_ENABLE_STACK_CHECKING
#define TX_ENABLE_STACK_CHECKING
#endif

/* Enable event trace capability if needed for telemetry (disabled by default) */
/* #define TX_ENABLE_EVENT_TRACE */

#endif /* TX_USER_H */
