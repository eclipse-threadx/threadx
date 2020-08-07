//
// Aliases for GICv3 registers
//
// Copyright (c) 2016-2017 Arm Limited (or its affiliates). All rights reserved.
// Use, modification and redistribution of this file is subject to your possession of a
// valid End User License Agreement for the Arm Product of which these examples are part of 
// and your compliance with all applicable terms and conditions of such licence agreement.
//

#ifndef GICV3_ALIASES_H
#define GICV3_ALIASES_H

#ifndef __clang__

/*
 * Mapping of MSR and MRS to physical and virtual CPU interface registers
 *
 * Arm Generic Interrupt Controller Architecture Specification
 * GIC architecture version 3.0 and version 4.0
 * Table 8-5
 */
#define ICC_AP0R0_EL1       S3_0_C12_C8_4
#define ICC_AP0R1_EL1       S3_0_C12_C8_5
#define ICC_AP0R2_EL1       S3_0_C12_C8_6
#define ICC_AP0R3_EL1       S3_0_C12_C8_7

#define ICC_AP1R0_EL1       S3_0_C12_C9_0
#define ICC_AP1R1_EL1       S3_0_C12_C9_1
#define ICC_AP1R2_EL1       S3_0_C12_C9_2
#define ICC_AP1R3_EL1       S3_0_C12_C9_3

#define ICC_ASGI1R_EL1      S3_0_C12_C11_6

#define ICC_BPR0_EL1        S3_0_C12_C8_3
#define ICC_BPR1_EL1        S3_0_C12_C12_3

#define ICC_CTLR_EL1        S3_0_C12_C12_4
#define ICC_CTLR_EL3        S3_6_C12_C12_4

#define ICC_DIR_EL1         S3_0_C12_C11_1

#define ICC_EOIR0_EL1       S3_0_C12_C8_1
#define ICC_EOIR1_EL1       S3_0_C12_C12_1

#define ICC_HPPIR0_EL1      S3_0_C12_C8_2
#define ICC_HPPIR1_EL1      S3_0_C12_C12_2

#define ICC_IAR0_EL1        S3_0_C12_C8_0
#define ICC_IAR1_EL1        S3_0_C12_C12_0

#define ICC_IGRPEN0_EL1     S3_0_C12_C12_6
#define ICC_IGRPEN1_EL1     S3_0_C12_C12_7
#define ICC_IGRPEN1_EL3     S3_6_C12_C12_7

#define ICC_PMR_EL1         S3_0_C4_C6_0
#define ICC_RPR_EL1         S3_0_C12_C11_3

#define ICC_SGI0R_EL1       S3_0_C12_C11_7
#define ICC_SGI1R_EL1       S3_0_C12_C11_5

#define ICC_SRE_EL1         S3_0_C12_C12_5
#define ICC_SRE_EL2         S3_4_C12_C9_5
#define ICC_SRE_EL3         S3_6_C12_C12_5

/*
 * Mapping of MSR and MRS to virtual interface control registers
 *
 * Arm Generic Interrupt Controller Architecture Specification
 * GIC architecture version 3.0 and version 4.0
 * Table 8-6
 */
#define ICH_AP0R0_EL2       S3_4_C12_C8_0
#define ICH_AP0R1_EL2       S3_4_C12_C8_1
#define ICH_AP0R2_EL2       S3_4_C12_C8_2
#define ICH_AP0R3_EL2       S3_4_C12_C8_3

#define ICH_AP1R0_EL2       S3_4_C12_C9_0
#define ICH_AP1R1_EL2       S3_4_C12_C9_1
#define ICH_AP1R2_EL2       S3_4_C12_C9_2
#define ICH_AP1R3_EL2       S3_4_C12_C9_3

#define ICH_HCR_EL2         S3_4_C12_C11_0

#define ICH_VTR_EL2         S3_4_C12_C11_1

#define ICH_MISR_EL2        S3_4_C12_C11_2

#define ICH_EISR_EL2        S3_4_C12_C11_3

#define ICH_ELRSR_EL2       S3_4_C12_C11_5

#define ICH_VMCR_EL2        S3_4_C12_C11_7

#define ICH_LR0_EL2         S3_4_C12_C12_0
#define ICH_LR1_EL2         S3_4_C12_C12_1
#define ICH_LR2_EL2         S3_4_C12_C12_2
#define ICH_LR3_EL2         S3_4_C12_C12_3
#define ICH_LR4_EL2         S3_4_C12_C12_4
#define ICH_LR5_EL2         S3_4_C12_C12_5
#define ICH_LR6_EL2         S3_4_C12_C12_6
#define ICH_LR7_EL2         S3_4_C12_C12_7
#define ICH_LR8_EL2         S3_4_C12_C13_0
#define ICH_LR9_EL2         S3_4_C12_C13_1
#define ICH_LR10_EL2        S3_4_C12_C13_2
#define ICH_LR11_EL2        S3_4_C12_C13_3
#define ICH_LR12_EL2        S3_4_C12_C13_4
#define ICH_LR13_EL2        S3_4_C12_C13_5
#define ICH_LR14_EL2        S3_4_C12_C13_6
#define ICH_LR15_EL2        S3_4_C12_C13_7

#endif /* not __clang__ */

#endif /* GICV3_ALIASES */
