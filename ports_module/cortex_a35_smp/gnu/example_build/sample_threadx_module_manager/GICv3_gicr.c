/*
 * GICv3_gicr.c - generic driver code for GICv3 redistributor
 *
 * Copyright (c) 2014-2018 Arm Limited (or its affiliates). All rights reserved.
 * Use, modification and redistribution of this file is subject to your possession of a
 * valid End User License Agreement for the Arm Product of which these examples are part of
 * and your compliance with all applicable terms and conditions of such licence agreement.
 */
#include "GICv3.h"

/*
 * physical LPI Redistributor register map
 */
typedef struct
{
        volatile uint32_t GICR_CTLR;             // +0x0000 - RW - Redistributor Control Register
  const volatile uint32_t GICR_IIDR;             // +0x0004 - RO - Implementer Identification Register
  const volatile uint32_t GICR_TYPER[2];         // +0x0008 - RO - Redistributor Type Register
        volatile uint32_t GICR_STATUSR;          // +0x0010 - RW - Error Reporting Status Register, optional
        volatile uint32_t GICR_WAKER;            // +0x0014 - RW - Redistributor Wake Register
  const volatile uint32_t padding1[2];           // +0x0018 - RESERVED
#ifndef USE_GIC600
        volatile uint32_t IMPDEF1[8];            // +0x0020 - ?? - IMPLEMENTATION DEFINED
#else
        volatile uint32_t GICR_FCTLR;            // +0x0020 - RW - Function Control Register
        volatile uint32_t GICR_PWRR;             // +0x0024 - RW - Power Management Control Register
        volatile uint32_t GICR_CLASS;            // +0x0028 - RW - Class Register
  const volatile uint32_t padding2[5];           // +0x002C - RESERVED
#endif
        volatile uint64_t GICR_SETLPIR;          // +0x0040 - WO - Set LPI Pending Register
        volatile uint64_t GICR_CLRLPIR;          // +0x0048 - WO - Clear LPI Pending Register
  const volatile uint32_t padding3[8];           // +0x0050 - RESERVED
        volatile uint64_t GICR_PROPBASER;        // +0x0070 - RW - Redistributor Properties Base Address Register
        volatile uint64_t GICR_PENDBASER;        // +0x0078 - RW - Redistributor LPI Pending Table Base Address Register
  const volatile uint32_t padding4[8];           // +0x0080 - RESERVED
        volatile uint64_t GICR_INVLPIR;          // +0x00A0 - WO - Redistributor Invalidate LPI Register
  const volatile uint32_t padding5[2];           // +0x00A8 - RESERVED
        volatile uint64_t GICR_INVALLR;          // +0x00B0 - WO - Redistributor Invalidate All Register
  const volatile uint32_t padding6[2];           // +0x00B8 - RESERVED
        volatile uint64_t GICR_SYNCR;            // +0x00C0 - RO - Redistributor Synchronize Register
  const volatile uint32_t padding7[2];           // +0x00C8 - RESERVED
  const volatile uint32_t padding8[12];          // +0x00D0 - RESERVED
        volatile uint64_t IMPDEF2;               // +0x0100 - WO - IMPLEMENTATION DEFINED
  const volatile uint32_t padding9[2];           // +0x0108 - RESERVED
        volatile uint64_t IMPDEF3;               // +0x0110 - WO - IMPLEMENTATION DEFINED
  const volatile uint32_t padding10[2];          // +0x0118 - RESERVED
} GICv3_redistributor_RD;

/*
 * SGI and PPI Redistributor register map
 */
