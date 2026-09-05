/*
 * Copyright (c) 2025 10xEngineers
 * Copyright (c) 2026 Eclipse ThreadX contributors
 *
 * This program and the accompanying materials are made available under the
 * terms of the MIT License which is available at
 * https://opensource.org/licenses/MIT.
 *
 * SPDX-License-Identifier: MIT
 */

.section .iram0.text, "ax"
.align 4
.global _tx_initialize_low_level
.extern _tx_initialize_unused_memory
.extern _tx_thread_system_stack_ptr
.extern esp32c6_platform_init

_tx_initialize_low_level:
    /* Allocate stack frame to preserve return address (ra) across C call */
    addi    sp, sp, -16
    sw      ra, 12(sp)

    /* Save current System Stack Pointer for ThreadX interrupt processing */
    la      t0, _tx_thread_system_stack_ptr
    sw      sp, 0(t0)

    /* Perform C platform driver initialization (Console, PLIC, SYSTIMER) */
    call    esp32c6_platform_init

    /* Restore return address and stack frame */
    lw      ra, 12(sp)
    addi    sp, sp, 16
    ret
