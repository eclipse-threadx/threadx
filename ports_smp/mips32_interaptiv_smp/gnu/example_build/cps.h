/*
 * cps.h
 *
 *  Created on: May 1, 2012
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

/*
 * This include file contains #defines for the memory mapped registers in a coherent Processing system
 * of both single cores and multi threaded cores. It contains registers offset for all the registers,
 * defines for the fields with in the registers and encodings for some of the fields.
 *
 * The fields in the registers are defined by a pair of #defines, one define is the starting bit position of
 * the field and another, (with a "_S" appended to the name), is the size of the field. Here is an example of
 * how you would use these #defines in the the extraction of a filed:
 *
 *   li      $5, GIC_BASE_ADDR			         # load GIC KSEG0 Address
 *   lw      $4, GIC_SH_CONFIG($5)               # Read the GIC_SH_CONFIG Register
 *   ext     $4, NUMINTERRUPTS, NUMINTERRUPTS_S  # Extract NUMINTERRUPTS
 *
 *   The names for the registers and fields are usually the same as used in the Software Users Manual
 *   (SUM). The exceptions occur when the names would conflict with each other in that case the name
 *   is appended with something to make it unique.
 */

#ifndef CPS_H_
#define CPS_H_

// GCR Offset for GCR_CONFIG, field positions and field size
#define GCR_CONFIG 0x0000
#define NUM_ADDR_REGIONS 16
#define NUM_ADDR_REGIONS_S 4
#define NUMIOCU 8
#define NUMIOCU_S 4
#define PCORES 0
#define PCORES_S 8

// GCR Offset for GCR_BASE, field positions and field size
#define GCR_BASE 0x0008
#define GCR_BASE_ADDR 15
#define GCR_BASE_ADDR_S 17
#define CCA_DEFAULT_OVERRIDE_VALUE 5
#define CCA_DEFAULT_OVERRIDE_VALUE_S 3
#define CCA_DEFAULT_OVERRIDE_ENABLE 4
#define CCA_DEFAULT_OVERRIDE_ENABLE_S 1
#define CM_DEFAULT_TARGET 0
#define CM_DEFAULT_TARGET_S 2

// GCR Offset for GCR_CONTROL, field positions and field size
#define GCR_CONTROL 0x0010
#define SYNCCTL 16
#define SYNCCTL_S 1
#define CM_SYNC_TX_DISABLE 5
#define CM_SYNC_TX_DISABLE_S 1
#define CM_AUTO_CLR_IVU_EN 4
#define CM_AUTO_CLR_IVU_EN_S 1
#define CM_COHST_SH_ALWAYS_EN 3
#define CM_COHST_SH_ALWAYS_EN_S 1
#define CM_PARK_EN 2
#define CM_PARK_EN_S 1
#define CM_DISABLE_MMIO_LIMIT 1
#define CM_DISABLE_MMIO_LIMIT_S 1
#define CM_SPEC_READ_EN 0
#define CM_SPEC_READ_EN_S 1

// GCR Offset for GCR_CONTROL_2, field positions and field size
#define GCR_CONTROL2 0x0018
#define L2_CACHEOP_LIMIT 16
#define L2_CACHEOP_LIMIT_S 4
#define L1_CACHEOP_LIMIT 3
#define L1_CACHEOP_LIMIT_S 4

// GCR Offset for GCR_ACCESS, field positions and field size
#define GCR_ACCESS 0x0020
#define CM_ACCESS_EN 0
#define CM_ACCESS_EN_S 8

// GCR Offset for GCR_REV, field positions and field size
#define GCR_REV 0x0030
#define MAJOR_REV_GCR 8
#define MAJOR_REV_GCR_S 8
#define MINOR_REV_GCR 7
#define MINOR_REV_GCR_S 8

// GCR Offset for GCR_ERROR_MASK
#define GCR_ERROR_MASK 0x0040

// error type encoding
#define GC_WR_ERR 1
#define GC_RD_ERR 2
#define COH_WR_ERR 3
#define COH_RD_ERR 4
#define MMIO_WR_ERR 5
#define MMIO_RD_ERR 6
#define INTVN_WR_ERR 17
#define INTVN_RD_ERR 18

// GCR Offset for GCR_ERROR_CAUSE, field positions and field size
#define GCR_ERROR_CAUSE 0x0048
#define CM_ERROR_TYPE 27
#define CM_ERROR_TYPE_S 5
#define CM_ERROR_INFO 0
#define CM_ERROR_INFO_S 27

