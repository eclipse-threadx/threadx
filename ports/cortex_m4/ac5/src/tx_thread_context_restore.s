;/**************************************************************************/
;/*                                                                        */
;/*       Copyright (c) Microsoft Corporation. All rights reserved.        */
;/*                                                                        */
;/**************************************************************************/
;
;
;/**************************************************************************/
;/**************************************************************************/
;/**                                                                       */
;/** ThreadX Component                                                     */
;/**                                                                       */
;/**   Thread                                                              */
;/**                                                                       */
;/**************************************************************************/
;/**************************************************************************/
;
;
;#define TX_SOURCE_CODE
;
;
;/* Include necessary system files.  */
;
;#include "tx_api.h"
;#include "tx_thread.h"
;#include "tx_timer.h"
;
;
    IF :DEF:TX_ENABLE_EXECUTION_CHANGE_NOTIFY
    IMPORT  _tx_execution_isr_exit
    ENDIF
;
;
    AREA    ||.text||, CODE, READONLY
    PRESERVE8
;/**************************************************************************/
;/*                                                                        */
;/*  FUNCTION                                               RELEASE        */
;/*                                                                        */
;/*    _tx_thread_context_restore                        Cortex-M4/AC5     */
;/*                                                           6.0.1        */
;/*  AUTHOR                                                                */
;/*                                                                        */
;/*    William E. Lamie, Microsoft Corporation                             */
;/*                                                                        */
;/*  DESCRIPTION                                                           */
;/*                                                                        */
;/*    This function restores the interrupt context if it is processing a  */
;/*    nested interrupt.  If not, it returns to the interrupt thread if no */
;/*    preemption is necessary.  Otherwise, if preemption is necessary or  */
;/*    if no thread was running, the function returns to the scheduler.    */
;/*                                                                        */
;/*  INPUT                                                                 */
;/*                                                                        */
;/*    None                                                                */
;/*                                                                        */
;/*  OUTPUT                                                                */
;/*                                                                        */
;/*    None                                                                */
;/*                                                                        */
;/*  CALLS                                                                 */
;/*                                                                        */
;/*    _tx_thread_schedule                   Thread scheduling routine     */
;/*                                                                        */
;/*  CALLED BY                                                             */
;/*                                                                        */
;/*    ISRs                                  Interrupt Service Routines    */
;/*                                                                        */
;/*  RELEASE HISTORY                                                       */
;/*                                                                        */
;/*    DATE              NAME                      DESCRIPTION             */
;/*                                                                        */
;/*  06-30-2020     William E. Lamie         Initial Version 6.0.1         */
;/*                                                                        */
;/**************************************************************************/
;VOID   _tx_thread_context_restore(VOID)
;{
    EXPORT  _tx_thread_context_restore
_tx_thread_context_restore
    IF :DEF:TX_ENABLE_EXECUTION_CHANGE_NOTIFY
;
;    /* Call the ISR exit function to indicate an ISR is complete.  */
;
    PUSH    {r0,lr}                                 ; Save ISR lr
    BL      _tx_execution_isr_exit                  ; Call the ISR exit function
    POP     {r0,lr}                                 ; Restore ISR lr
    ENDIF
;
    POP     {lr}
    BX      lr
;}
    ALIGN
    LTORG
    END
