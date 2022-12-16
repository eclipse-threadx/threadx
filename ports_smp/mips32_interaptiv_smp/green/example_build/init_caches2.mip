/*
 * init_caches2.S
 *
 *  Created on: March 30, 2012
 *  Author: MIPS TECHNOLOGIES, INC
 *  Common Cache initialization for a coherent processing system
 *
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
* init_icache invalidates all Instruction cache entries
**************************************************************************************/

LEAF(init_icache)

	// For this Core there is always a I cache
	// The IS field determines how may set there are
	// IS = 2 there are 256 sets per way 1024 total
	// IS = 3 there are 512 sets per way 2048 total

	// v1 set to line size, will be used to increment through the cache tags
	li		v1, 32 					// Line size is always 32 bytes.
	mfc0	v0, C0_CONFIG1			// Read C0_Config1
	ext	    a3, v0, CFG1_ILSHIFT, 3 		// Extract IS
	li	    a2, 2		    		// Used to test against
	beq		a2, a3, Isets_done	 	// if  IS = 2
	li		a3, 1024					// sets = 256
	li		a3, 2048 				// else sets = 512 Skipped if branch taken
Isets_done:
	lui	    a2, 0x8000				// Get a KSeg0 address for cacheops
	// clear the lock bit, valid bit, and the LRF bit
	mtc0    zero, C0_TAGLO			// Clear C0_ITagLo to invalidate entry

next_icache_tag:
	cache   0x8, 0(a2)				 // Index Store tag Cache opt
	add	    a3, -1					 // Decrement set counter
	bne     a3, zero, next_icache_tag // Done yet?
	add     a2, v1		    	 // Increment line address by line size

done_icache:

    jr      ra
    nop
END(init_icache)

/**************************************************************************************
* init_dcache invalidates all data cache entries
**************************************************************************************/

LEAF(init_dcache)

	// For this Core there is always a D cache
	// The DS field determines how may set there are
	// DS = 2 there are 256 sets per way 1024 total
	// DS = 3 there are 512 sets per way 2048 total

	// v1 set to line size, will be used to increment through the cache tags
	li		v1, 32 					// Line size is always 32 bytes.
	mfc0	v0, C0_CONFIG1			// Read C0_Config1
	ext	    a3, v0, CFG1_DSSHIFT, 3 // Extract DS
	li	    a2, 2		    		// Used to test against
	beq		a2, a3, Dsets_done 		// if  DS = 2
	li		a3, 1024				// sets = 256
	li		a3, 2048 				// else sets = 512 Skipped if branch taken
Dsets_done:
	lui	    a2, 0x8000		    	// Get a KSeg0 address for cacheops
	// clear the lock bit, valid bit, and the LRF bit
	mtc0    zero, C0_TAGLO, 2		// Clear C0_DTagLo to invalidate entry

next_dcache_tag:
    cache	0x9, 0(a2)				 // Index Store tag Cache opt
	add	    a3, -1		    		 // Decrement set counter
	bne	    a3, zero, next_dcache_tag // Done yet?
	add	    a2, v1	    			 // Increment line address by line size

done_dcache:
    jr      ra
    nop
END(init_dcache)

/**************************************************************************************
**************************************************************************************/
LEAF(disable_L23)

	bnez    r8_core_num, done_disable_L23	# Only done from core 0.

    // Use CCA Override disable the L2 cache
    // NOTE: If you have a L3 cache you must add code here
    // to disable it or initialize it if it can't be disabled.
    // Disable the L2 cache using CCA override by writing a 0x50 to
    // the GCR Base register. 0x50 enables the CCA override bit and sets
    // the CCA to uncached.

	lw	    a0, 0x0008(r22_gcr_addr)  // Read GCR_BASE
    li      a3, 0x50                  // Enable CCA and set to uncached
    ins     a0, a3, 0, 8    // Insert bits
    sw      a0, 0x0008(r22_gcr_addr)  // Write GCR_BASE

done_disable_L23:
    jr      ra
    nop
