/*
 * init_mc_denali.S
 *
 *  Created on: Jan 12, 2011
 *  Author: MIPS TECHNOLOGIES, INC
 
Initialization code for Denali memory controller needed for CoreFPGA5
*/
/*
Unpublished work (c) MIPS Technologies, Inc.  All rights reserved.  Unpublished rights reserved
under the copyright laws of the United States of America and other countries.

This code is confidential and proprietary to MIPS Technologies, Inc. ("MIPS Technologies") and
may be disclosed only as permitted in writing by MIPS Technologies or an authorized third party.
Any copying, reproducing, modifying, use or disclosure of this code (in whole or in part) that is
not expressly permitted in writing by MIPS Technologies or an authorized third party is strictly
prohibited. At a minimum, this code is protected under trade secret, unfair competition, and
copyright laws. Violations thereof may result in criminal penalties and fines.

MIPS Technologies reserves the right to change this code to improve function, design or
otherwise. MIPS Technologies does not assume any liability arising out of the application or use
of this code, or of any error or omission in such code.  Any warranties, whether express, statutory,
implied or otherwise, including but not limited to the implied warranties of merchantability or
fitness for a particular purpose, are excluded.  Except as expressly provided in any written license
agreement from MIPS Technologies or an authorized third party, the furnishing of this code does
not give recipient any license to any intellectual property rights, including any patent rights, that
cover this code.

This code shall not be exported, reexported, transferred, or released, directly or indirectly, in
violation of the law of any country or international law, regulation, treaty, Executive Order,
statute, amendments or supplements thereto.  Should a conflict arise regarding the export,
reexport, transfer, or release of this code, the laws of the United States of America shall be
the governing law.

This code may only be disclosed to the United States government ("Government"), or to
Government users, with prior written consent from MIPS Technologies or an authorized third
party.  This code constitutes one or more of the following: commercial computer software,
commercial computer software documentation or other commercial items.  If the user of this
code, or any related documentation of any kind, including related technical data or manuals, is an
agency, department, or other entity of the Government, the use, duplication, reproduction, release,
modification, disclosure, or transfer of this code, or any related documentation of any kind, is
restricted in accordance with Federal Acquisition Regulation 12.212 for civilian agencies and
Defense Federal Acquisition Regulation Supplement 227.7202 for military agencies.  The use of
this code by the Government is further restricted in accordance with the terms of the license
agreement(s) and/or applicable contract terms and conditions covering this code from MIPS
Technologies or an authorized third party.
*/
#ifndef SDR_SDRAM_DIMM
#define SDR_SDRAM_DIMM      0          /* Set to 0 if you have a DDR SDRAM DIMM. */
#endif
#include <boot.h>
#include <mips/regdef.h>

	.set	noreorder           # Don't allow the assembler to reorder instructions.
	.set	noat                # Don't allow the assembler to use r1(at) for synthetic instr.
/**************************************************************************************
Hardcoded Denali Databahn DRAM controller initialization.
**************************************************************************************/
LEAF(init_mc_denali)

    li      a1, DENALI_CTL_SECTION

    lw      a0, 0x0c(a1)
    ins     a0, zero, 16, 1          # Clear Start bit
    sw      a0, 0x0c(a1)

    //# (Use YAMON to dump register settings for your target.)
#if (SDR_SDRAM_DIMM) /* defined in c command line in make file */
    li      a0, 0x00000100
    sw      a0, 0x04(a1)
    li      a0, 0x00000001
    sw      a0, 0x0c(a1)
    li      a0, 0x01000101
    sw      a0, 0x10(a1)
    li      a0, 0x02020200
    sw      a0, 0x14(a1)
    li      a0, 0x00000302
    sw      a0, 0x18(a1)
    li      a0, 0x02020000
    sw      a0, 0x1c(a1)
    li      a0, 0x00080A01
    sw      a0, 0x20(a1)
    li      a0, 0x02040000
    sw      a0, 0x24(a1)
    li      a0, 0x000F0404
    sw      a0, 0x28(a1)
    li      a0, 0x00090602
    sw      a0, 0x2c(a1)
    li      a0, 0x0A0A001E
    sw      a0, 0x30(a1)
    li      a0, 0x00000A0A
    sw      a0, 0x34(a1)
    li      a0, 0x04040000
    sw      a0, 0x38(a1)
    li      a0, 0x00000404
    sw      a0, 0x3c(a1)
    li      a0, 0x01010000
    sw      a0, 0x40(a1)
    li      a0, 0x02040101
    sw      a0, 0x44(a1)
    li      a0, 0x000C2D06
    sw      a0, 0x48(a1)
    li      a0, 0x00062706
    sw      a0, 0x50(a1)
    li      a0, 0x00000006
    sw      a0, 0x54(a1)
    li      a0, 0x00000014
    sw      a0, 0x58(a1)
    li      a0, 0x01010101
    sw      a0, 0x60(a1)
    li      a0, 0x01010000
    sw      a0, 0x64(a1)
    li      a0, 0x00010100
    sw      a0, 0x68(a1)
    li      a0, 0x0F0F0002
    sw      a0, 0x6c(a1)

    lw      a0, 0x0c(a1)
    ins     a0, r1_all_ones, 16, 1      # Set Start bit
    sw      a0, 0x0c(a1)
