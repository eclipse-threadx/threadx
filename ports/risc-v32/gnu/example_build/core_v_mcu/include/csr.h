/***************************************************************************/
/* Copyright (C) 2020 ETH Zurich and University of Bologna
 * Copyright (C) 2026 Eclipse ThreadX contributors
 *
 * Derived from core-v-freertos (https://github.com/openhwgroup/core-v-freertos)
 * Original work licensed under the Apache License, Version 2.0.
 * See https://www.apache.org/licenses/LICENSE-2.0
 * Modifications licensed under MIT (https://opensource.org/licenses/MIT).
 *
 * AI Disclosure: Some portions generated or modified by Copilot (Sonnet 4.6).
 *
 * SPDX-License-Identifier: Apache-2.0 AND MIT
 ***************************************************************************/

#ifndef CSR_H
#define CSR_H

#include "bits.h"

#define CSR_MSTATUS  0x300
#define CSR_MISA     0x301
#define CSR_MIE      0x304
#define CSR_MTVEC    0x305
#define CSR_MSCRATCH 0x340
#define CSR_MEPC     0x341
#define CSR_MCAUSE   0x342
#define CSR_MTVAL    0x343
#define CSR_MIP      0x344
#define CSR_PMPCFG0   0x3A0
#define CSR_PMPADDR0   0x3B0
#define CSR_MVENDORID  0xF11
#define CSR_MARCHID    0xF12
#define CSR_MIMPID     0xF13
#define CSR_MHARTID    0xF14

#define MSTATUS_IE BIT(3)

#define __CSR_EXPAND(x) #x

#ifndef csr_read
#define csr_read(csr) \
    ({ \
        register unsigned long __val; \
        __asm__ volatile("csrr %0, " __CSR_EXPAND(csr) : "=r"(__val) : : "memory"); \
        __val; \
    })
#endif

#ifndef csr_write
#define csr_write(csr, val) \
    ({ \
        unsigned long __val = (unsigned long)(val); \
        __asm__ volatile("csrw " __CSR_EXPAND(csr) ", %0" : : "rK"(__val) : "memory"); \
    })
#endif

#ifndef csr_read_clear
#define csr_read_clear(csr, val) \
    ({ \
        unsigned long __val = (unsigned long)(val); \
        __asm__ volatile("csrrc %0, " __CSR_EXPAND(csr) ", %1" : "=r"(__val) : "rK"(__val) : "memory"); \
        __val; \
    })
#endif

#ifndef csr_read_set
#define csr_read_set(csr, val) \
    ({ \
        unsigned long __val = (unsigned long)(val); \
        __asm__ volatile("csrrs %0, " __CSR_EXPAND(csr) ", %1" : "=r"(__val) : "rK"(__val) : "memory"); \
        __val; \
    })
#endif

#endif /* CSR_H */
