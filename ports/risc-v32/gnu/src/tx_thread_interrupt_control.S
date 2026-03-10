/***************************************************************************
 * Copyright (c) 2025 10xEngineers
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
/**   Thread                                                              */
/**                                                                       */
/**************************************************************************/
/**************************************************************************/

/* #define TX_SOURCE_CODE  */


/* Include necessary system files.  */

/*  #include "tx_api.h"
    #include "tx_thread.h"  */

.equ RETURN_MASK,               0x0000000F
.equ SET_SR_MASK,               0xFFFFFFF0

    .section .text
    .balign 4
/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _tx_thread_interrupt_control                       RISC-V32/GNU     */
/*                                                           6.4.x        */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Akif Ejaz, 10xEngineers                                             */
/*    Wei-Chen Lai, National Cheng Kung University                        */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function is responsible for changing the interrupt lockout     */
/*    posture of the system.                                              */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    new_posture                           New interrupt lockout posture */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    old_posture                           Old interrupt lockout posture */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    None                                                                */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application Code                                                    */
/*                                                                        */
/**************************************************************************/

/* UINT   _tx_thread_interrupt_control(UINT new_posture)
{  */
    .globl  _tx_thread_interrupt_control
_tx_thread_interrupt_control:
    /* Pickup current interrupt lockout posture.  */

    csrr    t0, mstatus
    mv      t1, t0                                      // Save original mstatus for return

    /* Apply the new interrupt posture.  */
    
    li      t2, SET_SR_MASK                             // Build set SR mask
    and     t0, t0, t2                                  // Isolate interrupt lockout bits
    or      t0, t0, a0                                  // Put new lockout bits in
    csrw    mstatus, t0
    andi    a0, t1, RETURN_MASK                         // Return original mstatus.
    ret