// Fields and sizes for Error Types 1 - 5
#define CCA 15
#define CCA_S 3
#define TARGET_REGION 12
#define TARGET_REGION_S 3
#define OCP_MCMD_ERROR 7
#define OCP MCMD_ERROR_S 5
#define SOURCE_TAGID 3
#define SOURCE_TAGID_S 4
#define SOURCE_PORT 0
#define SOURCE_PORT_S 3

// Fields for Error Types 16 - 17
#define COHERENT_STATE_CORE_3 19
#define COHERENT_STATE_CORE_3_S 2
#define INTERVENTION_SRESP_CORE3 18
#define INTERVENTION_SRESP_CORE3_S 1

#define COHERENT_STATE_CORE_2 16
#define COHERENT_STATE_CORE_2_S 2
#define INTERVENTION_SRESP_CORE2 15
#define INTERVENTION_SRESP_CORE2_S 1

#define COHERENT_STATE_CORE_1 13
#define COHERENT_STATE_CORE_1_S 2
#define INTERVENTION_SRESP_CORE1 12
#define INTERVENTION_SRESP_CORE1_S 1

#define COHERENT_STATE_CORE_0 10
#define COHERENT_STATE_CORE_0_S 2
#define INTERVENTION_SRESP_CORE0 9
#define INTERVENTION_SRESP_CORE0_S 1

#define FROM_STORE_CONDITIONAL 8
#define FROM_STORE_CONDITIONAL_S 1
#define OCP_MCMD 3
#define OCP_MCMD_S 5
#define SOURCE_PORT 0
#define SOURCE_PORT_S 3

// Coherent state encoding
#define CS_INVALID 0
#define CS_SHARED 1
#define CS_MODIFID 2
#define CS_EXCLUSIVE

// Intervention Response encoding
#define IR_OK 0
#define IR_DATA 1

// MCmd Encodings for CM_ERROR_INFO
#define LEGACY_WRITE 0x01
#define LEGACY_READ 0x02
#define COHERENT_READ_OWN 0x08
#define COHERENT_READ_SHARE 0x09
#define COHERENT_READ_DISCARD 0x0A
#define COHERENT_READ_SHARE_ALWAYS 0x0B
#define COHERENT_UPGRADE 0x0C
#define COHERENT_WRITEBACK 0x0D
#define COHERENT_COPYBACK 0x10
#define COHERENT_COPYBACK_INVALIADATE 0x11
#define COHERENT_INVALIADATE 0x12
#define COHERENT_WRITE_INVALIADATE 0x13
#define COHERENT_COMPLETION_SYNC 0x14

// GCR Offset for GCR_ERROR_ADDR
#define GCR_ERROR_ADDR 0x0050

// GCR Offset for GCR_ERROR_MULT
#define GCR_ERROR_MULT 0x0058
#define CM_ERROR_2ND 0
#define CM_ERROR_2ND_S 5

// GCR Offset for GCR_GIC_BASE, fields and sizes
#define GCR_GIC_BASE 0x0080
#define GIC_BASEADDRESS 17
#define GIC_BASEADDRESS_S 15
#define GIC_EN 0
#define GIC_EN_S 1

// GCR Offset for GCR_CPC_BAS, fields and sizes
#define GCR_CPC_BASE 0x0088
#define CPC_BASEADDRESS 15
#define CPC_BASEADDRESS_S 17
#define CPC_EN 0
#define CPC_EN_S 1

// GCR Offset for GCR_REGn_BASE, fields and sizes
#define GCR_REG0_BASE 0x0090
#define GCR_REG1_BASE 0x00A0
#define GCR_REG2_BASE 0x00B0
#define GCR_REG3_BASE 0x00C0
#define GCR_REG4_BASE 0x0190
#define GCR_REG5_BASE 0x01A0

#define CM_REGION_BASEADDRESS 16
#define CM_REGION_BASEADDRESS_S 16

// GCR Offset for GCR_REGn_MASK, fields, sizes and encodings
#define GCR_REG0_MASK 0x0098
#define GCR_REG1_MASK 0x00A8
#define GCR_REG2_MASK 0x00B8
#define GCR_REG3_MASK 0x00C8
#define GCR_REG4_MASK 0x0198
#define GCR_REG5_MASK 0x01A8
#define CM_REGION_ADDRESS_MASK 16
#define CM_REGION_ADDRESS_MASK_S 16
#define CCA_OVERRIDE_VALUE 5
#define CCA_OVERRIDE_VALUE_S 3
#define CCA_OVERRIDE_ENABLE 4
#define CCA_OVERRIDE_ENABLE_S 1
#define CM_REGION_TARGET 0
#define CM_REGION_TARGET_S 2
// CM_REGION_TARGET encoding
#define CM_REGION_TARGET_DISABLE 0x0
#define CM_REGION_TARGET_MEMORY 0x1
#define CM_REGION_TARGET_IOCU 0x2