#else

    //# Sequence of accesses recorded from YAMON 2.21 booting w/ supplied DDR DIMM...
    //# Hardcoded initial values.

    li      a0, 0x01000000
    sw      a0, 0x00(a1)
    li      a0, 0x01000100
    sw      a0, 0x04(a1)
    li      a0, 0x00000001
    sw      a0, 0x08(a1)
    li      a0, 0x01000000
    sw      a0, 0x0c(a1)
    li      a0, 0x03000101
    sw      a0, 0x10(a1)
    li      a0, 0x02020000
    sw      a0, 0x14(a1)
    li      a0, 0x00000302
    sw      a0, 0x18(a1)
    li      a0, 0x02010000
    sw      a0, 0x1c(a1)
    li      a0, 0x00020A01
    sw      a0, 0x20(a1)
    li      a0, 0x02040000
    sw      a0, 0x24(a1)
    li      a0, 0x00000606
    sw      a0, 0x28(a1)
    li      a0, 0x00090602
    sw      a0, 0x2c(a1)
    li      a0, 0x2020001E
    sw      a0, 0x30(a1)
    li      a0, 0x00002020
    sw      a0, 0x34(a1)
    li      a0, 0x0b0b0000
    sw      a0, 0x38(a1)
    li      a0, 0x00000b0b
    sw      a0, 0x3c(a1)
    li      a0, 0x01010000
    sw      a0, 0x40(a1)

    li      a0, 0x02040101
    sw      a0, 0x44(a1)
    li      a0, 0x00030507
    sw      a0, 0x48(a1)
    li      a0, 0x00c80000
    sw      a0, 0x4c(a1)
    li      a0, 0x00c80618
    sw      a0, 0x50(a1)
    li      a0, 0x000000c8
    sw      a0, 0x54(a1)
    li      a0, 0x00000014
    sw      a0, 0x58(a1)
    li      a0, 0x00000000
    sw      a0, 0x5c(a1)
    li      a0, 0x01010101
    sw      a0, 0x60(a1)
    li      a0, 0x01010000
    sw      a0, 0x64(a1)
    li      a0, 0x00010100
    sw      a0, 0x68(a1)
    li      a0, 0x0F0F0002
    sw      a0, 0x6c(a1)
    li      a0, 0x00000000
    sw      a0, 0x70(a1)
    li      a0, 0x00000000
    sw      a0, 0x74(a1)
    li      a0, 0x00000000
    sw      a0, 0x78(a1)
    li      a0, 0x00020000
    sw      a0, 0x7c(a1)


    li      a0, 0x000f0606
    sw      a0, 0x28(a1)
    li      a0, 0x00000001
    sw      a0, 0x08(a1)
    li      a0, 0x01020000
    sw      a0, 0x14(a1)
    li      a0, 0x00000303
    sw      a0, 0x18(a1)
    li      a0, 0x01000101
    sw      a0, 0x10(a1)
    li      a0, 0x00000302
    sw      a0, 0x18(a1)
    li      a0, 0x000f0604
    sw      a0, 0x28(a1)
    li      a0, 0x000f0404
    sw      a0, 0x28(a1)
    li      a0, 0x00008207
    sw      a0, 0x48(a1)
    li      a0, 0x01000101
    sw      a0, 0x10(a1)

    lw      a0, 0x0c(a1)
    ins     a0, r1_all_ones, 16, 1      # Set Start bit
    sw      a0, 0x0c(a1)

wait_init_ddr:
    lw      a0, 0x28(a1)
    ext     a0, a0, 26, 1      # Extract init status
    beq     a0, zero, wait_init_ddr
    nop

    li      a0, 0x0F020A01
    sw      a0, 0x20(a1)
    li      a0, 0x00000404
    sw      a0, 0x28(a1)
    li      a0, 0x00010000
    sw      a0, 0x70(a1)
    li      a0, 0x01010101
    sw      a0, 0x10(a1)
    li      a0, 0x00000402
    sw      a0, 0x28(a1)
    li      a0, 0x00000202
    sw      a0, 0x28(a1)
    li      a0, 0x00000203
    sw      a0, 0x28(a1)
    li      a0, 0x00000303
    sw      a0, 0x28(a1)
    li      a0, 0x00000304
    sw      a0, 0x28(a1)
    li      a0, 0x00000404
    sw      a0, 0x28(a1)
    li      a0, 0x00000405
    sw      a0, 0x28(a1)
    li      a0, 0x00000505
    sw      a0, 0x28(a1)

#endif

    jr      ra
    nop
END(init_mc_denali)

