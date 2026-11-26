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

/* Memory description of CPU0 of the TC375 for the default linker script of
   the AURIX GCC and its start-up code crt0-tc38xx.o.

   The file is given with -T and augments the default linker script through
   INSERT. It replaces the memory description that the compiler driver adds
   for -mcpu=tc38xx. The start-up code occupies the start of program flash
   bank 0, 0x80000000, which the boot mode header of the board selects
   through its non-cached address 0xA0000000.  */

__USTACK_SIZE = 4K;
__ISTACK_SIZE = 4K;
__HEAP_SIZE = 4K;
__CSA_SIZE = 16K;

MEMORY
{
    PFLASH0 (rx!p): org = 0x80000000, len = 3M
    DSPR0 (w!xp): org = 0x70000000, len = 240K
}

REGION_ALIAS("CODE_MEM", PFLASH0)
REGION_ALIAS("DATA_MEM", DSPR0)
REGION_ALIAS("SDATA_MEM", DSPR0)
REGION_ALIAS("BSS_MEM", DSPR0)
REGION_ALIAS("ZDATA_MEM", DSPR0)
REGION_ALIAS("CSA_MEM", DSPR0)

INSERT BEFORE .startup


/* Interrupt vector table of CPU0. TX_PORT_TRICORE_INTERRUPT in tx_port.h
   places the entry of priority n in the section .intvec_tc0_<n>, which is
   located at BIV + 32 * n. The table holds the priorities 1 to 15 and is
   aligned to its size of 512 bytes.  */

SECTIONS
{
    .inttab_tc0 : ALIGN(512)
    {
        __INTTAB_CPU0 = .;
        . = 0x020; KEEP (*(.intvec_tc0_1))
        . = 0x040; KEEP (*(.intvec_tc0_2))
        . = 0x060; KEEP (*(.intvec_tc0_3))
        . = 0x080; KEEP (*(.intvec_tc0_4))
        . = 0x0A0; KEEP (*(.intvec_tc0_5))
        . = 0x0C0; KEEP (*(.intvec_tc0_6))
        . = 0x0E0; KEEP (*(.intvec_tc0_7))
        . = 0x100; KEEP (*(.intvec_tc0_8))
        . = 0x120; KEEP (*(.intvec_tc0_9))
        . = 0x140; KEEP (*(.intvec_tc0_10))
        . = 0x160; KEEP (*(.intvec_tc0_11))
        . = 0x180; KEEP (*(.intvec_tc0_12))
        . = 0x1A0; KEEP (*(.intvec_tc0_13))
        . = 0x1C0; KEEP (*(.intvec_tc0_14))
        . = 0x1E0; KEEP (*(.intvec_tc0_15))
    } > CODE_MEM
}

INSERT AFTER .inttab


/* The memory of DSPR0 after the context save areas is free.
   _tx_initialize_low_level passes its start to tx_application_define.  */

SECTIONS
{
    .tx_free_memory (NOLOAD) : ALIGN(8)
    {
        __TX_FREE_MEM = .;
    } > DATA_MEM
}

INSERT AFTER .csa
