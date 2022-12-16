/*
 * initcp0.S
 *
 *  Created on: Jan 12, 2011
 *  Author: MIPS TECHNOLOGIES, INC
 *  Common CP0 register initializations
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
LEAF(init_cp0)
	// Initialize Status
	// RAMHACK: Use RAM (Status.BEV == 0) exception vectors.
	// li	    v1, 0x00400404		// (M_StatusIM | M_StatusERL | M_StatusBEV)
    	li	    v1, 0x00000404      // (M_StatusIM | M_StatusERL)
	mtc0	v1, C0_STATUS		// write C0_Status

	// Initialize Watch registers if implemented.
	mfc0	v0, C0_CONFIG1		// read C0_Config1
	ext	    v1, v0, 3, 1		// extract bit 3 WR (Watch registers implemented)
	beq 	v1, zero, done_wr
	li    	v1, 0x7				// (M_WatchHiI | M_WatchHiR | M_WatchHiW)

	// Clear Watch Status bits and disable watch exceptions
	mtc0	v1, C0_WATCHHI		// write C0_WatchHi0
	mfc0	v0, C0_WATCHHI		// read C0_WatchHi0
	bgez 	v0, done_wr			// Check for bit 31 (sign bit) for more Watch registers
	mtc0	zero, C0_WATCHLO	// write C0_WatchLo0

	mtc0	v1, C0_WATCHHI, 1	// write C0_WatchHi1
	mfc0	v0, C0_WATCHHI, 1	// read C0_WatchHi1
	bgez 	v0, done_wr			// Check for bit 31 (sign bit) for more Watch registers
	mtc0	zero, C0_WATCHLO, 1	// write C0_WatchLo1

	mtc0	v1, C0_WATCHHI, 2	// write C0_WatchHi2
	mfc0	v0, C0_WATCHHI, 2	// read C0_WatchHi2
	bgez 	v0, done_wr			// Check for bit 31 (sign bit) for more Watch registers
	mtc0	zero, C0_WATCHLO, 2	// write C0_WatchLo2

	mtc0	v1, C0_WATCHHI, 3	// write C0_WatchHi3
	mfc0	v0, C0_WATCHHI, 3	// read C0_WatchHi3
	bgez 	v0, done_wr			// Check for bit 31 (sign bit) for more Watch registers
	mtc0	zero, C0_WATCHLO, 3	// write C0_WatchLo3

	mtc0	v1, C0_WATCHHI, 4	// write C0_WatchHi4
	mfc0	v0, C0_WATCHHI, 4	// read C0_WatchHi4
	bgez 	v0, done_wr			// Check for bit 31 (sign bit) for more Watch registers
	mtc0	zero, C0_WATCHLO, 4	// write C0_WatchLo4

	mtc0	v1, C0_WATCHHI, 5	// write C0_WatchHi5
	mfc0	v0, C0_WATCHHI, 5	// read C0_WatchHi5
	bgez 	v0, done_wr			// Check for bit 31 (sign bit) for more Watch registers
	mtc0	zero, C0_WATCHLO, 5	// write C0_WatchLo5

	mtc0	v1, C0_WATCHHI, 6	// write C0_WatchHi6
	mfc0	v0, C0_WATCHHI, 6	// read C0_WatchHi6
	bgez 	v0, done_wr			// Check for bit 31 (sign bit) for more Watch registers
	mtc0	zero, C0_WATCHLO, 6	// write C0_WatchLo6

	mtc0	v1, C0_WATCHHI, 7	// write C0_WatchHi7
	mtc0	zero, C0_WATCHLO, 7	// write C0_WatchLo7

done_wr:

	// Clear WP bit to avoid watch exception upon user code entry, IV, and software interrupts.
	// RAMHACK: Use vectored interrupts (Cause.IV) to isolate them from other exceptions.
	lui     v1, 0x0080              // Cause.IV = 1
	mtc0	v1, C0_CAUSE		// write C0_Cause: Init AFTER init of CP0 WatchHi/Lo registers.

	// Clear timer interrupt. (Count was cleared at the reset vector to allow timing boot.)
	mtc0	zero, C0_COMPARE	// write C0_Compare

    jr      ra
	nop
END(init_cp0)