// GCR Offset for GCR_GIC_STATUS, fields and sizes
#define GCR_GIC_STATUS 0x00D0
#define GIC_EX 0
#define GIC_EX_S 1

// GCR Offset for GCR_CACHE_REV, fields and sizes
#define GCR_CACHE_REV 0x00E0
#define MAJOR_REV_CACHE 8
#define MAJOR_REV_CACHE_S 8
#define MINOR_REV_CACHE 7
#define MINOR_REV_CACHE_S 8

// GCR Offset for GCR_CPC_STATUS, fields and sizes
#define GCR_CPC_STATUS 0x00F0
#define CPC_EX 0
#define CPC_EX_S 1

// GCR Offset for GCR_IOCU1_REV, fields and sizes
#define GCR_IOCU1_REV 0x0200
#define MAJOR_REV_IOCU 8
#define MAJOR_REV_IOCU_S 8
#define MINOR_REV_IOCU 7
#define MINOR_REV_IOCU_S 8

// GCR Core Local and Core other offsets
#define CORE_LOCAL_CONTROL_BLOCK 0x2000
#define CORE_OTHER_CONTROL_BLOCK 0x4000

// GCR Core Local and Other COHERENCE, fields and sizes
#define GCR_CL_COHERENCE 0x0008
#define GCR_CO_COHERENCE 0x0008
#define COH_DOMAIN_EN 0
#define COH_DOMAIN_EN_S 8

// GCR Core Local and Other CONFIG , fields and sizes
#define GCR_CL_CONFIG 0x0010
#define GCR_CO_CONFIG 0x0010
#define IOCU_TYPE 10
#define IOCU_TYPE_S 2
#define PVPE 0
#define PVPE_S 10

// GCR Core Local and Other OTHER, fields and sizes
#define GCR_CL_OTHER 0x0018
#define GCR_CO_OTHER 0x0018
#define OTHER_CORE_NUM 16
#define OTHER_CORE_NUM_S 16

// GCR Core Local and Other RESET_BASE, fields and sizes
#define GCR_CL_RESET_BASE 0x0020
#define GCR_CO_RESET_BASE 0x0020
#define BEV_EXCEPTION_BASE 12
#define BEV_EXCEPTION_BASE_S 20

// GCR Core Local and Other ID
#define GCR_CL_ID 0x0028
#define GCR_CO_ID 0x0028

// GCR Global Debug Block Offsets
#define Global_Debug_Block 0x6000

// GCR Global Debug GCR_DB_TCBCONTROLB, fields and sizes
#define GCR_DB_TCBCONTROLB 0x0008
#define WE_DB_TCBCONTROLB 31
#define WE_DB_TCBCONTROLB_S 1
#define TWSRC_WIDTH 26
#define TWSRC_WIDTH_S 2
#define TRPAD 18
#define TRPAD_S 1
#define RM 16
#define RM_S 1
#define TR 15
#define TR_S 1
#define BF 14
#define BF_S 1
#define TM 12
#define TM_S 2
#define CR 8
#define CR_S 3
#define CAL 7
#define CAL_S 1
#define OFC 1
#define OFC_S 1
#define FUNNEL_TRACE_ENABLE 0
#define FUNNEL_TRACE_ENABLE_S 1

// GCR Global Debug GCR_DB_TCBCONTROLD, fields and sizes
#define GCR_DB_TCBCONTROLD 0x0010
#define P4_CTL 24
#define P4_CTL_S 2
#define P3_CTL 22
#define P3_CTL_S 2
#define P2_CTL 20
#define P2_CTL_S 2
#define P1_CTL 18
#define P1_CTL_S 2
#define P0_CTL 16
#define P0_CTL_S 2
#define TW_SRC_VAL 8
#define TW_SRC_VAL_S 3
#define TRACE_WB 7
#define TRACE_WB_S 1
#define CM_INHIBIT_OVERFLOW 5
#define CM_INHIBIT_OVERFLOW_S 1
#define TLEV 3
#define TLEV_S 2
#define AE_PER_PORT 2
#define AE_PER_PORT_S 1
#define GLOBAL_CM_EN 1
#define GLOBAL_CM_EN_S 1
#define CM_EN 0
#define CM_EN_S 1

