/***************************************************************************
 * Copyright (c) 2026 safecubed GmbH
 * Copyright (c) 2026 Eclipse ThreadX contributors
 *
 * This program and the accompanying materials are made available under the
 * terms of the MIT License which is available at
 * https://opensource.org/licenses/MIT.
 *
 * SPDX-License-Identifier: MIT
 **************************************************************************/

/* Portions of this file were generated with AI assistance. */


/* Definitions shared by the sources of the TriCore port. Neither the ThreadX
   common sources nor the application include this file.  */

#ifndef TX_PORT_TRICORE_H
#define TX_PORT_TRICORE_H

#include "IfxCpu_bf.h"


/* TriCore core registers.  */

#define TX_PORT_TRICORE_CSFR_PCXI                        0xFE00
#define TX_PORT_TRICORE_CSFR_FCX                         0xFE38
#define TX_PORT_TRICORE_CSFR_LCX                         0xFE3C


/* Context save areas (CSAs). A link word holds the segment of a CSA in bits
   19:16 and its offset, divided by 64, in bits 15:0.  */

#define TX_PORT_TRICORE_CSA_SIZE                         64

#define TX_PORT_TRICORE_CSA_LINK_SEGMENT_OFF             16
#define TX_PORT_TRICORE_CSA_LINK_SEGMENT_MASK            0x000F0000UL
#define TX_PORT_TRICORE_CSA_LINK_OFFSET_MASK             0x0000FFFFUL
#define TX_PORT_TRICORE_CSA_LINK_MASK                    (TX_PORT_TRICORE_CSA_LINK_SEGMENT_MASK | TX_PORT_TRICORE_CSA_LINK_OFFSET_MASK)

/* Bit positions of the segment and of the offset in the address of a CSA.  */
#define TX_PORT_TRICORE_CSA_ADDRESS_SEGMENT_OFF          28
#define TX_PORT_TRICORE_CSA_ADDRESS_OFFSET_OFF           6

/* Word indices within a CSA.  */
#define TX_PORT_TRICORE_CSA_PCXI_LINK_WORD               0
#define TX_PORT_TRICORE_CSA_LOWER_CONTEXT_A11_RA         1

#define TX_PORT_TRICORE_CSA_UPPER_CONTEXT_PCXI_LINK_WORD 0
#define TX_PORT_TRICORE_CSA_UPPER_CONTEXT_PSW            1
#define TX_PORT_TRICORE_CSA_UPPER_CONTEXT_A10_STACK_PTR  2
#define TX_PORT_TRICORE_CSA_UPPER_CONTEXT_A11_RA         3

/* Initial context of a thread.  */
#define TX_PORT_TRICORE_PSW_IO_SUPERVISOR                2UL

/* Supervisor mode, protection register set 0. PSW.CDC = 0x7F disables call
   depth counting.  */
#define TX_PORT_TRICORE_CSA_INITIAL_SYSTEM_PSW           ((TX_PORT_TRICORE_PSW_IO_SUPERVISOR << IFX_CPU_PSW_IO_OFF) | \
                                                          (1UL << IFX_CPU_PSW_CDE_OFF) | \
                                                          ((UINT) IFX_CPU_PSW_CDC_MSK << IFX_CPU_PSW_CDC_OFF))
/* PCXI.UL = 1: links to an upper context.  */
#define TX_PORT_TRICORE_CSA_INITIAL_LOWER_PCXI           ((1UL << IFX_CPU_PCXI_UL_OFF) | (1UL << IFX_CPU_PCXI_PIE_OFF))
#define TX_PORT_TRICORE_CSA_INITIAL_UPPER_PCXI           (1UL << IFX_CPU_PCXI_PIE_OFF)

#define TX_PORT_TRICORE_STACK_ALIGNMENT                  8U

#define TX_PORT_TRICORE_CSA_LINK_WORD_END_OF_CHAIN       0U


/* CSA links from _tx_thread_csa_save and _tx_thread_csa_restore back to the
   lower context of the interrupted thread.  */

#define TX_PORT_TRICORE_SYSCALL_CALL_DEPTH               3
#define TX_PORT_TRICORE_DISPATCH_CALL_DEPTH              2


#ifndef __ASSEMBLER__

#include "tx_api.h"

UINT * _tx_thread_decode_link_word(UINT link_word);

#endif  /* __ASSEMBLER__ */

#endif  /* TX_PORT_TRICORE_H */
