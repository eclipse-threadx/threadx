/*
 * interAptiv/set_gpr_boot_values.S
 *
 *  Created on: May 9, 2012
 *  Author: MIPS TECHNOLOGIES, INC
 *  Set GPR values for interAptiv Family of Cores
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
#include <m32c0.h>
#include <regdef.h>
#include <cps.h>

// defines not yet in m32c0.h
#define C0_MVPCTL $0, 1
#define C0_MVPCONF0 $0, 2
#define C0_VPECTL $1, 1
#define C0_VPECONF0 $1, 2
#define C0_TCSTATUS $2, 1
#define C0_TCRESTART $2, 3
#define C0_TCHALT $2, 4
#define C0_TCBIND $2, 2

	.set	noreorder           // Don't allow the assembler to reorder instructions.
	.set	noat                // Don't allow the assembler to use r1(at) for synthetic instr.
/**************************************************************************************
**************************************************************************************/
LEAF(set_gpr_boot_values)

    li      r1_all_ones, 0xffffffff             // Simplify code and improve clarity
    mfc0    a0, C0_EBASE                			// Read CP0 EBase
	ext	    r23_cpu_num, a0, 0, 4     			// Extract CPUNum
    li      r24_malta_word, MALTA_DISP_ADDR     // Need for reporting failed assertions.
    la      gp, _gp                // All vpe share globals.
    li      sp, STACK_BASE_ADDR     // Each vpe gets it's own stack.
    ins     sp, r23_cpu_num, STACK_SIZE_LOG2, 3

check_mt_ase:
	mfc0	a0, C0_CONFIG1	        			// C0_Config1
	bgez	a0, no_mt_ase             			// bit 31 sign bit set?
	mfc0	a0, C0_CONFIG2	        			// C0_Config2
	bgez	a0, no_mt_ase             			// bit 31 sign bit set?
	mfc0	a0, C0_CONFIG3	        			// C0_Config3
	and	    a0, (1 << 2)              			// M_Config3MT
	beqz	a0, no_mt_ase
    li      r10_has_mt_ase, 0

has_mt_ase:
    li      r10_has_mt_ase, 1

    // Every vpe will set up the following to simplify resource initialization.
    mfc0    a0, C0_TCBIND						// Read CP0 TCBind
	ext	    r9_vpe_num, a0,  0, 4				// Extract CurVPE
	ext	    r18_tc_num, a0, 21, 8				// Extract CurTC
	mfc0    a0, C0_MVPCONF0						// C0_MVPConf0
    ext	    r21_more_tcs, a0, 0, 8   			// Extract PTC
    b       check_cps
	ext	    r20_more_vpes, a0, 10, 4			// Extract VPE

no_mt_ase: // This processor does not implement the MIPS32 MT ASE. Set up defaults.
	sdbbp                                       // Failed assertion: not mt.

check_cps: // Determine if there is a coherency manager present. (Implementation Dependent.)

    mfc0    r25_coreid, C0_PRID					// CP0 PRId.
    ext     a0, r25_coreid, 8, 16				// Extract Manuf and Core.
    li      a3, 0x01A1							// interAptiv Multi core
    beq     a3, a0, is_cps
    nop

is_not_interAptiv: // This processor is not a InterAptive Core so exit!
	sdbbp                                       // Failed assertion: not interAptiv.

is_cps:
	li      r11_is_cps, 1

    // Verify that we can find the GCRs.
	la	    a1, GCR_CONFIG_ADDR					// KSEG1(GCRBASE)
	lw	    a0, GCR_BASE(a1)					// read GCR_BASE
    ins     a1, $0, 29, 3 						// Convert KSEG1 to physical address.
	ins	    a0, $0, 0, 15						// Isolate physical base address of GCR (Clear lower bits).
	beq	    a1, a0, gcr_found
    nop
    sdbbp   // Can't find GCR. RTL config override of MIPS default?

gcr_found:
    // Every vpe will set up the following to simplify resource initialization.
    li      r22_gcr_addr, GCR_CONFIG_ADDR
    lw      r8_core_num, (CORE_LOCAL_CONTROL_BLOCK + GCR_CL_ID) (r22_gcr_addr) // Load GCR_CL_ID
    lw      a0, GCR_CONFIG(r22_gcr_addr)      // Load GCR_CONFIG
	ext	    r19_more_cores, a0, PCORES, PCORES_S // Extract PCORES

done_init_gpr:
    jr     ra
    nop
END(set_gpr_boot_values)