// GCR Global Debug GCR_DB_TCBCONTROLE, fields and sizes
#define GCR_DB_TCBCONTROLE 0x0020
#define TrIdle 8
#define TrIdle_S 1
#define PeC 0
#define PeC_S 1

// GCR Global Debug GCR_DB_TCBConfig, fields and sizes
#define GCR_DB_TCBCONFIG 0x0028
#define CF1 31
#define CF1_S 1
#define SZ 17
#define SZ_S 4
#define CRMAX 14
#define CRMAX_S 3
#define CRMIN 11
#define CRMIN_S 3
#define PW 9
#define PW_S 2
#define ONT 5
#define ONT_S 1
#define OFT 4
#define OFT_S 1
#define TCB_REV 0
#define TCB_REV_S 4

// GCR Global Debug GCR_DB_PC_CTL, fields and sizes
#define GCR_DB_PC_CTL 0x0100
#define PERF_INT_EN 30
#define PERF_INT_EN_S 1
#define PERF_OVF_STOP 29
#define PERF_OVF_STOP_S 1
#define P1_RESET 9
#define P1_RESET_S 1
#define P1_COUNT_ON 8
#define P1_COUNT_ON_S 1
#define P0_RESET 7
#define P0_RESET_S 1
#define P0_COUNT_ON 6
#define P0_COUNT_ON_S 1
#define CYCL_CNT_RESET 5
#define CYCL_CNT_RESET_S 1
#define CYCL_CNT__ON 4
#define CYCL_CNT__ON_S 1
#define PERF_NUM_CNT 0
#define PERF_NUM_CNT_S 4

// GCR Global Debug Read Pointer GCR_DB_TCBRDP
#define GCR_DB_TCBRDP 0x0108

// GCR Global Debug Write Pointer GCR_DB_TCBWDP
#define GCR_DB_TCBWRP 0x0110

// GCR Global Debug Start Pointer GCR_DB_TCBSTP
#define GCR_DB_TCBSTP 0x0118

// GCR_DB_PC_OV, fields and sizes
#define GCR_DB_PC_OV 0x0120
#define P1_OVERFLOW 2
#define P1_OVERFLOW_S 1
#define P0_OVERFLOW 1
#define P0_OVERFLOW_S 1_S 1
#define CYCL_CNT_OVERFLOW 0
#define CYCL_CNT_OVERFLOW_S 1

// GCR Global Debug GCR_DB_PC_EVENT, fields and sizes
#define GCR_DB_PC_EVENT 0x0130
#define P1_EVENT 8
#define P1_EVENT_S 8
#define P0_EVENT 0
#define P0_EVENT_S 8

// GCR Global Debug GCR_DB_PC_CYCLE
#define GCR_DB_PC_CYCLE 0x0180


// GCR Global Debug Qualifier and count registers
#define GCR_DB_PC_QUAL0 0x0190
#define GCR_DB_PC_CNT0 0x0198
#define GCR_DB_PC_QUAL1 0x01a0
#define GCR_DB_PC_CNT1 0x01a8

// GCR Global Debug Trace word access registers
#define GCR_DB_TCBTW_LO 0x0200
#define GCR_DB_TCBTW_HI 0x0208

// GIC Offsets within the Global interrupt controller

#define GIC_SH_CONFIG 0x0000
#define COUNTSTOP 28
#define COUNTSTOP_S 1
#define COUNTBITS 24
#define COUNTBITS_S 4
#define NUMINTERRUPTS 16
#define NUMINTERRUPTS_S 8
#define PVPES 0
#define PVPES_S 9

#define GIC_SH_CounterLo 0x0010
#define GIC_SH_CounterHi 0x0014

#define GIC_RevisionID 0x0020

#define GIC_SH_POL31_0 0x0100
#define GIC_SH_POL63_32 0x0104
#define GIC_SH_POL95_64 0x0108
#define GIC_SH_POL127_96 0x010c
#define GIC_SH_POL159_128 0x0110
#define GIC_SH_POL191_160 0x0114
#define GIC_SH_POL223_192 0x0118
#define GIC_SH_POL255_224 0x011c

#define GIC_SH_TRIG31_0 0x0180
#define GIC_SH_TRIG63_32 0x0184
#define GIC_SH_TRIG95_64 0x0188
#define GIC_SH_TRIG127_96 0x018c
#define GIC_SH_TRIG159_128 0x0190
#define GIC_SH_TRIG191_160 0x0194
#define GIC_SH_TRIG223_192 0x0198
#define GIC_SH_TRIG255_224 0x019c

