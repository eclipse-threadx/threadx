//
// Private Peripheral Map for the v8 Architecture Envelope Model
//
// Copyright (c) 2012-2017 Arm Limited (or its affiliates). All rights reserved.
// Use, modification and redistribution of this file is subject to your possession of a
// valid End User License Agreement for the Arm Product of which these examples are part of 
// and your compliance with all applicable terms and conditions of such licence agreement.
//

#ifndef PPM_AEM_H
#define PPM_AEM_H

//
// Distributor layout
//
#define GICD_CTLR      0x0000
#define GICD_TYPER     0x0004
#define GICD_IIDR      0x0008
#define GICD_IGROUP    0x0080
#define GICD_ISENABLE  0x0100
#define GICD_ICENABLE  0x0180
#define GICD_ISPEND    0x0200
#define GICD_ICPEND    0x0280
#define GICD_ISACTIVE  0x0300
#define GICD_ICACTIVE  0x0380
#define GICD_IPRIORITY 0x0400
#define GICD_ITARGETS  0x0800
#define GICD_ICFG      0x0c00
#define GICD_PPISR     0x0d00
#define GICD_SPISR     0x0d04
#define GICD_SGIR      0x0f00
#define GICD_CPENDSGI  0x0f10
#define GICD_SPENDSGI  0x0f20
#define GICD_PIDR4     0x0fd0
#define GICD_PIDR5     0x0fd4
#define GICD_PIDR6     0x0fd8
#define GICD_PIDR7     0x0fdc
#define GICD_PIDR0     0x0fe0
#define GICD_PIDR1     0x0fe4
#define GICD_PIDR2     0x0fe8
#define GICD_PIDR3     0x0fec
#define GICD_CIDR0     0x0ff0
#define GICD_CIDR1     0x0ff4
#define GICD_CIDR2     0x0ff8
#define GICD_CIDR3     0x0ffc

//
// CPU Interface layout
//
#define GICC_CTLR      0x0000
#define GICC_PMR       0x0004
#define GICC_BPR       0x0008
#define GICC_IAR       0x000c
#define GICC_EOIR      0x0010
#define GICC_RPR       0x0014
#define GICC_HPPIR     0x0018
#define GICC_ABPR      0x001c
#define GICC_AIAR      0x0020
#define GICC_AEOIR     0x0024
#define GICC_AHPPIR    0x0028
#define GICC_APR0      0x00d0
#define GICC_NSAPR0    0x00e0
#define GICC_IIDR      0x00fc
#define GICC_DIR       0x1000

#endif // PPM_AEM_H
