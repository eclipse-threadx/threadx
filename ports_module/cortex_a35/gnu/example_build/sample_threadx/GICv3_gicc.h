/*
 * GICv3_gicc.h - prototypes and inline functions for GICC system register operations
 *
 * Copyright (c) 2014-2017 Arm Limited (or its affiliates). All rights reserved.
 * Use, modification and redistribution of this file is subject to your possession of a
 * valid End User License Agreement for the Arm Product of which these examples are part of 
 * and your compliance with all applicable terms and conditions of such licence agreement.
 */
#ifndef GICV3_gicc_h
#define GICV3_gicc_h

#include "GICv3_aliases.h"

#define stringify_no_expansion(x) #x
#define stringify(x) stringify_no_expansion(x)

/**********************************************************************/

typedef enum
{
    sreSRE = (1 << 0),
    sreDFB = (1 << 1),
    sreDIB = (1 << 2),
    sreEnable = (1 << 3)
} ICC_SREBits_t;

static inline void setICC_SRE_EL1(ICC_SREBits_t mode)
{
    asm("msr  "stringify(ICC_SRE_EL1)", %0\n; isb" :: "r" ((uint64_t)mode));
}

static inline uint64_t getICC_SRE_EL1(void)
{
    uint64_t retc;

    asm("mrs  %0, "stringify(ICC_SRE_EL1)"\n" : "=r" (retc));

    return retc;
}

static inline void setICC_SRE_EL2(ICC_SREBits_t mode)
{
    asm("msr  "stringify(ICC_SRE_EL2)", %0\n; isb" :: "r" ((uint64_t)mode));
}

static inline uint64_t getICC_SRE_EL2(void)
{
    uint64_t retc;

    asm("mrs  %0, "stringify(ICC_SRE_EL2)"\n" : "=r" (retc));

    return retc;
}

static inline void setICC_SRE_EL3(ICC_SREBits_t mode)
{
    asm("msr  "stringify(ICC_SRE_EL3)", %0\n; isb" :: "r" ((uint64_t)mode));
}

static inline uint64_t getICC_SRE_EL3(void)
{
    uint64_t retc;

    asm("mrs  %0, "stringify(ICC_SRE_EL3)"\n" : "=r" (retc));

    return retc;
}

/**********************************************************************/

typedef enum
{
    igrpEnable = (1 << 0),
    igrpEnableGrp1NS = (1 << 0),
    igrpEnableGrp1S = (1 << 2)
} ICC_IGRPBits_t;

static inline void setICC_IGRPEN0_EL1(ICC_IGRPBits_t mode)
{
    asm("msr  "stringify(ICC_IGRPEN0_EL1)", %0\n; isb" :: "r" ((uint64_t)mode));
}

static inline void setICC_IGRPEN1_EL1(ICC_IGRPBits_t mode)
{
    asm("msr  "stringify(ICC_IGRPEN1_EL1)", %0\n; isb" :: "r" ((uint64_t)mode));
}

static inline void setICC_IGRPEN1_EL3(ICC_IGRPBits_t mode)
{
    asm("msr  "stringify(ICC_IGRPEN1_EL3)", %0\n; isb" :: "r" ((uint64_t)mode));
}

/**********************************************************************/

typedef enum
{
    ctlrCBPR = (1 << 0),
    ctlrCBPR_EL1S = (1 << 0),
    ctlrEOImode = (1 << 1),
    ctlrCBPR_EL1NS = (1 << 1),
    ctlrEOImode_EL3 = (1 << 2),
    ctlrEOImode_EL1S = (1 << 3),
    ctlrEOImode_EL1NS = (1 << 4),
    ctlrRM = (1 << 5),
    ctlrPMHE = (1 << 6)
} ICC_CTLRBits_t;

static inline void setICC_CTLR_EL1(ICC_CTLRBits_t mode)
{
    asm("msr  "stringify(ICC_CTLR_EL1)", %0\n; isb" :: "r" ((uint64_t)mode));
}

static inline uint64_t getICC_CTLR_EL1(void)
{
    uint64_t retc;

    asm("mrs  %0, "stringify(ICC_CTLR_EL1)"\n" : "=r" (retc));

    return retc;
}

static inline void setICC_CTLR_EL3(ICC_CTLRBits_t mode)
{
    asm("msr  "stringify(ICC_CTLR_EL3)", %0\n; isb" :: "r" ((uint64_t)mode));
}