#define GIC_SH_DUAL31_0 0x0200
#define GIC_SH_DUAL63_32 0x0204
#define GIC_SH_DUAL95_64 0x0208
#define GIC_SH_DUAL127_96 0x020c
#define GIC_SH_DUAL159_128 0x0210
#define GIC_SH_DUAL159_128 0x0210
#define GIC_SH_DUAL191_160 0x0214
#define GIC_SH_DUAL223_192 0x0218
#define GIC_SH_DUAL255_224 0x021c

#define GIC_SH_WEDGE 0x0280

#define GIC_SH_RMASK31_0 0x0300
#define GIC_SH_RMASK63_32 0x0304
#define GIC_SH_RMASK95_64 0x0308
#define GIC_SH_RMASK127_96 0x030c
#define GIC_SH_RMASK159_128 0x0310
#define GIC_SH_RMASK191_160 0x0314
#define GIC_SH_RMASK223_192 0x0318
#define GIC_SH_RMASK255_224 0x031c

#define GIC_SH_SMASK31_00 0x0380
#define GIC_SH_SMASK63_32 0x0384
#define GIC_SH_SMASK95_64 0x0388
#define GIC_SH_SMASK127_96 0x038c
#define GIC_SH_SMASK159_128 0x0390
#define GIC_SH_SMASK191_160 0x0394
#define GIC_SH_SMASK223_192 0x0398
#define GIC_SH_SMASK255_224 0x039c

#define GIC_SH_MASK31_00 0x0400
#define GIC_SH_MASK63_32 0x0404
#define GIC_SH_MASK95_64 0x0408
#define GIC_SH_MASK127_96 0x040c
#define GIC_SH_MASK159_128 0x0410
#define GIC_SH_MASK191_160 0x0414
#define GIC_SH_MASK223_192 0x0418
#define GIC_SH_MASK255_224 0x041c

#define GIC_SH_PEND31_00 0x0480
#define GIC_SH_PEND63_32 0x0484
#define GIC_SH_PEND95_64 0x0488
#define GIC_SH_PEND127_96 0x048c
#define GIC_SH_PEND159_128 0x0490
#define GIC_SH_PEND191_160 0x0494
#define GIC_SH_PEND223_192 0x0498
#define GIC_SH_PEND255_224 0x049c

// Global MAP to Pin GIC_SH_MAP_PIN + (4 x interrupt_source)
#define GIC_SH_MAP_PIN 0x0500

#define GIC_SH_MAP_SPACER 0x20
// Map source to VPEs 31 - 0 GIC_SH_MAP0_VPE31_0 + (0x20 x interrupt source)
#define GIC_SH_MAP0_VPE31_0 0x2000
// Map source to VPEs 63 - 32 GIC_SH_MAP0_VPE63_32 + (0x20 x interrupt source)
#define GIC_SH_MAP0_VPE63_32  0x2004

// Map source to core 31 - 0 GIC_SH_MAP0_CORE31_0 + (0x20 x interrupt source)
#define GIC_SH_MAP0_CORE31_0 0x2000
// Map source to core 63 - 32 GIC_SH_MAP0_CORE63_32 + (0x20 x interrupt source)
#define GIC_SH_MAP0_CORE63_32 0x2004

#define GIC_VB_DINT_SEND 0x6000

// GIC VPE Local offsets (note the VPEL)
#define GIC_VPE_LOCAL_SECTION_OFFSET 0x8000

#define GIC_VPEL_CTL 0x0000
#define GIC_VPEL_PEND 0x0004
#define GIC_VPEL_MASK 0x0008
#define GIC_VPEL_RMASK 0x000c
#define GIC_VPEL_SMASK 0x0010
#define GIC_VPEL_WD_MAP 0x0040
#define GIC_VPEL_COMPARE_MAP 0x0044
#define GIC_VPEL_TIMER_MAP (0x0048 )
#define GIC_VPEL_FDC_MAP 0x004c
#define GIC_VPEL_PERFCTR_MAP 0x0050
#define GIC_VPEL_SWInt0_MAP 0x0054
#define GIC_VPEL_SWInt1_MAP 0x0058
#define GIC_VPEL_OTHER_ADDR 0x0080
#define GIC_VPEL_IDENT 0x0088
#define GIC_VPEL_WD_CONFIG0 0x0090
#define GIC_VPEL_WD_COUNT0 0x0094
#define GIC_VPEL_WD_INITIAL0 0x0098
#define GIC_VPEL_CompareLo 0x00A0
#define GIC_VPEL_CompareHi 0x00A4

