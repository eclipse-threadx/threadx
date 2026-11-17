/***************************************************************************
 * Copyright (c) 2026 safecubed GmbH
 * Copyright (c) 2026 Eclipse ThreadX contributors
 *
 * This program and the accompanying materials are made available under the
 * terms of the MIT License which is available at
 * https://opensource.org/licenses/MIT.
 *
 * SPDX-License-Identifier: MIT
 **************************************************************************/

/* Portions of this file were generated with AI assistance. */

/* Test memory of the regression test in DSPR1 of the TC375. The file is given
   with -T after sample_threadx.ld of the example and augments the default
   linker script of the AURIX GCC through INSERT. regression/CMakeLists.txt
   places test_control_memory and tests_memory in the section .bss_cpu1;
   main() clears it.  */

MEMORY
{
    DSPR1 (w!xp): org = 0x60000000, len = 240K
}

SECTIONS
{
    .bss_cpu1 (NOLOAD) : ALIGN(8)
    {
        __bss_cpu1_start = .;
        *(.bss_cpu1)
        . = ALIGN(8);
        __bss_cpu1_end = .;
    } > DSPR1
}

INSERT AFTER .csa
