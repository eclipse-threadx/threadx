/*
 * init_tlb.S
 *
 *  Created on: Jan 12, 2011
 *  Author: MIPS TECHNOLOGIES, INC
 *  Common TLB initialization for MIPS cores
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
#include <m32c0.h>

	.set	noreorder           // Don't allow the assembler to reorder instructions.
	.set	noat                // Don't allow the assembler to use r1(at) for synthetic instr.
/**************************************************************************************
**************************************************************************************/
LEAF(init_tlb)

check_for_tlb:
	// Determine if we have a TLB
	mfc0    v1, C0_CONFIG			// read C0_Config
	ext	    v1, v1, 7, 3			// extract MT field
	li	    a3, 0x1					// load a 1 to check against
	bne	    v1, a3, done_init_tlb
	mfc0    v0, C0_CONFIG1			// C0_Config1
	nop

start_init_tlb:
	// Config1MMUSize == Number of TLB entries - 1
	ext     v1, v0, CFG1_MMUSSHIFT, 6 // extract MMU Size
	mtc0    zero, C0_ENTRYLO0		// write C0_EntryLo0
	mtc0    zero, C0_ENTRYLO1		// write C0_EntryLo1
	mtc0    zero, C0_PAGEMASK		// write C0_PageMask
	mtc0    zero, C0_WIRED			// write C0_Wired
	li	    a0, 0x80000000

next_tlb_entry_pair:
	mtc0    v1, C0_INDEX			// write C0_Index
	mtc0	a0, C0_ENTRYHI			// write C0_EntryHi
	ehb
	tlbwi
	add	    a0, (2<<13)				// Add 8K to the address to avoid TLB conflict with previous entry

	bne		v1, zero, next_tlb_entry_pair
	add	    v1, -1

done_init_tlb:
    jr      ra
    nop
END(init_tlb)
