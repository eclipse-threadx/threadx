/***************************************************************************
 * Copyright (c) 2026 10xEngineers
 *
 * This program and the accompanying materials are made available under the
 * terms of the MIT License which is available at
 * https://opensource.org/licenses/MIT.
 *
 * SPDX-License-Identifier: MIT
 **************************************************************************/

/* Boot code for RISC-V QEMU virt regression tests.
   Works for both RV32 and RV64 (instructions are width-agnostic). */

.section .text.init
.align 4
.global _start
.extern main
.extern _sysstack_end
.extern _bss_start
.extern _bss_end

_start:
    csrr    t0, mhartid
    bne     t0, zero, _secondary_halt

    /* Zero all general-purpose registers. */
    li x1, 0
    li x2, 0
    li x3, 0
    li x4, 0
    li x5, 0
    li x6, 0
    li x7, 0
    li x8, 0
    li x9, 0
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

    /* Set up stack pointer from linker symbol. */
    la      sp, _sysstack_end

    /* Clear BSS. */
    la      t0, _bss_start
    la      t1, _bss_end
_bss_clean:
    bgeu    t0, t1, _bss_done
    sb      zero, 0(t0)
    addi    t0, t0, 1
    j       _bss_clean
_bss_done:
    call    main

_secondary_halt:
    wfi
    j       _secondary_halt
