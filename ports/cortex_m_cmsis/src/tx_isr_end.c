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
/**   ISR                                                                 */
/**                                                                       */
/**************************************************************************/
/**************************************************************************/


/* Include necessary system files.  */

#include "tx_api.h"
#ifdef TX_ENABLE_EVENT_TRACE
#include "tx_trace.h"
#endif
#ifdef TX_ENABLE_EXECUTION_CHANGE_NOTIFY
#include "tx_execution_profile.h"
#endif


/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    tx_isr_end                                          Cortex-M        */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function is responsible for indicating the end of processing   */
/*    for the specified interrupt. This is purely for diagnostic use with */
/*    either TraceX or execution profiling.                               */
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
/*    _tx_trace_isr_exit_insert         Mark the end of an ISR            */
/*    _tx_execution_isr_exit            Mark the end of ISR processing    */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    ISR                                                                 */
/*                                                                        */
/**************************************************************************/

#if defined(TX_ENABLE_EVENT_TRACE) || defined(TX_ENABLE_EXECUTION_CHANGE_NOTIFY)
 
VOID  tx_isr_end(ULONG isr_id)
{

#ifdef TX_ENABLE_EXECUTION_CHANGE_NOTIFY
TX_INTERRUPT_SAVE_AREA
#endif

#ifdef TX_ENABLE_EVENT_TRACE

    /* Indicate the end of the ISR in the trace buffer.  */
    _tx_trace_isr_exit_insert(isr_id);
#else
    (void) isr_id;
#endif    
    
#ifdef TX_ENABLE_EXECUTION_CHANGE_NOTIFY

    /* Disable interrupts.  */
    TX_DISABLE
    
    /* Indicate the end of ISR processing.  */
    _tx_execution_isr_exit();

    /* Restore interrupts.  */
    TX_RESTORE
#endif
}

#endif
