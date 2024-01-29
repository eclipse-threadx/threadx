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
/**   Trace                                                               */
/**                                                                       */
/**************************************************************************/
/**************************************************************************/

#define TX_SOURCE_CODE


/* Include necessary system files.  */

#include "tx_api.h"
#ifdef TX_ENABLE_EVENT_TRACE
#include "tx_thread.h"
#endif
#include "tx_trace.h"


/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _tx_trace_isr_enter_insert                          PORTABLE C      */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    William E. Lamie, Microsoft Corporation                             */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function provides inserts an ISR entry event into the trace    */
/*    buffer.                                                             */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    isr_id                                User defined ISR ID           */
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
/*    Application Code                                                    */
/*                                                                        */
/*  RELEASE HISTORY                                                       */
/*                                                                        */
/*    DATE              NAME                      DESCRIPTION             */
/*                                                                        */
/*  05-19-2020     William E. Lamie         Initial Version 6.0           */
/*  09-30-2020     Yuxin Zhou               Modified comment(s),          */
/*                                            resulting in version 6.1    */
/*                                                                        */
/**************************************************************************/
VOID  _tx_trace_isr_enter_insert(ULONG isr_id)
{

TX_INTERRUPT_SAVE_AREA


#ifdef TX_ENABLE_EVENT_TRACE

UINT            stack_address;
ULONG           system_state;
UINT            preempt_disable;


    /* Disable interrupts.  */
    TX_DISABLE

    /* Insert this event into the trace buffer.  */
    system_state =  TX_THREAD_GET_SYSTEM_STATE();
    preempt_disable =  _tx_thread_preempt_disable;
    TX_TRACE_IN_LINE_INSERT(TX_TRACE_ISR_ENTER, &stack_address, isr_id, system_state, preempt_disable, TX_TRACE_INTERNAL_EVENTS)

    /* Restore interrupts.  */
    TX_RESTORE
#else

    /* Access input arguments just for the sake of lint, MISRA, etc.  */
    if (isr_id != ((ULONG) 0))
    {

        /* NOP code.  */
        TX_DISABLE
        TX_RESTORE
    }
#endif
}