typedef struct
{
  const volatile uint32_t padding1[32];          // +0x0000 - RESERVED
        volatile uint32_t GICR_IGROUPR0;         // +0x0080 - RW - Interrupt Group Registers (Security Registers in GICv1)
  const volatile uint32_t padding2[31];          // +0x0084 - RESERVED
        volatile uint32_t GICR_ISENABLER;        // +0x0100 - RW - Interrupt Set-Enable Registers
  const volatile uint32_t padding3[31];          // +0x0104 - RESERVED
        volatile uint32_t GICR_ICENABLER;        // +0x0180 - RW - Interrupt Clear-Enable Registers
  const volatile uint32_t padding4[31];          // +0x0184 - RESERVED
        volatile uint32_t GICR_ISPENDR;          // +0x0200 - RW - Interrupt Set-Pending Registers
  const volatile uint32_t padding5[31];          // +0x0204 - RESERVED
        volatile uint32_t GICR_ICPENDR;          // +0x0280 - RW - Interrupt Clear-Pending Registers
  const volatile uint32_t padding6[31];          // +0x0284 - RESERVED
        volatile uint32_t GICR_ISACTIVER;        // +0x0300 - RW - Interrupt Set-Active Register
  const volatile uint32_t padding7[31];          // +0x0304 - RESERVED
        volatile uint32_t GICR_ICACTIVER;        // +0x0380 - RW - Interrupt Clear-Active Register
  const volatile uint32_t padding8[31];          // +0x0184 - RESERVED
        volatile uint8_t  GICR_IPRIORITYR[32];   // +0x0400 - RW - Interrupt Priority Registers
  const volatile uint32_t padding9[504];         // +0x0420 - RESERVED
        volatile uint32_t GICR_ICnoFGR[2];       // +0x0C00 - RW - Interrupt Configuration Registers
  const volatile uint32_t padding10[62];         // +0x0C08 - RESERVED
        volatile uint32_t GICR_IGRPMODR0;        // +0x0D00 - RW - ????
  const volatile uint32_t padding11[63];         // +0x0D04 - RESERVED
        volatile uint32_t GICR_NSACR;            // +0x0E00 - RW - Non-Secure Access Control Register
} GICv3_redistributor_SGI;

/*
 * We have a multiplicity of GIC Redistributors; on the GIC-AEM and
 * GIC-500 they are arranged as one 128KB region per redistributor: one
 * 64KB page of GICR LPI registers, and one 64KB page of GICR Private
 * Int registers
 */
typedef struct
{
    union
    {
        GICv3_redistributor_RD RD_base;
        uint8_t padding[64 * 1024];
    } RDblock;

    union
    {
        GICv3_redistributor_SGI SGI_base;
        uint8_t padding[64 * 1024];
    } SGIblock;
} GICv3_GICR;

/*
 * use the scatter file to place GIC Redistributor base address
 *
 * although this code doesn't know how many Redistributor banks
 * a particular system will have, we declare gicrbase as an array
 * to avoid unwanted compiler optimisations when calculating the
 * base of a particular Redistributor bank
 */
static const GICv3_GICR gicrbase[2] __attribute__((section (".gicr")));

/**********************************************************************/

/*
 * utility functions to calculate base of a particular
 * Redistributor bank
 */

static inline GICv3_redistributor_RD *const getgicrRD(uint32_t gicr)
{
    GICv3_GICR *const arraybase = (GICv3_GICR *const)&gicrbase;

    return &((arraybase + gicr)->RDblock.RD_base);
}

static inline GICv3_redistributor_SGI *const getgicrSGI(uint32_t gicr)
{
    GICv3_GICR *arraybase = (GICv3_GICR *)(&gicrbase);

    return &(arraybase[gicr].SGIblock.SGI_base);
}

/**********************************************************************/

void WakeupGICR(uint32_t gicr)
{
    GICv3_redistributor_RD *const gicrRD = getgicrRD(gicr);
#ifdef USE_GIC600
    //Power up Re-distributor for GIC-600
    gicrRD->GICR_PWRR = 0x2;
#endif

    /*
     * step 1 - ensure GICR_WAKER.ProcessorSleep is off
     */
    gicrRD->GICR_WAKER &= ~gicrwaker_ProcessorSleep;

    /*
     * step 2 - wait for children asleep to be cleared
     */
    while ((gicrRD->GICR_WAKER & gicrwaker_ChildrenAsleep) != 0)
        continue;

    /*
     * OK, GICR is go
     */
    return;
}

