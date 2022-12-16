/*
 * init_vpe1.S
 *
 *  Created on: Jan 13, 2011
 *  Author: MIPS TECHNOLOGIES, INC
 *  Initialize the second vpe and additional TCs
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

// defines not yet in m32c0.h
#define C0_MVPCTL $0, 1
#define C0_MVPCONF0 $0, 2
#define C0_VPECTL $1, 1
#define C0_VPECONF0 $1, 2
#define C0_TCSTATUS $2, 1
#define C0_TCBIND $2, 2
#define C0_TCRESTART $2, 3
#define C0_TCHALT $2, 4

	.set	noreorder           // Don't allow the assembler to reorder instructions.
	.set	noat                // Don't allow the assembler to use r1(at) for synthetic instr.
/**************************************************************************************
**************************************************************************************/
LEAF(init_vpe1)

   // Each vpe will need to set up additional TC bound to it. (No rebinding.)

    beqz    r21_more_tcs, done_init_vpe1   // If there is no .
    nop
    beqz    r20_more_vpes, done_init_vpe1   // If there is no vpe1 then skip init_vpe1.
    nop

	// This is executing on TC0 bound to VPE0.  Therefore VPEConf0.MVP is set.
	// Enter config mode
	mfc0	v0, C0_MVPCTL	// read C0_MVPCtl
	or		v0, (1 << 1)		// set VPC
	mtc0	v0, C0_MVPCTL	// write C0_MVPCtl
	ehb

#define a0_NTCS	a0
#define a2_NVPES	a2
#define a3_TC	a3

	// Get number of a3_TC's and VPE's
	mfc0	v0, C0_MVPCONF0	// read C0_MVPCONF0
	ext		a0_NTCS, v0, 0, 8	// extract PTC
	ext		a2_NVPES, v0, 10, 4	// extract PVPE

	// Initialise a3_TC's/VPE's
	move	a3_TC, zero
nexttc:
	// Select TCn
	mfc0	v0, C0_VPECTL	// read C0_VPECTL
	ins		v0, a3_TC, 0, 8		// insert TargTC
	mtc0	v0, C0_VPECTL	// write C0_VPECTL
	ehb

	// Bind a3_TC to next VPE
	beqz	a3_TC, nextvpe	// Don't rebind TC0
	nop

	// Halt a3_TC being configured
	li		v0, 1			    // set Halt bit
	mttc0	v0, C0_TCHALT	// write C0_TCHALT
	ehb

	move    v1, a3_TC
	// Bind TC to a VPE
	mftc0   v0, C0_TCBIND	// Read C0_TCBind
	ins     v0, v1, 0, 4	// change S_TCBindCurVPE
	mttc0   v0, C0_TCBIND	// write C0_TCBind

	// This code checks to see if the a3_TC mumber is
	// less than the VPE number. This ensures
	// in a system with more than 1 VPE and TCs each
	// VPE will have at least one a3_TC bound to it
	// and any TCs left over will be bound to the last a3_TC
	slt		v1, a2_NVPES, a3_TC
	bnez	v1, 2f			// Bind spare a3_TC's to VPElast
	move	v1, a2_NVPES

	// Set XTC for active a3_TC's
	mftc0	v0, C0_VPECONF0	// read C0_VPECONF0
	ins		v0, a3_TC, 21, 8 // insert XTC
	mttc0	v0, C0_VPECONF0	// write C0_VPECONF0

	move	v1, a3_TC
