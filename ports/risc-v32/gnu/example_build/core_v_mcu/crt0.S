/***************************************************************************/
/* Copyright 2020 ETH Zurich
 * Copyright (c) 2017 SiFive Inc. All rights reserved.
 * Copyright (c) 2019 ETH Zürich and University of Bologna
 * Copyright (C) 2026 Eclipse ThreadX contributors
 *
 * Derived from core-v-freertos (https://github.com/openhwgroup/core-v-freertos)
 * Original work licensed under the Apache License, Version 2.0 (Apache-2.0)
 * or the BSD 2-Clause License (BSD-2-Clause) at your option.
 * See https://www.apache.org/licenses/LICENSE-2.0
 * Modifications licensed under MIT (https://opensource.org/licenses/MIT).
 *
 * AI Disclosure: Some portions generated or modified by Copilot (Sonnet 4.6).
 *
 * SPDX-License-Identifier: (Apache-2.0 OR BSD-2-Clause) AND MIT
 ***************************************************************************/

    .section .text.start
    .global _start
    .type _start, @function

_start:
    .option push
    .option norelax
1:  auipc gp, %pcrel_hi(__global_pointer$)
    addi  gp, gp, %pcrel_lo(1b)
    .option pop

    la    sp, __stack_top

    la    a0, __vector_start
    ori   a0, a0, 1
    csrw  mtvec, a0

    la    t0, __bss_start
    la    t1, __bss_end
2:
    bgeu  t0, t1, 3f
    sw    zero, 0(t0)
    addi  t0, t0, 4
    j     2b
3:
    call  main
4:
    wfi
    j     4b

    .size _start, .-_start