END(disable_L23)
/**************************************************************************************
* Initialize the L2 and L3 caches
**************************************************************************************/
LEAF(init_L23)

	bnez    r8_core_num, done_cach_init			# Only done from core 0.

	// L2 Cache initialization routine
	// Check L2 cache size
	mfc0	v0, C0_CONFIG2		// Read C0_Config2
	// Isolate L2$ Line Size
	ext	    v1, v0, 4, 4		// extract L2 line size
	li	    a2, 2
	sllv	v1, a2, v1			// Now have true L2$ line size in bytes

	// Isolate L2 Sets per Way (cache lines per way)
	ext	    a3, v0, 8, 4		// extrace sets per way encoding
	li	    a2, 64
	sllv	a3, a2, a3			// L2$ Sets per way

	// Isolate L2 Associativity (number of ways)
	// L2$ Assoc (-1)
	ext	    a1, v0, 0, 4		// extract ways encoding
	add	    a1, 1				// Decode L2 number of ways

	mul	    a3, a3, a1			// Get total number of sets (sets per way * number of ways)

	lui	    a2, 0x8000			// Get a KSeg0 address for cacheops

	// Clear L23TagLo/L23TagHi registers these are used to set the cache tag
	mtc0	zero, C0_TAGLO, 4
	mtc0	zero, C0_TAGHI, 4

	// L2$ Index Store Tag Cache Op
	// Will invalidate the tag entry, clear the lock bit, and clear the LRF bit
next_L2cache_tag:
	cache	0xB, 0(a2)			// Write Tag using index store tag
	add	    a3, -1				// Decrement set counter

	bne	    a3, zero, next_L2cache_tag // Done yet?
	add	    a2, v1				// Get next line address (each tag covers one line)


	// Start of L3 cache initialization
	// Isolate L3$ Line Size
	ext	    v1, v0, CFG2_TLSHIFT, 4		// Extract L3 line size

	// Skip ahead if No L3$
	beq	    v1, zero, done_l3cache
	nop

	li	    a2, 2
	sllv	v1, a2, v1			// Decode L3$ line size in bytes

	// Isolate L3$ Sets per Way (cache lines per way)
	ext	    a3, v0, CFG2_TSSHIFT, 4		// Extract L3 sets per way TDS encoding
	li	    a2, 64
	sllv	a3, a2, a3			// Decode L3 Sets per way

	// Isolate L3$ Associativity (number of ways)
	// L3$ Assoc (-1)
	ext	    a1, v0, CFG2_TASHIFT, 4		// Extrace L3 associativity 2TA encoding
	add	    a1, 1				// Decode L3 associativity (number of sets)
	mul	    a3, a3, a1			// Compute total number of sets
	lui	    a2, 0x8000		    // Get a KSeg0 address for cacheops

	// Clear L23TagLo/L23TagHi registers
	mtc0	zero, C0_TAGLO, 4
	mtc0	zero, C0_TAGHI, 4

	// L3 Index Store Tag Cache Op
	// Will invalidate the tag entry, clear the lock bit, and clear the LRF bit
next_L3cache_tag:
	cache	0xA, 0(a2)			// TCIndexStTag
	add	    a3, -1				// Decrement set counter
	bne	    a3, zero, next_L3cache_tag
	add	    a2, v1				// Get next line address

done_l3cache:
    // disable CCA Override to enable L2 cache
	lw	    a0, 0x0008(r22_gcr_addr)  // GCR_BASE
    ins     a0, zero, 0, 8            // CCA Override disabled
    sw      a0, 0x0008(r22_gcr_addr)  // GCR_BASE

done_cach_init:
    jr      ra
    nop
END(init_L23)

LEAF(change_k0_cca)
	// NOTE! This code must be executed in KSEG1 (not KSGE0 uncached)
    // Set CCA for kseg0 to cacheable
	mfc0	v0, C0_CONFIG	// read C0_Config
	beqz    r11_is_cps, set_kseg0_cca
	li	    v1, 3			// CCA for non coherent core
	li	    v1, 5			// CCA for coherent cores

set_kseg0_cca:
    ins	    v0, v1, 0, 3	// instert K0
	mtc0	v0, C0_CONFIG	// write C0_Config
	jr.hb      ra
    nop

END(change_k0_cca)
