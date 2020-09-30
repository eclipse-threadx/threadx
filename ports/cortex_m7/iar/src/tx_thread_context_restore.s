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
    EXTERN  _tx_thread_system_state
    EXTERN  _tx_thread_current_ptr
    EXTERN  _tx_thread_system_stack_ptr
    EXTERN  _tx_thread_execute_ptr
    EXTERN  _tx_timer_time_slice
    EXTERN  _tx_thread_schedule
    EXTERN  _tx_thread_preempt_disable
    EXTERN  _tx_execution_isr_exit
;
;
    SECTION `.text`:CODE:NOROOT(2)
    THUMB
;/**************************************************************************/
;/*                                                                        */
;/*  FUNCTION                                               RELEASE        */
;/*                                                                        */
;/*    _tx_thread_context_restore                        Cortex-M7/IAR     */
;/*                                                           6.1          */
;/*  AUTHOR                                                                */
;/*                                                                        */
;/*    William E. Lamie, Microsoft Corporation                             */
;/*                                                                        */
;/*  DESCRIPTION                                                           */
;/*                                                                        */
;/*    This function is only needed for legacy applications and it should  */
;/*    not be called in any new development on a Cortex-M.                 */
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
;/*  09-30-2020     William E. Lamie         Initial Version 6.1           */
;/*                                                                        */
;/**************************************************************************/
;VOID   _tx_thread_context_restore(VOID)
;{
    PUBLIC  _tx_thread_context_restore
_tx_thread_context_restore:

#ifdef TX_ENABLE_EXECUTION_CHANGE_NOTIFY
;
;    /* Call the ISR exit function to indicate an ISR is complete.  */
;
    PUSH    {r0,lr}                                 ; Save ISR lr
    BL      _tx_execution_isr_exit                  ; Call the ISR exit function
    POP     {r0,lr}                                 ; Restore ISR lr
#endif
;
    POP     {lr}
    BX      lr
;
;}
    END
