/*
 * release_mp.S
 *
 *  Created on: Jan 12, 2011
 *  Author: MIPS TECHNOLOGIES, INC
 *  Release other processors so they can boot
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
LEAF(release_mp)

    blez    r19_more_cores, done_release_mp     // If no more cores then we are done.
    li      a3, 1

    beqz    r30_cpc_addr, release_next_core     // If no CPC then use GCR_CO_RESET_RELEASE
    nop											// else use CPC Power Up command.

powerup_next_core:
    // Send PwrUp command to next core causing execution at their reset exception vector.
    move    a0, a3
    sll     a0, 16
    sw      a0, (CPS_CORE_LOCAL_CONTROL_BLOCK | CPC_OTHERL_REG)(r30_cpc_addr)
    li      a0, PWR_UP							// "PwrUp" power domain command.
    sw      a0, (CPS_CORE_OTHER_CONTROL_BLOCK | CPC_CMDO_REG)(r30_cpc_addr)
    bne     r19_more_cores, a3, powerup_next_core
    add     a3, a3, 1

    jr      ra
    nop

release_next_core:
    // Release next core to execute at their reset exception vector.
    move    a0, a3
    sll     a0, 16
    sw      a0, (CORE_LOCAL_CONTROL_BLOCK | GCR_CL_OTHER)(r22_gcr_addr) // GCR_CL_OTHER
    sw      zero, 0x4000(r22_gcr_addr)           // GCR_CO_RESET_RELEASE
    bne     r19_more_cores, a3, release_next_core
    add     a3, a3, 1

done_release_mp:
    jr      ra
    nop
END(release_mp)
