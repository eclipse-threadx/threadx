/*
 * GICv3_gicd.c - generic driver code for GICv3 distributor
 *
 * Copyright (c) 2014-2017 Arm Limited (or its affiliates). All rights reserved.
 * Use, modification and redistribution of this file is subject to your possession of a
 * valid End User License Agreement for the Arm Product of which these examples are part of 
 * and your compliance with all applicable terms and conditions of such licence agreement.
 */
#include <stdint.h>

#include "GICv3.h"

typedef struct
{
        volatile uint32_t GICD_CTLR;              // +0x0000
  const volatile uint32_t GICD_TYPER;             // +0x0004
  const volatile uint32_t GICD_IIDR;              // +0x0008

  const volatile uint32_t padding0;               // +0x000c

        volatile uint32_t GICD_STATUSR;           // +0x0010

  const volatile uint32_t padding1[3];            // +0x0014

        volatile uint32_t IMP_DEF[8];             // +0x0020

        volatile uint32_t GICD_SETSPI_NSR;        // +0x0040
  const volatile uint32_t padding2;               // +0x0044
        volatile uint32_t GICD_CLRSPI_NSR;        // +0x0048
  const volatile uint32_t padding3;               // +0x004c
        volatile uint32_t GICD_SETSPI_SR;         // +0x0050
  const volatile uint32_t padding4;               // +0x0054
        volatile uint32_t GICD_CLRSPI_SR;         // +0x0058

  const volatile uint32_t padding5[3];            // +0x005c

        volatile uint32_t GICD_SEIR;              // +0x0068

  const volatile uint32_t padding6[5];            // +0x006c

        volatile uint32_t GICD_IGROUPR[32];       // +0x0080

        volatile uint32_t GICD_ISENABLER[32];     // +0x0100
        volatile uint32_t GICD_ICENABLER[32];     // +0x0180
        volatile uint32_t GICD_ISPENDR[32];       // +0x0200
        volatile uint32_t GICD_ICPENDR[32];       // +0x0280
        volatile uint32_t GICD_ISACTIVER[32];     // +0x0300
        volatile uint32_t GICD_ICACTIVER[32];     // +0x0380

        volatile uint8_t  GICD_IPRIORITYR[1024];  // +0x0400
        volatile uint8_t  GICD_ITARGETSR[1024];   // +0x0800
        volatile uint32_t GICD_ICFGR[64];         // +0x0c00
        volatile uint32_t GICD_IGRPMODR[32];      // +0x0d00
  const volatile uint32_t padding7[32];           // +0x0d80
        volatile uint32_t GICD_NSACR[64];         // +0x0e00

        volatile uint32_t GICD_SGIR;              // +0x0f00

  const volatile uint32_t padding8[3];            // +0x0f04

        volatile uint32_t GICD_CPENDSGIR[4];      // +0x0f10
        volatile uint32_t GICD_SPENDSGIR[4];      // +0x0f20

  const volatile uint32_t padding9[52];           // +0x0f30
  const volatile uint32_t padding10[5120];        // +0x1000

        volatile uint64_t GICD_IROUTER[1024];     // +0x6000
} GICv3_distributor;

/*
 * use the scatter file to place GICD
 */
static GICv3_distributor __attribute__((section(".bss.distributor"))) gicd;

void ConfigGICD(GICDCTLRFlags_t flags)
{
    gicd.GICD_CTLR = flags;
}

void EnableGICD(GICDCTLRFlags_t flags)
{
    gicd.GICD_CTLR |= flags;
}

void DisableGICD(GICDCTLRFlags_t flags)
{
    gicd.GICD_CTLR &= ~flags;
}

void SyncAREinGICD(GICDCTLRFlags_t flags, uint32_t dosync)
{
    if (dosync)
    {
	const uint32_t tmask = gicdctlr_ARE_S | gicdctlr_ARE_NS;
	const uint32_t tval = flags & tmask;

	while ((gicd.GICD_CTLR & tmask) != tval)
	    continue;
    }
    else
	gicd.GICD_CTLR = flags;
}

void EnableSPI(uint32_t id)
{
    uint32_t bank;

    /*
     * GICD_ISENABLER has 32 interrupts per register
     */
    bank = (id >> 5) & RANGE_LIMIT(gicd.GICD_ISENABLER);
    id &= 32 - 1;

    gicd.GICD_ISENABLER[bank] = 1 << id;

    return;
}

void DisableSPI(uint32_t id)
{
    uint32_t bank;

    /*
     * GICD_ISENABLER has 32 interrupts per register
     */
    bank = (id >> 5) & RANGE_LIMIT(gicd.GICD_ICENABLER);
    id &= 32 - 1;

    gicd.GICD_ICENABLER[bank] = 1 << id;

    return;
}

void SetSPIPriority(uint32_t id, uint32_t priority)
{
    uint32_t bank;

    /*
     * GICD_IPRIORITYR has one byte-wide entry per interrupt
     */
    bank = id & RANGE_LIMIT(gicd.GICD_IPRIORITYR);

    gicd.GICD_IPRIORITYR[bank] = priority;
}

uint32_t GetSPIPriority(uint32_t id)
{
    uint32_t bank;

    /*
     * GICD_IPRIORITYR has one byte-wide entry per interrupt
     */
    bank = id & RANGE_LIMIT(gicd.GICD_IPRIORITYR);

    return (uint32_t)(gicd.GICD_IPRIORITYR[bank]);
}

