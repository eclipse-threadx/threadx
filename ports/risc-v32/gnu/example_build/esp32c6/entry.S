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
.global _start
.extern _esp32c6_vector_table
.extern c6_startup

_start:
    /* Force Direct Mode (Mode 0) by clearing bottom 2 mode bits of mtvec */
    la      t0, _esp32c6_vector_table
    andi    t0, t0, -4
    csrw    mtvec, t0

    /* Clear MIE bit in mstatus during boot */
    csrc    mstatus, 8

    /* Set up global pointer and stack pointer */
    la      gp, __global_pointer$
    la      sp, _sp

    /* Clear BSS Section */
    la      t0, _bss_start
    la      t1, _bss_end
1:
    bgeu    t0, t1, 2f
    sw      zero, 0(t0)
    addi    t0, t0, 4
    j       1b
2:
    /* Call C startup entry point */
    call    c6_startup

    /* Safety infinite loop if c6_startup returns */
3:
    j       3b