// NOTE: EIC Shadow set GIC_VPEL_EICSS + (4 x interrupt number)
#define GIC_VPEL_EICSS 0x0100
#define GIC_VL_DINT_PART 0x3000
#define GIC_VL_BRK_GROUP 0x3080

// GIC VPE Other offsets (note the VPEO)
#define GIC_VPE_OTHER_SECTION_OFFSET 0xc000

#define GIC_VPEO_CTL 0x0000
#define GIC_VPEO_PEND 0x0004
#define GIC_VPEO_MASK 0x0008
#define GIC_VPEO_RMASK 0x000c
#define GIC_VPEO_SMASK 0x0010
#define GIC_VPEO_WD_MAP 0x0040
#define GIC_VPEO_COMPARE_MAP 0x0044
#define GIC_VPEO_TIMER_MAP 0x0048
#define GIC_VPEO_FDC_MAP 0x004c
#define GIC_VPEO_PERFCTR_MAP 0x0050
#define GIC_VPEO_SWInt0_MAP 0x0054
#define GIC_VPEO_SWInt1_MAP 0x0058
#define GIC_VPEO_OTHER_ADDR 0x0080
#define GIC_VPEO_IDENT 0x0088
#define GIC_VPEO_WD_CONFIG0 0x0090
#define GIC_VPEO_WD_COUNT0 0x0094
#define GIC_VPEO_WD_INITIAL0 0x0098
#define GIC_VPEO_CompareLo 0x00A0
#define GIC_VPEO_CompareHi 0x00A4

// NOTE: EIC Shadow set GIC_VPEO_EICSS + (4 x interrupt number)
#define GIC_VPEO_EICSS 0x0100
#define GIC_VO_DINT_PART 0x3000
#define GIC_VO_BRK_GROUP 0x3080

// GIC CORE Local offsets (note the COREL)
#define GIC_CORE_LOCAL_SECTION_OFFSET 0x8000

#define GIC_COREL_CTL 0x0000
#define GIC_COREL_PEND 0x0004
#define GIC_COREL_MASK 0x0008
#define GIC_COREL_RMASK 0x000c
#define GIC_COREL_SMASK 0x0010
#define GIC_COREL_WD_MAP 0x0040
#define GIC_COREL_COMPARE_MAP 0x0044
#define GIC_COREL_TIMER_MAP 0x0048
#define GIC_COREL_FDC_MAP 0x004c
#define GIC_COREL_PERFCTR_MAP 0x0050
#define GIC_COREL_SWInt0_MAP 0x0054
#define GIC_COREL_SWInt1_MAP 0x0058
#define GIC_COREL_OTHER_ADDR 0x0080
#define GIC_COREL_IDENT 0x0088
#define GIC_COREL_WD_CONFIG0 0x0090
#define GIC_COREL_WD_COUNT0 0x0094
#define GIC_COREL_WD_INITIAL0 0x0098
#define GIC_COREL_CompareLo 0x00A0
#define GIC_COREL_CompareHi 0x00A4
// NOTE: EIC Shadow set GIC_COREL_EICSS + (4 x interrupt number)
#define GIC_COREL_EICSS 0x0100
#define GIC_COREL_DINT_PART 0x3000
#define GIC_COREL_BRK_GROUP 0x3080

// GIC CORE Other offsets (note the COREO)
#define GIC_CORE_OTHER_SECTION_OFFSET 0xc000
#define GIC_COREO_CTL 0x0000
#define GIC_COREO_PEND 0x0004
#define GIC_COREO_MASK 0x0008
#define GIC_COREO_RMASK 0x000c
#define GIC_COREO_SMASK 0x0010
#define GIC_COREO_WD_MAP 0x0040
#define GIC_COREO_COMPARE_MAP 0x0044
#define GIC_COREO_TIMER_MAP 0x0048
#define GIC_COREO_FDC_MAP 0x004c
#define GIC_COREO_PERFCTR_MAP 0x0050
#define GIC_COREO_SWInt0_MAP 0x0054
#define GIC_COREO_SWInt1_MAP 0x0058
#define GIC_COREO_OTHER_ADDR 0x0080
#define GIC_COREO_IDENT 0x0088
#define GIC_COREO_WD_CONFIG0 0x0090
#define GIC_COREO_WD_COUNT0 0x0094
#define GIC_COREO_WD_INITIAL0 0x0098
#define GIC_COREO_CompareLo 0x00A0
#define GIC_COREO_CompareHi 0x00A4
// NOTE: EIC Shadow set GIC_COREO_EICSS + (4 x interrupt number)
#define GIC_COREO_EICSS 0x0100
#define GIC_COREO_DINT_PART 0x3000
#define GIC_COREO_BRK_GROUP 0x3080


