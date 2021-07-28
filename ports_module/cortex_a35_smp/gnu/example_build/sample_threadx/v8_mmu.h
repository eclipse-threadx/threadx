//
// Defines for v8 Memory Model
//
// Copyright (c) 2012-2016 Arm Limited (or its affiliates). All rights reserved.
// Use, modification and redistribution of this file is subject to your possession of a
// valid End User License Agreement for the Arm Product of which these examples are part of 
// and your compliance with all applicable terms and conditions of such licence agreement.
//

#ifndef V8_MMU_H
#define V8_MMU_H

//
// Translation Control Register fields
//
// RGN field encodings
//
#define TCR_RGN_NC   0b00
#define TCR_RGN_WBWA 0b01
#define TCR_RGN_WT   0b10
#define TCR_RGN_WBRA 0b11

//
// Shareability encodings
//
#define TCR_SHARE_NONE  0b00
#define TCR_SHARE_OUTER 0b10
#define TCR_SHARE_INNER 0b11

//
// Granule size encodings
//
#define TCR_GRANULE_4K  0b00
#define TCR_GRANULE_64K 0b01
#define TCR_GRANULE_16K 0b10

//
// Physical Address sizes
//
#define TCR_SIZE_4G   0b000
#define TCR_SIZE_64G  0b001
#define TCR_SIZE_1T   0b010
#define TCR_SIZE_4T   0b011
#define TCR_SIZE_16T  0b100
#define TCR_SIZE_256T 0b101

//
// Translation Control Register fields
//
#define TCR_EL1_T0SZ_SHIFT   0
#define TCR_EL1_EPD0  (1 << 7)
#define TCR_EL1_IRGN0_SHIFT  8
#define TCR_EL1_ORGN0_SHIFT 10
#define TCR_EL1_SH0_SHIFT   12
#define TCR_EL1_TG0_SHIFT   14

#define TCR_EL1_T1SZ_SHIFT  16
#define TCR_EL1_A1   (1 << 22)
#define TCR_EL1_EPD1 (1 << 23)
#define TCR_EL1_IRGN1_SHIFT 24
#define TCR_EL1_ORGN1_SHIFT 26
#define TCR_EL1_SH1_SHIFT   28
#define TCR_EL1_TG1_SHIFT   30
#define TCR_EL1_IPS_SHIFT   32
#define TCR_EL1_AS   (1 << 36)
#define TCR_EL1_TBI0 (1 << 37)
#define TCR_EL1_TBI1 (1 << 38)

//
// Stage 1 Translation Table descriptor fields
//
#define TT_S1_ATTR_FAULT (0b00 << 0)
#define TT_S1_ATTR_BLOCK (0b01 << 0) // Level 1/2
#define TT_S1_ATTR_TABLE (0b11 << 0) // Level 0/1/2
#define TT_S1_ATTR_PAGE  (0b11 << 0) // Level 3

#define TT_S1_ATTR_MATTR_LSB 2

#define TT_S1_ATTR_NS (1 << 5)

#define TT_S1_ATTR_AP_RW_PL1 (0b00 << 6)
#define TT_S1_ATTR_AP_RW_ANY (0b01 << 6)
#define TT_S1_ATTR_AP_RO_PL1 (0b10 << 6)
#define TT_S1_ATTR_AP_RO_ANY (0b11 << 6)

#define TT_S1_ATTR_SH_NONE   (0b00 << 8)
#define TT_S1_ATTR_SH_OUTER  (0b10 << 8)
#define TT_S1_ATTR_SH_INNER  (0b11 << 8)

#define TT_S1_ATTR_AF (1 << 10)
#define TT_S1_ATTR_nG (1 << 11)

#define TT_S1_ATTR_CONTIG (1 << 52)
#define TT_S1_ATTR_PXN    (1 << 53)
#define TT_S1_ATTR_UXN    (1 << 54)

#define TT_S1_MAIR_DEV_nGnRnE 0b00000000
#define TT_S1_MAIR_DEV_nGnRE  0b00000100
#define TT_S1_MAIR_DEV_nGRE   0b00001000
#define TT_S1_MAIR_DEV_GRE    0b00001100

//
// Inner and Outer Normal memory attributes use the same bit patterns
// Outer attributes just need to be shifted up
//
#define TT_S1_MAIR_OUTER_SHIFT 4

#define TT_S1_MAIR_WT_TRANS_RA  0b0010

#define TT_S1_MAIR_WB_TRANS_RA  0b0110
#define TT_S1_MAIR_WB_TRANS_RWA 0b0111

#define TT_S1_MAIR_WT_RA  0b1010

#define TT_S1_MAIR_WB_RA  0b1110
#define TT_S1_MAIR_WB_RWA 0b1111

#endif // V8_MMU_H
