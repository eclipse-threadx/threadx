/***************************************************************************
 * Copyright (c) 2024 Microsoft Corporation 
 * 
 * This program and the accompanying materials are made available under the
 * terms of the MIT License which is available at
 * https://opensource.org/licenses/MIT.
 * 
 * SPDX-License-Identifier: MIT
 **************************************************************************/

#ifndef TM_PORTING_LAYER_H
#define TM_PORTING_LAYER_H

#include <stdio.h>


/* Define the TRAP instruction. This is used by the Interrupt Processing and Interrupt Preemption Processing tests. 
   The SVC instruction below is for Cortex-M architectures using IAR tools. This will likely need to be modified 
   for different processors and/or development tools. 

   Note also that for the Interrupt Processing test there is the assumption that the SVC ISR looks like:

    PUBLIC  SVC_Handler
SVC_Handler:
    PUSH    {lr}
    BL      tm_interrupt_handler
    POP     {lr}
    BX      LR

    And that for the Interrupt Preemption Processing test the SVC ISR looks like:

    PUBLIC  SVC_Handler
SVC_Handler:
    PUSH    {lr}
    BL      tm_interrupt_preemption_handler
    POP     {lr}
    BX      LR

   Again, this is very processor/tool specific so changes are likely needed for non Cortex-M/IAR
   environments.  */

#define TM_CAUSE_INTERRUPT    asm("SVC #0");



#endif

