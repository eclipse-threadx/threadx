/***************************************************************************
 * Copyright (c) 2024 Microsoft Corporation 
 * 
 * This program and the accompanying materials are made available under the
 * terms of the MIT License which is available at
 * https://opensource.org/licenses/MIT.
 * 
 * SPDX-License-Identifier: MIT
 **************************************************************************/


/**************************************************************************/
/**************************************************************************/
/**                                                                       */
/** ThreadX Component                                                     */
/**                                                                       */
/**   Initialize                                                          */
/**                                                                       */
/**************************************************************************/
/**************************************************************************/

    .arm

    .global      _start
    .global      __tx_undefined
    .global      __tx_swi_interrupt
    .global      __tx_prefetch_handler
    .global      __tx_abort_handler
    .global      __tx_reserved_handler
    .global      __tx_irq_handler
    .global      __tx_fiq_handler

/* Define the vector area.  This should be located or copied to 0.  */

    .text
    .global __vectors
__vectors:

    LDR     pc, STARTUP                     // Reset goes to startup function
    LDR     pc, UNDEFINED                   // Undefined handler
    LDR     pc, SWI                         // Software interrupt handler
    LDR     pc, PREFETCH                    // Prefetch exception handler
    LDR     pc, ABORT                       // Abort exception handler
    LDR     pc, RESERVED                    // Reserved exception handler
    LDR     pc, IRQ                         // IRQ interrupt handler
    LDR     pc, FIQ                         // FIQ interrupt handler

STARTUP:
    .word  _start                           // Reset goes to C startup function
UNDEFINED:
    .word  __tx_undefined                   // Undefined handler
SWI:
    .word  __tx_swi_interrupt               // Software interrupt handler
PREFETCH:
    .word  __tx_prefetch_handler            // Prefetch exception handler
ABORT:
    .word  __tx_abort_handler               // Abort exception handler
RESERVED:
    .word  __tx_reserved_handler            // Reserved exception handler
IRQ:
    .word  __tx_irq_handler                 // IRQ interrupt handler
FIQ:
    .word  __tx_fiq_handler                 // FIQ interrupt handler