static inline uint64_t getICC_CTLR_EL3(void)
{
    uint64_t retc;

    asm("mrs  %0, "stringify(ICC_CTLR_EL3)"\n" : "=r" (retc));

    return retc;
}

/**********************************************************************/

static inline uint64_t getICC_IAR0(void)
{
    uint64_t retc;

    asm("mrs  %0, "stringify(ICC_IAR0_EL1)"\n" : "=r" (retc));

    return retc;
}

static inline uint64_t getICC_IAR1(void)
{
    uint64_t retc;

    asm("mrs  %0, "stringify(ICC_IAR1_EL1)"\n" : "=r" (retc));

    return retc;
}

static inline void setICC_EOIR0(uint32_t interrupt)
{
    asm("msr  "stringify(ICC_EOIR0_EL1)", %0\n; isb" :: "r" ((uint64_t)interrupt));
}

static inline void setICC_EOIR1(uint32_t interrupt)
{
    asm("msr  "stringify(ICC_EOIR1_EL1)", %0\n; isb" :: "r" ((uint64_t)interrupt));
}

static inline void setICC_DIR(uint32_t interrupt)
{
    asm("msr  "stringify(ICC_DIR_EL1)", %0\n; isb" :: "r" ((uint64_t)interrupt));
}

static inline void setICC_PMR(uint32_t priority)
{
    asm("msr  "stringify(ICC_PMR_EL1)", %0\n; isb" :: "r" ((uint64_t)priority));
}

static inline void setICC_BPR0(uint32_t binarypoint)
{
    asm("msr  "stringify(ICC_BPR0_EL1)", %0\n; isb" :: "r" ((uint64_t)binarypoint));
}

static inline void setICC_BPR1(uint32_t binarypoint)
{
    asm("msr  "stringify(ICC_BPR1_EL1)", %0\n; isb" :: "r" ((uint64_t)binarypoint));
}

static inline uint64_t getICC_BPR0(void)
{
    uint64_t retc;

    asm("mrs  %0, "stringify(ICC_BPR0_EL1)"\n" : "=r" (retc));

    return retc;
}

static inline uint64_t getICC_BPR1(void)
{
    uint64_t retc;

    asm("mrs  %0, "stringify(ICC_BPR1_EL1)"\n" : "=r" (retc));

    return retc;
}

static inline uint64_t getICC_RPR(void)
{
    uint64_t retc;

    asm("mrs  %0, "stringify(ICC_RPR_EL1)"\n" : "=r" (retc));

    return retc;
}

/**********************************************************************/

typedef enum
{
    sgirIRMTarget = 0,
    sgirIRMAll = (1ull << 40)
} ICC_SGIRBits_t;

static inline void setICC_SGI0R(uint8_t aff3, uint8_t aff2,
                uint8_t aff1, ICC_SGIRBits_t irm,
                uint16_t targetlist, uint8_t intid)
{
    uint64_t packedbits = (((uint64_t)aff3 << 48) | ((uint64_t)aff2 << 32) | \
               ((uint64_t)aff1 << 16) | irm | targetlist | \
               ((uint64_t)(intid & 0x0f) << 24));

    asm("msr  "stringify(ICC_SGI0R_EL1)", %0\n; isb" :: "r" (packedbits));
}

static inline void setICC_SGI1R(uint8_t aff3, uint8_t aff2,
                uint8_t aff1, ICC_SGIRBits_t irm,
                uint16_t targetlist, uint8_t intid)
{
    uint64_t packedbits = (((uint64_t)aff3 << 48) | ((uint64_t)aff2 << 32) | \
               ((uint64_t)aff1 << 16) | irm | targetlist | \
               ((uint64_t)(intid & 0x0f) << 24));

    asm("msr  "stringify(ICC_SGI1R_EL1)", %0\n; isb" :: "r" (packedbits));
}

static inline void setICC_ASGI1R(uint8_t aff3, uint8_t aff2,
                uint8_t aff1, ICC_SGIRBits_t irm,
                uint16_t targetlist, uint8_t intid)
{
    uint64_t packedbits = (((uint64_t)aff3 << 48) | ((uint64_t)aff2 << 32) | \
               ((uint64_t)aff1 << 16) | irm | targetlist | \
               ((uint64_t)(intid & 0x0f) << 24));

    asm("msr  "stringify(ICC_ASGI1R_EL1)", %0\n; isb" :: "r" (packedbits));
}

#endif /* ndef GICV3_gicc_h */
