/*
 * init_L23caches.S
 *
 *  Created on: Jun 8, 2012
 *      Author: chrisr
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
#include <mips/regdef.h>
#include <mips/m32c0.h>
	.set	noreorder           // Don't allow the assembler to reorder instructions.
	.set	noat                // Don't allow the assembler to use r1(at) for synthetic instr.

/**************************************************************************************
**************************************************************************************/
/**************************************************************************************
* Initialize the L2 and L3 caches Uncached version no CPS
**************************************************************************************/
LEAF(init_l23u)

    // Use CCA Override disable the L2 cache or initialize the L2
    // and L3 caches if CCA override is not available.
    // NOTE: If you have a L3 cache you must add code here
    // to disable it or initialize it if it can't be disabled.

    // First check to see if this is a CPS, if not
    // the l2 CCA override is not present and the L2 will have to be initialized
    // from uncached code.
    beqz    r11_is_cps, init_l23
    nop

    // Diaable the L2 cache using CCA override by writting a 0x50 to
    // the GCR Base register. 0x50 enables the CCA override bit and sets
    // the CCA to uncached.

	lw	    a0, 0x0008(r22_gcr_addr)  // Read GCR_BASE
    li      a3, 0x50                  // Enable CCA and set to uncached
    ins     a0, a3, 0, 8    // Insert bits
    sw      a0, 0x0008(r22_gcr_addr)  // Write GCR_BASE

    // Read the GCR_BASE register back to see if the enabling
    // of the CCA override took. If it did skip the L2 and L3
    // initialization. (It will be called later once the L1 cache has
    // been initialized, for better performance.)

	lw	    a0, 0x0008(r22_gcr_addr)  // Read GCR_BASE
    ext     a0, a0, 4, 1    // Extract CCA_Override_Enable
    bnez    a0, done_l23          	// Skip uncached execution if CCA
    											// Override is implemented.
    nop

    // If the code gets here the CCA override is not available so
    // the L2 cache can't be disabled and must be initialized now
    // instead of later.
    b		init_l23
    nop
END(init_l23u)
/**************************************************************************************
* Initialize the L2 and L3 caches cached version is CPS
**************************************************************************************/
LEAF(init_l23c)

    // Skip cached execution if CCA Override is not implemented.
    // If CCA override is not implemented the L2 and L3 caches
    // would have already been initialized when init_l23u was called.

    beqz    r11_is_cps, done_l23
    nop
	lw	    a0, 0x0008(r22_gcr_addr)  // Read GCR_BASE
    bnez    r8_core_num, done_l23			    // Check it for Core0.
    ext     a0, a0, 4, 1    // Extract CCA_Override_Enable bit

    // If CCA override is not set it means that the setting failed in
    // init_l23u and the L2 and L3 caches were initialized at that time
    // If it is set then the code will fall through and initialize the L2/L3 caches
    beqz    a0, done_l23
    nop
END(init_l23c)

LEAF(init_l23)
	// L2 Cache initialization routine

	// Check L2 cache size
	mfc0	v0, C0_CONFIG2		// C0_Config2

	// Isolate L2$ Line Size
	ext	    v1, v0, 4, 4		// extract SL

	// Skip ahead if No L2$
	beq	    v1, zero, done_l2cache
	nop

	li	    a2, 2
	sllv	v1, a2, v1		// Now have true L2$ line size in bytes

	// Isolate L2$ Sets per Way
	ext	    a0, v0, 8, 4		// extract SS
	li	    a2, 64
	sllv	a0, a2, a0		// L2$ Sets per way

	// Isolate L2$ Associativity
	// L2$ Assoc (-1)
	ext	    a1, v0, 0, 4		// extract SA
	add	    a1, 1

	mul	    a0, a0, a1		// Get total number of sets

	lui	    a2, 0x8000		// Get a KSeg0 address for cacheops

	// Clear L23TagLo/L23TagHi registers
	mtc0	zero, C0_TAGLO, 4

	move	a3, a0

	// L2$ Index Store Tag Cache Op
	// Will invalidate the tag entry, clear the lock bit, and clear the LRF bit
next_L2cache_tag:
	cache	0xB, 0(a2)					// Write Tag using index store tag
	add	    a3, -1						// Decrement set counter

	bne	    a3, zero, next_L2cache_tag 	// Done yet?
	add	    a2, v1						// Get next line address

done_l2cache:

	// Isolate L3$ Line Size
	ext	    v1, v0, CFG2_TLSHIFT, 4		// Extract L3 line size

	// Skip ahead if No L3$
	beq	    v1, zero, done_l3cache
	nop

	li	    a2, 2
	sllv	v1, a2, v1					// Decode L3$ line size in bytes

	// Isolate L3$ Sets per Way
	ext	    a0, v0, CFG2_TSSHIFT, 4		// Extract L3 sets per way TDS encoding
	li	    a2, 64
	sllv	a0, a2, a0		// Decode L3 Sets per way

	// Isolate L3$ Associativity
	// L3$ Assoc (-1)
	ext	    a1, v0, CFG2_TASHIFT, 4		// Extrace L3 associativity 2TA encoding
	add	    a1, 1						// Decode L3 associativity (number of sets)
	mul	    a0, a0, a1					// Compute total number of sets

	lui	    a2, 0x8000		   			// Get a KSeg0 address for cacheops

	// Clear L23Tag register
	mtc0	zero, C0_TAGLO, 4


	move	a3, a0

	// L3$ Index Store Tag Cache Op
	// Will invalidate the tag entry, clear the lock bit, and clear the LRF bit
next_L3cache_tag:
	cache	0xA, 0(a2)					// TCIndexStTag
	add	    a3, -1						// Decrement set counter
	bne	    a3, zero, next_L3cache_tag
	add	    a2, v1						// Get next line address

done_l3cache:
    // disable CCA Override
    beqz    r11_is_cps, done_l23
    nop
	lw	    a0, 0x0008(r22_gcr_addr)  	// GCR_BASE
    ins     a0, zero, 0, 8    			// CCA Override disabled
    sw      a0, 0x0008(r22_gcr_addr)  	// GCR_BASE

done_l23:
    jr      ra
    nop
END(init_l23)