void SetSPIRoute(uint32_t id, uint64_t affinity, GICDIROUTERBits_t mode)
{
    uint32_t bank;

    /*
     * GICD_IROUTER has one doubleword-wide entry per interrupt
     */
    bank = id & RANGE_LIMIT(gicd.GICD_IROUTER);

    gicd.GICD_IROUTER[bank] = affinity | (uint64_t)mode;
}

uint64_t GetSPIRoute(uint32_t id)
{
    uint32_t bank;

    /*
     * GICD_IROUTER has one doubleword-wide entry per interrupt
     */
    bank = id & RANGE_LIMIT(gicd.GICD_IROUTER);

    return gicd.GICD_IROUTER[bank];
}

void SetSPITarget(uint32_t id, uint32_t target)
{
    uint32_t bank;

    /*
     * GICD_ITARGETSR has one byte-wide entry per interrupt
     */
    bank = id & RANGE_LIMIT(gicd.GICD_ITARGETSR);

    gicd.GICD_ITARGETSR[bank] = target;
}

uint32_t GetSPITarget(uint32_t id)
{
    uint32_t bank;

    /*
     * GICD_ITARGETSR has one byte-wide entry per interrupt
     */
    /*
     * GICD_ITARGETSR has 4 interrupts per register, i.e. 8-bits of
     * target bitmap per register
     */
    bank = id & RANGE_LIMIT(gicd.GICD_ITARGETSR);

    return (uint32_t)(gicd.GICD_ITARGETSR[bank]);
}

void ConfigureSPI(uint32_t id, GICDICFGRBits_t config)
{
    uint32_t bank, tmp;

    /*
     * GICD_ICFGR has 16 interrupts per register, i.e. 2-bits of
     * configuration per register
     */
    bank = (id >> 4) & RANGE_LIMIT(gicd.GICD_ICFGR);
    config &= 3;

    id = (id & 0xf) << 1;

    tmp = gicd.GICD_ICFGR[bank];
    tmp &= ~(3 << id);
    tmp |= config << id;
    gicd.GICD_ICFGR[bank] = tmp;
}

void SetSPIPending(uint32_t id)
{
    uint32_t bank;

    /*
     * GICD_ISPENDR has 32 interrupts per register
     */
    bank = (id >> 5) & RANGE_LIMIT(gicd.GICD_ISPENDR);
    id &= 0x1f;

    gicd.GICD_ISPENDR[bank] = 1 << id;
}

void ClearSPIPending(uint32_t id)
{
    uint32_t bank;

    /*
     * GICD_ICPENDR has 32 interrupts per register
     */
    bank = (id >> 5) & RANGE_LIMIT(gicd.GICD_ICPENDR);
    id &= 0x1f;

    gicd.GICD_ICPENDR[bank] = 1 << id;
}

uint32_t GetSPIPending(uint32_t id)
{
    uint32_t bank;

    /*
     * GICD_ICPENDR has 32 interrupts per register
     */
    bank = (id >> 5) & RANGE_LIMIT(gicd.GICD_ICPENDR);
    id &= 0x1f;

    return (gicd.GICD_ICPENDR[bank] >> id) & 1;
}

void SetSPISecurity(uint32_t id, GICIGROUPRBits_t group)
{
    uint32_t bank, groupmod;

    /*
     * GICD_IGROUPR has 32 interrupts per register
     */
    bank = (id >> 5) & RANGE_LIMIT(gicd.GICD_IGROUPR);
    id &= 0x1f;

    /*
     * the single group argument is split into two separate
     * registers, so filter out and remove the (new to gicv3)
     * group modifier bit
     */
    groupmod = (group >> 1) & 1;
    group &= 1;

    /*
     * either set or clear the Group bit for the interrupt as appropriate
     */
    if (group)
	gicd.GICD_IGROUPR[bank] |= 1 << id;
    else
	gicd.GICD_IGROUPR[bank] &= ~(1 << id);

    /*
     * now deal with groupmod
     */
    if (groupmod)
	gicd.GICD_IGRPMODR[bank] |= 1 << id;
    else
	gicd.GICD_IGRPMODR[bank] &= ~(1 << id);
}

void SetSPISecurityBlock(uint32_t block, GICIGROUPRBits_t group)
{
    uint32_t groupmod;
    const uint32_t nbits = (sizeof group * 8) - 1;

    /*
     * GICD_IGROUPR has 32 interrupts per register
     */
    block &= RANGE_LIMIT(gicd.GICD_IGROUPR);

    /*
     * get each bit of group config duplicated over all 32-bits in a word
     */
    groupmod = (uint32_t)(((int32_t)group << (nbits - 1)) >> 31);
    group = (uint32_t)(((int32_t)group << nbits) >> 31);

    /*
     * set the security state for this block of SPIs
     */
    gicd.GICD_IGROUPR[block] = group;
    gicd.GICD_IGRPMODR[block] = groupmod;
}

void SetSPISecurityAll(GICIGROUPRBits_t group)
{
    uint32_t block;

    /*
     * GICD_TYPER.ITLinesNumber gives (No. SPIS / 32) - 1, and we
     * want to iterate over all blocks excluding 0 (which are the
     * SGI/PPI interrupts, and not relevant here)
     */
    for (block = (gicd.GICD_TYPER & ((1 << 5) - 1)); block > 0; --block)
	SetSPISecurityBlock(block, group);
}

/* EOF GICv3_gicd.c */
