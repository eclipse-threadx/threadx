//
// Defines for v8 System Registers
//
// Copyright (c) 2012-2016 Arm Limited (or its affiliates). All rights reserved.
// Use, modification and redistribution of this file is subject to your
// possession of a valid DS-5 end user licence agreement and your compliance
// with all applicable terms and conditions of such licence agreement.
//

#ifndef V8_SYSTEM_H
#define V8_SYSTEM_H

//
// AArch64 SPSR
//
#define AARCH64_SPSR_EL3h 0b1101
#define AARCH64_SPSR_EL3t 0b1100
#define AARCH64_SPSR_EL2h 0b1001
#define AARCH64_SPSR_EL2t 0b1000
#define AARCH64_SPSR_EL1h 0b0101
#define AARCH64_SPSR_EL1t 0b0100
#define AARCH64_SPSR_EL0t 0b0000
#define AARCH64_SPSR_RW (1 << 4)
#define AARCH64_SPSR_F  (1 << 6)
#define AARCH64_SPSR_I  (1 << 7)
#define AARCH64_SPSR_A  (1 << 8)
#define AARCH64_SPSR_D  (1 << 9)
#define AARCH64_SPSR_IL (1 << 20)
#define AARCH64_SPSR_SS (1 << 21)
#define AARCH64_SPSR_V  (1 << 28)
#define AARCH64_SPSR_C  (1 << 29)
#define AARCH64_SPSR_Z  (1 << 30)
#define AARCH64_SPSR_N  (1 << 31)

//
// Multiprocessor Affinity Register
//
#define MPIDR_EL1_AFF3_LSB 32
#define MPIDR_EL1_U  (1 << 30)
#define MPIDR_EL1_MT (1 << 24)
#define MPIDR_EL1_AFF2_LSB 16
#define MPIDR_EL1_AFF1_LSB  8
#define MPIDR_EL1_AFF0_LSB  0
#define MPIDR_EL1_AFF_WIDTH 8

//
// Data Cache Zero ID Register
//
#define DCZID_EL0_BS_LSB   0
#define DCZID_EL0_BS_WIDTH 4
#define DCZID_EL0_DZP_LSB  5
#define DCZID_EL0_DZP (1 << 5)

//
// System Control Register
//
#define SCTLR_EL1_UCI     (1 << 26)
#define SCTLR_ELx_EE      (1 << 25)
#define SCTLR_EL1_E0E     (1 << 24)
#define SCTLR_ELx_WXN     (1 << 19)
#define SCTLR_EL1_nTWE    (1 << 18)
#define SCTLR_EL1_nTWI    (1 << 16)
#define SCTLR_EL1_UCT     (1 << 15)
#define SCTLR_EL1_DZE     (1 << 14)
#define SCTLR_ELx_I       (1 << 12)
#define SCTLR_EL1_UMA     (1 << 9)
#define SCTLR_EL1_SED     (1 << 8)
#define SCTLR_EL1_ITD     (1 << 7)
#define SCTLR_EL1_THEE    (1 << 6)
#define SCTLR_EL1_CP15BEN (1 << 5)
#define SCTLR_EL1_SA0     (1 << 4)
#define SCTLR_ELx_SA      (1 << 3)
#define SCTLR_ELx_C       (1 << 2)
#define SCTLR_ELx_A       (1 << 1)
#define SCTLR_ELx_M       (1 << 0)

//
// Architectural Feature Access Control Register
//
#define CPACR_EL1_TTA     (1 << 28)
#define CPACR_EL1_FPEN    (3 << 20)

//
// Architectural Feature Trap Register
//
#define CPTR_ELx_TCPAC (1 << 31)
#define CPTR_ELx_TTA   (1 << 20)
#define CPTR_ELx_TFP   (1 << 10)

//
// Secure Configuration Register
//
#define SCR_EL3_TWE  (1 << 13)
#define SCR_EL3_TWI  (1 << 12)
#define SCR_EL3_ST   (1 << 11)
#define SCR_EL3_RW   (1 << 10)
#define SCR_EL3_SIF  (1 << 9)
#define SCR_EL3_HCE  (1 << 8)
#define SCR_EL3_SMD  (1 << 7)
#define SCR_EL3_EA   (1 << 3)
#define SCR_EL3_FIQ  (1 << 2)
#define SCR_EL3_IRQ  (1 << 1)
#define SCR_EL3_NS   (1 << 0)

//
// Hypervisor Configuration Register
//
#define HCR_EL2_ID   (1 << 33)
#define HCR_EL2_CD   (1 << 32)
#define HCR_EL2_RW   (1 << 31)
#define HCR_EL2_TRVM (1 << 30)
#define HCR_EL2_HVC  (1 << 29)
#define HCR_EL2_TDZ  (1 << 28)

#endif // V8_SYSTEM_H
