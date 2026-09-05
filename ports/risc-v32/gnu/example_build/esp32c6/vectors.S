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
.global _esp32c6_vector_table
.extern _tx_thread_context_save
.extern _tx_thread_context_restore
.extern esp32c6_interrupt_dispatch

_esp32c6_vector_table:
    /* Pre-allocate stack frame expected by ThreadX RISC-V context save */
    addi    sp, sp, -128
    sw      ra, 112(sp)

    /* Call ThreadX interrupt context save */
    call    _tx_thread_context_save

    /* Dispatch interrupt to C handler */
    call    esp32c6_interrupt_dispatch

    /* Jump to ThreadX interrupt context restore */
    j       _tx_thread_context_restore
