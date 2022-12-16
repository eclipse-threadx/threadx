/*
 * copy_c2_ram.S
 *
 *  Created on: Jan 12, 2011
 *  Author: MIPS TECHNOLOGIES, INC
 *  Copy code and data to ram then clear BSS
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
#include <regdef.h>
#include <boot.h>

#define s1_all_ones     s1   /* at Will hold 0xffffffff to simplify bit insertion of 1's. */
#define a0_temp_data    a0   /* a0 data to be moved */
#define a1_temp_addr    a1   /* from address */
#define a2_temp_dest    a2   /* to address */
#define a3_temp_mark    a3   /* ending address */


	.set	noreorder           # Don't allow the assembler to reorder instructions.
	.set	noat                # Don't allow the assembler to use r1(at) for synthetic instr.

/**************************************************************************************
**************************************************************************************/
LEAF(copy_c2_ram)

	li      s1_all_ones, 0xffffffff

/* RAMHACK: Link addr == load addr. No copy of code from ROM to RAM required.
    // Copy code and read-only/initialized data from FLASH to (uncached) RAM.
    la      a1_temp_addr, _zap1
    ins     a1_temp_addr, s1_all_ones, 29, 1
    la      a2_temp_dest, _ftext_ram
    ins     a2_temp_dest, s1_all_ones, 29, 1
    la      a3_temp_mark, _edata_ram
    ins     a3_temp_mark, s1_all_ones, 29, 1
    beq     a2_temp_dest, a3_temp_mark, zero_bss
    nop
next_ram_word:
    lw      a0_temp_data, 0(a1_temp_addr)
    sw      a0_temp_data, 0(a2_temp_dest)
    addiu   a2_temp_dest, 4
    bne     a3_temp_mark, a2_temp_dest, next_ram_word
    addiu   a1_temp_addr, 4
*/

// RAMHACK: Zero sbss in addition to bss.
zero_sbss:
    la      a1_temp_addr, _start_sbss
    ins     a1_temp_addr, s1_all_ones, 29, 1
    la      a3_temp_mark, _end_sbss
    ins     a3_temp_mark, s1_all_ones, 29, 1
    beq     a1_temp_addr, a3_temp_mark, zero_bss
    nop
next_sbss_word:
    sw      zero, 0(a1_temp_addr)
    addiu   a1_temp_addr, 4
    bne     a1_temp_addr, a3_temp_mark, next_sbss_word
    nop

zero_bss:
    la      a1_temp_addr, _start_bss
    ins     a1_temp_addr, s1_all_ones, 29, 1
    la      a3_temp_mark, _end_bss
    ins     a3_temp_mark, s1_all_ones, 29, 1
    beq     a1_temp_addr, a3_temp_mark, copy_c2_ram_done
    nop
next_bss_word:
    sw      zero, 0(a1_temp_addr)
    addiu   a1_temp_addr, 4
    bne     a1_temp_addr, a3_temp_mark, next_bss_word
    nop

copy_c2_ram_done:
    jr      ra
    nop
END(copy_c2_ram)

