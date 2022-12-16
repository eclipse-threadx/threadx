/*
 * init_cm.S
 *
 *  Created on: Jan 12, 2011
 *  Author: MIPS TECHNOLOGIES, INC
 *  initializatoin of the Coherency Manager
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
LEAF(init_cm)

	beqz    r11_is_cps, done_cm_init		// skip if not a CPS or CM register verification failed.
	nop

	// Allow each core access to the CM registers (they should only access their local registers.)
	li	    a0, 2							// Start building mask for cores in this cps.
    sll     a0, a0, r19_more_cores
    addiu   a0, -1							// Complete mask.
	sw	    a0, GCR_ACCESS(r22_gcr_addr)	// GCR_ACCESS

    // Check to see if this CPS implements an IOCU.
    lw      a0, GCR_CONFIG(r22_gcr_addr)	// Load GCR_CONFIG
	ext	    a0, a0, NUMIOCU, NUMIOCU_S		// Extract NUMIOCU.
    beqz    a0, done_cm_init
	lui	    a0, 0xffff

	// Disable the CM regions if there is an IOCU.
	sw	    a0, GCR_REG0_BASE(r22_gcr_addr)	// GCR_REG0_BASE
	sw	    a0, GCR_REG0_MASK(r22_gcr_addr)	// GCR_REG0_MASK
	sw	    a0, GCR_REG1_BASE(r22_gcr_addr)	// GCR_REG1_BASE
	sw	    a0, GCR_REG1_MASK(r22_gcr_addr)	// GCR_REG1_MASK
	sw	    a0, GCR_REG2_BASE(r22_gcr_addr)	// GCR_REG2_BASE
	sw	    a0, GCR_REG2_MASK(r22_gcr_addr)	// GCR_REG2_MASK
	sw	    a0, GCR_REG3_BASE(r22_gcr_addr)	// GCR_REG3_BASE
	sw	    a0, GCR_REG3_MASK(r22_gcr_addr)	// GCR_REG3_MASK

done_cm_init:
	jr      ra
	nop
END(init_cm)