// Bit fields for Local Interrupt Control Register (GIC_COREi_CTL) or for MT (GIC_VPEi_CTL) or
#define FDC_ROUTABLE 4
#define FDC_ROUTABLE_S 1
#define SWINT_ROUTABLE 3
#define SWINT_ROUTABLE_S 1
#define PERFCOUNT_ROUTABLE 2
#define PERFCOUNT_ROUTABLE_S 1
#define TIMER_ROUTABLE 1
#define TIMER_ROUTABLE_S 1
#define EIC_MODE 0
#define EIC_MODE_S 1

// Bit  fields for Local Interrupt Pending Registers (GIC_COREi_PEND) or for MT (GIC_VPEi_PEND)
#define FDC_PEND 6
#define FDC_PEND_S 1
#define SWINT1_PEND 5
#define SWINT1_PEND_S 1
#define SWINT0_PEND 4
#define SWINT0_PEND_S 1
#define PERFCOUNT_PEND 3
#define PERFCOUNT_PEND_S 1
#define TIMER_PEND 2
#define TIMER_PEND_S 1
#define COMPARE_PEND 1
#define COMPARE_PEND_S 1
#define WD_PEND 0
#define WD_PEND_S 1

// Bit fields for Local Interrupt Mask Registers (GIC_COREi_MASK) or for MT (GIC_VPEi_MASK)
#define FDC_MASK 6
#define FDC_MASK_S 1
#define SWINT1_MASK 5
#define SWINT1_MASK_S 1
#define SWINT0_MASK 4
#define SWINT0_MASK_S 1
#define PERFCOUNT_MASK 3
#define PERFCOUNT_MASK_S 1
#define TIMER_MASK 2
#define TIMER_MASK_S 1
#define COMPARE_MASK 1
#define COMPARE_MASK_S 1
#define WD_MASK 0
#define WD_MASK_S 1

// Bit fields for Local Interrupt Reset Mask Registers (GIC_COREi_RMASK) or for MT (GIC_VPEi_RMASK)
#define FDC_MASK_RESET 6
#define FDC_MASK_RESET_S 1
#define SWINT1_MASK_RESET 5
#define SWINT1_MASK_RESET_S 1
#define SWINT0_MASK_RESET 4
#define SWINT0_MASK_RESET_S 1
#define PERFCOUNT_MASK_RESET 3
#define PERFCOUNT_MASK_RESET_S 1
#define TIMER_MASK_RESET 2
#define TIMER_MASK_RESET_S 1
#define COMPARE_MASK_RESET 1
#define COMPARE_MASK_RESET_S 1
#define WD_MASK_RESET 0
#define WD_MASK_RESET_S 1

// Bit fields for Local Interrupt Set Mask Registers (GIC_COREi_SMASK) or for MT (GIC_VPEi_SMASK)
#define FDC_MASK_SET 6
#define FDC_MASK_SET_S 1
#define SWINT1_MASK_SET 5
#define SWINT1_MASK_SET_S 1
#define SWINT0_MASK_SET 4
#define SWINT0_MASK_SET_S 1
#define PERFCOUNT_MASK_SET 3
#define PERFCOUNT_MASK_SET_S 1
#define TIMER_MASK_SET 2
#define TIMER_MASK_SET_S 1
#define COMPARE_MASK_SET 1
#define COMPARE_MASK_SET_S 1
#define WD_MASK_SET 0
#define WD_MASK_SET_S 1

// Bit fields for CORE-Other or for MT VPE-Other Addressing Register
#define VPENum 0
#define VPENum_S 16
#define CORENum 0
#define CORENum_S 16

// Bit fields for Core-Local Identification Register (GIC_COREi_IDENT) or for MT (GIC_VPEi_IDENT)
#define VPENumIDENT 0
#define VPENumIDENT_S 32
#define CORENumIDENT 0
#define CORENumIDENT_S 32

// Bit fields for Local EIC Shadow Set Registers (GIC_COREi_EICSSj) or for MT (GIC_VPEi_EICSSj)
#define EIC_SS 0
#define EIC_SS_S 4

// Bit fields for Local WatchDog/Compare/PerfCount/SWIntx Map to Pin Registers
#define MAP_TO_PIN 31
#define MAP_TO_PIN_S 1
#define MAP_TO_NMI 30
#define MAP_TO_NMI_S 1
#define MAP_TO_YQ 29
#define MAP_TO_YQ_S 1
#define MAP 0
#define MAP_S 6