void EnablePrivateInt(uint32_t gicr, uint32_t id)
{
    GICv3_redistributor_SGI *const gicrSGI = getgicrSGI(gicr);

    id &= 0x1f;

    gicrSGI->GICR_ISENABLER = 1 << id;
}

void DisablePrivateInt(uint32_t gicr, uint32_t id)
{
    GICv3_redistributor_SGI *const gicrSGI = getgicrSGI(gicr);

    id &= 0x1f;

    gicrSGI->GICR_ICENABLER = 1 << id;
}

void SetPrivateIntPriority(uint32_t gicr, uint32_t id, uint32_t priority)
{
    GICv3_redistributor_SGI *const gicrSGI = getgicrSGI(gicr);

    /*
     * GICD_IPRIORITYR has one byte-wide entry per interrupt
     */
    id &= RANGE_LIMIT(gicrSGI->GICR_IPRIORITYR);

    gicrSGI->GICR_IPRIORITYR[id] = priority;
}

uint32_t GetPrivateIntPriority(uint32_t gicr, uint32_t id)
{
    GICv3_redistributor_SGI *const gicrSGI = getgicrSGI(gicr);

    /*
     * GICD_IPRIORITYR has one byte-wide entry per interrupt
     */
    id &= RANGE_LIMIT(gicrSGI->GICR_IPRIORITYR);

    return (uint32_t)(gicrSGI->GICR_IPRIORITYR[id]);
}

void SetPrivateIntPending(uint32_t gicr, uint32_t id)
{
    GICv3_redistributor_SGI *const gicrSGI = getgicrSGI(gicr);

    /*
     * GICR_ISPENDR is one 32-bit register
     */
    id &= 0x1f;

    gicrSGI->GICR_ISPENDR = 1 << id;
}

void ClearPrivateIntPending(uint32_t gicr, uint32_t id)
{
    GICv3_redistributor_SGI *const gicrSGI = getgicrSGI(gicr);

    /*
     * GICR_ICPENDR is one 32-bit register
     */
    id &= 0x1f;

    gicrSGI->GICR_ICPENDR = 1 << id;
}

uint32_t GetPrivateIntPending(uint32_t gicr, uint32_t id)
{
    GICv3_redistributor_SGI *const gicrSGI = getgicrSGI(gicr);

    /*
     * GICR_ISPENDR is one 32-bit register
     */
    id &= 0x1f;

    return (gicrSGI->GICR_ISPENDR >> id) & 0x01;
}

void SetPrivateIntSecurity(uint32_t gicr, uint32_t id, GICIGROUPRBits_t group)
{
    GICv3_redistributor_SGI *const gicrSGI = getgicrSGI(gicr);
    uint32_t groupmod;

    /*
     * GICR_IGROUPR0 is one 32-bit register
     */
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
        gicrSGI->GICR_IGROUPR0 |= 1 << id;
    else
        gicrSGI->GICR_IGROUPR0 &= ~(1 << id);

    /*
     * now deal with groupmod
     */
    if (groupmod)
        gicrSGI->GICR_IGRPMODR0 |= 1 << id;
    else
        gicrSGI->GICR_IGRPMODR0 &= ~(1 << id);
}

void SetPrivateIntSecurityBlock(uint32_t gicr, GICIGROUPRBits_t group)
{
    GICv3_redistributor_SGI *const gicrSGI = getgicrSGI(gicr);
    const uint32_t nbits = (sizeof group * 8) - 1;
    uint32_t groupmod;

    /*
     * get each bit of group config duplicated over all 32 bits
     */
    groupmod = (uint32_t)(((int32_t)group << (nbits - 1)) >> 31);
    group = (uint32_t)(((int32_t)group << nbits) >> 31);

    /*
     * set the security state for this block of SPIs
     */
    gicrSGI->GICR_IGROUPR0 = group;
    gicrSGI->GICR_IGRPMODR0 = groupmod;
}

/* EOF GICv3_gicr.c */
