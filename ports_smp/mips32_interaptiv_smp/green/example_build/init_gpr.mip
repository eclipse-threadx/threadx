/*
 * init_gpr.S
 *
 *  Created on: Jan 12, 2011
 *      Author: MIPS TECHNOLOGIES, INC
 *  Start of boot code for 24K Family of Cores
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
LEAF(init_gpr)

	// Initialize the general purpose registers and any shadow register sets.
	// Although not necessary, register initialization may be useful during boot,
    // debug, and simulation when certain ways of initializing registers may not work
    // (xor rN, rN, rN for example.)

	// Initialize register sets
    li      $1, 0xdeadbeef      // (0xdeadbeef stands out, kseg2 mapped, odd.)

	// Determine how many shadow sets are implemented (in addition to the base register set.)
	// the first time thru the loop it will initialize using $1 set above.
	// At the bottom og the loop, 1 is  subtract from $30
	// and loop back to next_shadow_set to start the next loop and the next lowest set number.
	mfc0	$29, C0_SRSCTL		// read C0_SRSCtl
	ext	    $30, $29, 26, 4		// extract HSS

next_shadow_set:
	// set PSS to shadow set to be initialized
	ins	    $29, $30, 6, 4		// insert PSS
	mtc0	$29, C0_SRSCTL		// write C0_SRSCtl

	wrpgpr	$1, $1
	wrpgpr	$2, $1
	wrpgpr	$3, $1
	wrpgpr	$4, $1
	wrpgpr	$5, $1
	wrpgpr	$6, $1
	wrpgpr	$7, $1
	wrpgpr	$8, $1
	wrpgpr	$9, $1
	wrpgpr	$10, $1
	wrpgpr	$11, $1
	wrpgpr	$12, $1
	wrpgpr	$13, $1
	wrpgpr	$14, $1
	wrpgpr	$15, $1
	wrpgpr	$16, $1
	wrpgpr	$17, $1
	wrpgpr	$18, $1
	wrpgpr	$19, $1
	wrpgpr	$20, $1
	wrpgpr	$21, $1
	wrpgpr	$22, $1
	wrpgpr	$23, $1
	wrpgpr	$24, $1
	wrpgpr	$25, $1
	wrpgpr	$26, $1
	wrpgpr	$27, $1
	wrpgpr	$28, $1
	wrpgpr	$29, $1
	beqz    $30, done_init_gpr // early exit when we get to set 0 so we don't clobber return in $31
	nop
	wrpgpr	$30, $1
	wrpgpr	$31, $1
	b	next_shadow_set
	add	$30, -1  // Since the code started with the highest set number this decrements to the next lower number


done_init_gpr:
    jr      ra
    nop
END(init_gpr)

