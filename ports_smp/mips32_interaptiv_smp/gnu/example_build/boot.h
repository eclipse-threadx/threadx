/*
 * boot.h
 *
 *  Created on: Jan 12, 2011
 *      Author: MIPS TECHNOLOGIES, INC
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
#define LEAF(name)\
    .##text;\
    .##globl    name;\
    .##ent  name;\
name:

#define END(name)\
    .##size name,.-name;\
    .##end  name

#define GCR_CONFIG_ADDR     0xbfbf8000  // KSEG0 address of the GCR registers
#define GIC_P_BASE_ADDR		0x1bdc0000  // physical address of the GIC
#define GIC_BASE_ADDR		0xbbdc0000  // KSEG0 address address of the GIC
#define CPC_P_BASE_ADDR 	0x1bde0001  // physical address of the CPC
#define CPC_BASE_ADDR		0xbbde0000  // KSEG0 address address of the CPC

#define DENALI_CTL_SECTION  0xbbc00000
#define MALTA_DISP_ADDR     0xbf000410

#define STACK_BASE_ADDR     0x82000000  /* Change: Base on memory size. */
#define STACK_SIZE_LOG2     22          /* 4Mbytes each */


/**************************************************************************************
 Register use while executing in this file: ("GLOBAL" denotes a common value.)
**************************************************************************************/

#define r1_all_ones     $1   /* at Will hold 0xffffffff to simplify bit insertion of 1's. GLOBAL! */

// $2 - $7 (v0, v1 a0 - a3) reserved for program use

#define r8_core_num    $8  /* t0 Core number. Only core 0 is active after reset. */
#define r9_vpe_num     $9  /* t1 MT ASE VPE number that this TC is bound to (0 if non-MT.) */
#define r10_has_mt_ase  $10   /* t2 Core implements the MT ASE. */
#define r11_is_cps      $11   /* t3 Core is part of a Coherent Processing System. */

// $12 - $15 (t4 - t7) are free to use
// $16, $17 (s0 and s1) reserved for program use

#define r18_tc_num      $18  /* s2 MT ASE TC number (0 if non-MT.) */
#define r19_more_cores  $19  /* s3 Number of cores in CPS in addition to core 0. GLOBAL! */
#define r20_more_vpes   $20  /* s4 Number of vpes in this core in addition to vpe 0. */
#define r21_more_tcs    $21  /* s5 Number of tcs in vpe in addition to the first. */
#define r22_gcr_addr    $22  /* s6 Uncached (kseg1) base address of the Global Config Registers. */
#define r23_cpu_num     $23  /* s7 Unique per vpe "cpu" identifier (CP0 EBase[CPUNUM]). */
#define r24_malta_word  $24  /* t8 Uncached (kseg1) base address of Malta ascii display. GLOBAL! */
#define r25_coreid      $25  /* t9 Copy of cp0 PRiD GLOBAL! */
#define r26_int_addr    $26  /* k0 Interrupt handler scratch address. */
#define r27_int_data    $27  /* k1 Interrupt handler scratch data. */
// $28 gp and $29 sp
#define r30_cpc_addr    $30  /* s8 Address of CPC register block after cpc_init. 0 indicates no CPC. */
// $31 ra


