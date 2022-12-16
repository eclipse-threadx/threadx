/*
 * init_gic.S
 *
 *  Created on: Jan 12, 2011
 *  Author: MIPS TECHNOLOGIES, INC
 *  Initialization of the Global Interrupt Conrtoller
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

#include <boot.h>
#include <regdef.h>
#include <cps.h>

	.set	noreorder           // Don't allow the assembler to reorder instructions.
	.set	noat                // Don't allow the assembler to use r1(at) for synthetic instr.

/**************************************************************************************
**************************************************************************************/
LEAF(init_gic)

    beqz    r11_is_cps, done_gic					// Skip if non-CPS.
    nop

    la      a1, GCR_GIC_STATUS + GCR_CONFIG_ADDR	// Read GCR_GIC_STATUS
    lw      a0, 0(a1)
    ext     a0, a0, GIC_EX, GIC_EX_S				// Isolate  GCR_GIC_STATUS[GIC_EX].
    beqz    a0, done_gic							// If no gic then skip.
    nop

    bnez    r23_cpu_num, init_vpe_gic				// Only core0 vpe0 inits shared portion.
    nop

    li      a1, GCR_CONFIG_ADDR + GCR_GIC_BASE  	// Locate and enable GIC
    li      a0, GIC_P_BASE_ADDR | 1 				// Physical address + enable bit
    sw      a0, 0(a1)

    // Verify gic is 5 "slices" of 8 interrupts giving 40 interrupts.
    li      a1, GIC_BASE_ADDR						// load GIC KSEG0 Address
    lw      a0, GIC_SH_CONFIG(a1)					// GIC_SH_CONFIG
    ext     a0, a0, NUMINTERRUPTS, NUMINTERRUPTS_S		// NUMINTERRUPTS (actually slices - 1)
    li      a3, 4
    beq     a0, a3, configure_slices
    nop
    sdbbp   // Failed assertion that gic implements 40 external interrupts.

configure_slices:
    // Hardcoded to set up the last 16 of 40 external interrupts (24..39) for IPI.
    // First interrupts 24... 31 (top 8 bits of 31_0 registers)
    li      a0, 0xff000000
    sw      a0, GIC_SH_RMASK31_0(a1)	// (disable      0..31)
    sw      a0, GIC_SH_POL31_0(a1)		// (high/rise   24..31)
    sw      a0, GIC_SH_TRIG31_0(a1)		// (edge        24..31)
    sw      a0, GIC_SH_SMASK31_00(a1)	// (enable      24..31)

    // Then interrupts 32...40  (lower 8 bits of 63_32 registers)
    li      a0, 0xff
    sw      a0, GIC_SH_RMASK63_32(a1)	// (disable     32..63)
    sw      a0, GIC_SH_POL63_32(a1)		// (high/rise   32..39)
    sw      a0, GIC_SH_TRIG63_32(a1)	// (edge        32..39)
    sw      a0, GIC_SH_SMASK63_32(a1)	// (enable      32..39)

    // Initialize configuration of shared interrupts

    // Direct GIC_int24..39 to vpe 0..7
    // MIPS Linux convention that last 16 interrupts implemented be set aside for IPI signaling.
    // (The actual interrupts are tied low and software sends interrupts via GIC_SH_WEDGE writes.)
    li      a0, 1                        // set bit 0 for CORE0 or for MT vpe0
    sw      a0, GIC_SH_MAP0_VPE31_0 + (GIC_SH_MAP_SPACER * 24) (a1) // Source 24 to VPE 0
    sw      a0, GIC_SH_MAP0_VPE31_0 + (GIC_SH_MAP_SPACER * 32) (a1) // Source 32 to VPE 0
    sll     a0, a0, 1          // set bit 1 for CORE1 or for MT vpe1
    sw      a0, GIC_SH_MAP0_VPE31_0 + (GIC_SH_MAP_SPACER * 25) (a1) // Source 25 to VPE 1
    sw      a0, GIC_SH_MAP0_VPE31_0 + (GIC_SH_MAP_SPACER * 33) (a1) // Source 33 to VPE 1
    sll     a0, a0, 1          // set bit 2 for CORE2 or for MT vpe2
    sw      a0, GIC_SH_MAP0_VPE31_0 + (GIC_SH_MAP_SPACER * 26) (a1) // Source 26 to VPE 2
    sw      a0, GIC_SH_MAP0_VPE31_0 + (GIC_SH_MAP_SPACER * 34) (a1) // Source 34 to VPE 2
    sll     a0, a0, 1          // set bit 3 for CORE3 or for MT vpe3
 	sw      a0, GIC_SH_MAP0_VPE31_0 + (GIC_SH_MAP_SPACER * 27) (a1) // Source 27 to VPE 3
    sw      a0, GIC_SH_MAP0_VPE31_0 + (GIC_SH_MAP_SPACER * 35) (a1) // Source 35 to VPE 3
    sll     a0, a0, 1          // set bit 4 for CORE4 or for MT vpe4
 	sw      a0, GIC_SH_MAP0_VPE31_0 + (GIC_SH_MAP_SPACER * 28) (a1) // Source 28 to VPE 4
    sw      a0, GIC_SH_MAP0_VPE31_0 + (GIC_SH_MAP_SPACER * 36) (a1) // Source 36 to VPE 4
    sll     a0, a0, 1          // set bit 5 for CORE5 or for MT vpe5
 	sw      a0, GIC_SH_MAP0_VPE31_0 + (GIC_SH_MAP_SPACER * 29) (a1) // Source 29 to VPE 5
    sw      a0, GIC_SH_MAP0_VPE31_0 + (GIC_SH_MAP_SPACER * 37) (a1) // Source 37 to VPE 5
    sll     a0, a0, 1          // set bit 6 for CORE6 or for MT vpe6
 	sw      a0, GIC_SH_MAP0_VPE31_0 + (GIC_SH_MAP_SPACER * 30) (a1) // Source 30 to VPE 6
    sw      a0, GIC_SH_MAP0_VPE31_0 + (GIC_SH_MAP_SPACER * 38) (a1) // Source 38 to VPE 6
    sll     a0, a0, 1          // set bit 7 for CORE7 or for MT vpe7
 	sw      a0, GIC_SH_MAP0_VPE31_0 + (GIC_SH_MAP_SPACER * 31) (a1) // Source 31 to VPE 7
    sw      a0, GIC_SH_MAP0_VPE31_0 + (GIC_SH_MAP_SPACER * 39) (a1) // Source 39 to VPE 7

init_vpe_gic:

    // Initialize configuration of per Core or for MT vpe interrupts
    li      a1, (GIC_BASE_ADDR | GIC_CORE_LOCAL_SECTION_OFFSET)
    lw      a3, GIC_COREL_CTL(a1)

map_timer_int:
    ext     a0, a3, TIMER_ROUTABLE, TIMER_ROUTABLE_S
    beqz    a0, map_perfcount_int
    nop

    li      a0, 0x80000005               // Int5 is selected for timer routing
    sw      a0, GIC_COREL_TIMER_MAP(a1)

map_perfcount_int:
    ext     a0, a3, PERFCOUNT_ROUTABLE, PERFCOUNT_ROUTABLE_S
    beqz    a0, done_gic
    nop
    li      a0, 0x80000004               // Int4 is selected for performance routing
    sw      a0, GIC_COREL_PERFCTR_MAP(a1)

done_gic:
    jr      ra
    nop

END(init_gic)