// Bit fields for Watchdog Timer Config Register (GIC_COREi_WD_CONFIGk) or for MT (GIC_VPEi_WD_CONFIGk)
#define WDRESET 7
#define WDRESET_S 1
#define WDINTR 6
#define WDINTR_S 1
#define WAITMODE_CNTRL 5
#define WAITMODE_CNTRL_S 1
#define DEBUGMODE_CNTRL 5
#define DEBUGMODE_CNTRL_S 1
#define TYPE 1
#define TYPE_S 3
// TYPE Filed encoding:
#define WD_One_Trip_Mode 0
#define WD_Second_Countdown_Mode 1
#define PIT_Mode 2

// Bit fields for Local DINT Group Participate Register (GIC_Cx_DINT_PART) or for MT (GIC_Vx_DINT_PART)
#define DINT_Group_Particpate 0
#define DINT_Group_Particpate_S 1

// GIC, GIC User Mode Visible Section Offsets
#define USER_MODE_VISIBLE_SECTION_OFFSET 0x10000
#define GIC_SH_COUNTERLO 0x0000
#define GIC_SH_COUNTERHI 0x0004

// Cluster Power Controller Global Section
// CPC Block CPC_ACCESS_REG, fields and sizes
#define CPC_ACCESS_REG 0x000
#define CM_ACCESS_EN 0
#define CM_ACCESS_EN_S 8

// CPC Block CPC_SEQDEL_REG, fields and sizes
#define CPC_SEQDEL_REG 0x008
#define MICROSTEP 0
#define MICROSTEP_S 10

// CPC Block CPC_RAIL_REG, fields and sizes
#define CPC_RAIL_REG 0x010
#define RAILDELAY 0
#define RAILDELAY_S 10

// CPC Block CPC_RESETLEN_REG, fields and sizes
#define CPC_RESETLEN_REG 0x018
#define RESETLEN 0
#define RESETLEN_S 10

// CPC Block CPC_REVISION_REG, fields and sizes
#define CPC_REVISION_REG 0x020
#define MAJOR_REV_CPC 8
#define MAJOR_REV_CPC_S 8
#define MINOR_REV_CPC 0
#define MINOR_REV_CPC_S 8

// Cluster Power Controller Local and Other section
#define CPS_CORE_LOCAL_CONTROL_BLOCK 0x2000
#define CPS_CORE_OTHER_CONTROL_BLOCK 0x4000

// CPC Local and Other CPC_CMD_REG, command encoding
#define CPC_CMDL_REG 0x000
#define CPC_CMDO_REG 0x000
#define CLOCK_OFF 1
#define PWR_DOWN 2
#define PWR_UP 3
#define CPC_RESET 4

// CPC Local and Other CPC_STAT_CONF, fields, sizes and encodings
#define CPC_STATL_CONF_REG 0x008
#define CPC_STATO_CONF_REG 0x008
#define PWRUP_EVENT 23
#define PWRUP_EVENT_S 1
#define SEQ_STATE 19
#define SEQ_STATE_S 4

// sequencer state encodings
#define PWR_DOWN_STATE 0x0
#define VDD_OK_STATE 0x1
#define UP_DELAY_STATE 0x2
#define UCLK_OFF_STATE 0x3
#define CPC_RESET_STATE 0x4
#define CPC_RESET_DLY_STATE 0x5
#define NON_COHERENT_EXECUTION_STATE 0x6
#define COHERENT_EXECUTION_STATE 0x7
#define ISOLATE_STATE 0x8
#define CLR_BUS_STATE 0x8
#define DCLK_OFF_STATE 0xA

#define CLKGAT_IMPL 17
#define CLKGAT_IMPL_S 1
#define PWRDN_IMPL 16
#define PWRDN_IMPL_S 1
#define EJTAG_PROBE 15
#define EJTAG_PROBE_S 1
#define PWUP_POLICY 8
#define PWUP_POLICY_S 2
// Power up state encodings
#define POLICY_PWR_DOWN 0
#define POLICY_GO_CLOCK_OFF 1
#define PLOICY_PWR_UP 2

#define IO_TRFFC_EN 4
#define IO_TRFFC_EN_S 1
#define CPC_CMD_STATE 0
#define CPC_CMD_STATE_S 4

// CPC Local and Other Addressing Register CPC_OTHER_REG, field and size
#define CPC_OTHERL_REG 0x010
#define CPC_OTHERO_REG 0x010
#define CPC_CORENUM 16
#define CPC_CORENUM_S 8


#endif /* CPS_H_ */