2:
	// Bind a3_TC to a VPE
	mftc0	v0, C0_TCBIND	// read C0_TCBIND
	ins		v0, v1, 0, 4	// insert VPE
	mttc0	v0, C0_TCBIND	// write C0_TCBIND

	// Set up TCStatus register:
	// Disable Coprocessor Usable bits
	// Disable MDMX/DSP ASE
	// Clear Dirty a3_TC
	// not dynamically allocatable
	// not allocated
	// Kernel mode
	// interrupt exempt
	// ASID 0
	li		v0, (1 << 10)	// set IXMT
	mttc0	v0, C0_TCSTATUS	// write C0_TCSTATUS

	// Initialize the a3_TC's register file
	li v0, 0xdeadbeef
	mttgpr	v0, $1
	mttgpr	v0, $2
	mttgpr	v0, $3
	mttgpr	v0, $4
	mttgpr	v0, $5
	mttgpr	v0, $6
	mttgpr	v0, $7
	mttgpr	v0, $8
	mttgpr	v0, $9
	mttgpr	v0, $10
	mttgpr	v0, $11
	mttgpr	v0, $12
	mttgpr	v0, $13
	mttgpr	v0, $14
	mttgpr	v0, $15
	mttgpr	v0, $16
	mttgpr	v0, $17
	mttgpr	v0, $18
	mttgpr	v0, $19
	mttgpr	v0, $20
	mttgpr	v0, $21
	mttgpr	v0, $22
	mttgpr	v0, $23
	mttgpr	v0, $24
	mttgpr	v0, $25
	mttgpr	v0, $26
	mttgpr	v0, $27
	mttgpr	v0, $28
	mttgpr	v0, $29
	mttgpr	v0, $30
	mttgpr	v0, $31

nextvpe:
	slt		v1, a2_NVPES, a3_TC
	bnez	v1, donevpe		// No more VPE's
	nop

	// Disable multi-threading with a3_TC's
	mftc0	v0, C0_VPECTL	// read C0_VPECTL
	ins		v0, zero, 15, 1	// clear TE
	mttc0	v0, C0_VPECTL	// write C0_VPECTL

	beqz	a3_TC, 1f
	nop

	// For VPE1..n
	// Clear VPA and set master VPE
	mftc0	v0, C0_VPECONF0	// read C0_VPECONF0
	ins		v0, zero, 0, 1	// insert VPA
	or		v0, (1 << 1)	// set MVP
	mttc0	v0, C0_VPECONF0	// write C0_VPECONF0

	mfc0	v0, C0_STATUS	// read vpe0 C0_STATUS
	mttc0	v0, C0_STATUS	// write vpe1 C0_Status

	li		v0, 0x12345678
	mttc0	v0, C0_EPC		// write C0_EPC

	// RAMHACK: vpe0.Cause -> vpe1.Cause (Cause.IV == 1)
	mfc0	v0, C0_CAUSE	// read vpe0 C0_CAUSE
	mttc0	v0, C0_CAUSE	// write vpe1 C0_CAUSE

	mfc0	v0, C0_CONFIG	// read vpe0 C0_CONFIG
	mttc0	v0, C0_CONFIG	// write vpe1 C0_CONFIG

	mftc0	v0, C0_EBASE	// read C0_EBASE
	ext		v0, v0, 0, 10	// extract CPUNum
	mttgpr	v0, r23_cpu_num

	// vpe1 of each core can execute cached as it's L1 I$ has already been initialized.
	// and the L2$ has been initialized or "disabled" via CCA override.
	// RAMHACK: Move entry point to _start.
	// la		a1, __reset_vector
	la		a1, _start
	ins     a1, zero, 29, 1 // Convert to cached kseg0 address in case we linked to kseg1.
	mttc0   a1, C0_TCRESTART // write C0_TCRESTART

	// Yes.. this is undoing all of the work done above... :)
	mftc0	v0, C0_TCSTATUS	// read C0_TCSTATUS
	ins	    v0, zero, 10, 1	// insert IXMT
	ori	    v0, (1 << 13)	// set A
	mttc0	v0, C0_TCSTATUS	// write C0_TCSTATUS

	mttc0	zero, C0_TCHALT	// write C0_TCHALT

	mftc0	v0, C0_VPECONF0	// read C0_VPECONF0
	ori	    v0, 1		    // set VPA
	mttc0	v0, C0_VPECONF0	// write C0_VPECONF0
1:

donevpe:
	addu	a3_TC, 1
	sltu	v1, a0_NTCS, a3_TC
	beqz	v1, nexttc
	nop

	// Exit config mode
	mfc0	v0, C0_MVPCTL	// read C0_MVPCtl
    ori     v0, 1           // set EVP will enable execution by vpe1
	ins	    v0, zero, 1, 1	// insert VPC
	mtc0	v0, C0_MVPCTL	// write C0_MVPCtl
	ehb

#undef a0_NTCS
#undef a2_NVPES
#undef a3_TC

done_init_vpe1:

    jr   ra
    nop
END(init_vpe1)

