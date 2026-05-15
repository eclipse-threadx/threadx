/***************************************************************************
 * Copyright (c) 2026 10xEngineers
 *
 * This program and the accompanying materials are made available under the
 * terms of the MIT License which is available at
 * https://opensource.org/licenses/MIT.
 *
 * SPDX-License-Identifier: MIT
 **************************************************************************/

    .global _start
    .extern main
    .extern _sysstack_start
    .extern _bss_start
    .extern _bss_end

    .section .text.entry
    .align 4
_start:
    /* Zero all general-purpose registers (x1–x31). */
    li x1,  0
    li x2,  0
    li x3,  0
    li x4,  0
    li x5,  0
    li x6,  0
    li x7,  0
    li x8,  0
    li x9,  0
    li x10, 0
    li x11, 0
    li x12, 0
    li x13, 0
    li x14, 0
    li x15, 0
    li x16, 0
    li x17, 0
    li x18, 0
    li x19, 0
    li x20, 0
    li x21, 0
    li x22, 0
    li x23, 0
    li x24, 0
    li x25, 0
    li x26, 0
    li x27, 0
    li x28, 0
    li x29, 0
    li x30, 0
    li x31, 0

    /* Set up the initial supervisor stack (16 KiB). */
    la   t0, _sysstack_start
    li   t1, 0x4000
    add  sp, t0, t1

    /* Zero the .bss section. */
    la   t0, _bss_start
    la   t1, _bss_end
_bss_clean_start:
    bgeu t0, t1, _bss_clean_end
    sb   zero, 0(t0)
    addi t0, t0, 1
    j    _bss_clean_start
_bss_clean_end:

    call main

    /* Halt if main() ever returns. */
_park:
    wfi
    j _park
