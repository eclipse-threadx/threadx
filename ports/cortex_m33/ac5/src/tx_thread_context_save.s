;/**************************************************************************/
;/*                                                                        */
;/*       Copyright (c) Microsoft Corporation. All rights reserved.        */
;/*                                                                        */
;/*       This software is licensed under the Microsoft Software License   */
;/*       Terms for Microsoft Azure RTOS. Full text of the license can be  */
;/*       found in the LICENSE file at https://aka.ms/AzureRTOS_EULA       */
;/*       and in the root directory of this software.                      */
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
    IF :DEF:TX_ENABLE_EXECUTION_CHANGE_NOTIFY
    IMPORT  _tx_execution_isr_enter
    ENDIF
;
;
    AREA    ||.text||, CODE, READONLY
    PRESERVE8
;/**************************************************************************/
;/*                                                                        */
;/*  FUNCTION                                               RELEASE        */
;/*                                                                        */
;/*    _tx_thread_context_save                           Cortex-M33/AC5    */
;/*                                                           6.1          */
;/*  AUTHOR                                                                */
;/*                                                                        */
;/*    Scott Larson, Microsoft Corporation                                 */
;/*                                                                        */
;/*  DESCRIPTION                                                           */
;/*                                                                        */
;/*    This function saves the context of an executing thread in the       */
;/*    beginning of interrupt processing.  The function also ensures that  */
;/*    the system stack is used upon return to the calling ISR.            */
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
;/*    None                                                                */
;/*                                                                        */
;/*  CALLED BY                                                             */
;/*                                                                        */
;/*    ISRs                                                                */
;/*                                                                        */
;/*  RELEASE HISTORY                                                       */
;/*                                                                        */
;/*    DATE              NAME                      DESCRIPTION             */
;/*                                                                        */
;/*  09-30-2020      Scott Larson            Initial Version 6.1           */
;/*                                                                        */
;/**************************************************************************/
;VOID   _tx_thread_context_save(VOID)
;{
    EXPORT  _tx_thread_context_save
_tx_thread_context_save     FUNCTION
   IF :DEF:TX_ENABLE_EXECUTION_CHANGE_NOTIFY
;
;    /* Call the ISR enter function to indicate an ISR is executing.  */
;
    PUSH    {r0, lr}                                ; Save return address
    BL      _tx_execution_isr_enter                 ; Call the ISR enter function
    POP     {r0, lr}                                ; Recover return address
    ENDIF
;
;    /* Return to interrupt processing.  */
;     
    BX      lr
;}
    ENDFUNC
    ALIGN
    LTORG
    END
    
