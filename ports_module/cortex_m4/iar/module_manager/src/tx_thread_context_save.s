/**************************************************************************/
/*                                                                        */
/*       Copyright (c) Microsoft Corporation. All rights reserved.        */
/*                                                                        */
/*       This software is licensed under the Microsoft Software License   */
/*       Terms for Microsoft Azure RTOS. Full text of the license can be  */
/*       found in the LICENSE file at https://aka.ms/AzureRTOS_EULA       */
/*       and in the root directory of this software.                      */
/*                                                                        */
/**************************************************************************/


/**************************************************************************/
/**************************************************************************/
/**                                                                       */
/** ThreadX Component                                                     */
/**                                                                       */
/**   Thread                                                              */
/**                                                                       */
/**************************************************************************/
/**************************************************************************/

    EXTERN  _tx_execution_isr_enter
    SECTION `.text`:CODE:NOROOT(2)
    THUMB
/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _tx_thread_context_save                           Cortex-M4/IAR     */
/*                                                           6.1.2        */
/*  AUTHOR                                                                */
/*                                                                        */
/*    William E. Lamie, Microsoft Corporation                             */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function is only needed for legacy applications and it should  */
/*    not be called in any new development on a Cortex-M.                 */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    None                                                                */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    None                                                                */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    None                                                                */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    ISRs                                                                */
/*                                                                        */
/*  RELEASE HISTORY                                                       */
/*                                                                        */
/*    DATE              NAME                      DESCRIPTION             */
/*                                                                        */
/*  09-30-2020     William E. Lamie         Initial Version 6.1           */
/*  11-09-2020     Scott Larson             Modified comment(s),          */
/*                                            resulting in version 6.1.2  */
/*                                                                        */
/**************************************************************************/
// VOID   _tx_thread_context_save(VOID)
// {
    PUBLIC  _tx_thread_context_save
_tx_thread_context_save:
#ifdef TX_ENABLE_EXECUTION_CHANGE_NOTIFY

    /* Call the ISR enter function to indicate an ISR is starting.  */

    PUSH    {r0, lr}                                // Save return address
    BL      _tx_execution_isr_enter                 // Call the ISR enter function
    POP     {r0, lr}                                // Recover return address
#endif

    /* Context is already saved - just return!  */

    BX      lr
// }
    END
