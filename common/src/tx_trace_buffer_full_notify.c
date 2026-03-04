/***************************************************************************
 * Copyright (c) 2024 Microsoft Corporation
 * Copyright (c) 2026-present Eclipse ThreadX contributors
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

#ifndef TX_SOURCE_CODE
#define TX_SOURCE_CODE
#endif


/* Include necessary system files.  */

#include "tx_api.h"
#include "tx_trace.h"


/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _tx_trace_buffer_full_notify                        PORTABLE C      */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    William E. Lamie, Microsoft Corporation                             */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function sets up the application callback function that is     */
/*    called whenever the trace buffer becomes full. The application      */
/*    can then swap to a new trace buffer in order not to lose any        */
/*    events.                                                             */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    full_buffer_callback                  Full trace buffer processing  */
/*                                            function                    */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    Completion Status                                                   */
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
UINT  _tx_trace_buffer_full_notify(VOID (*full_buffer_callback)(VOID *buffer))
{

#ifdef TX_ENABLE_EVENT_TRACE

    /* Setup the callback function pointer.  */
    _tx_trace_full_notify_function =  full_buffer_callback;

    /* Return success.  */
    return(TX_SUCCESS);

#else

UINT    status;


    /* Access input arguments just for the sake of lint, MISRA, etc.  */
    if (full_buffer_callback != TX_NULL)
    {

        /* Trace not enabled, return an error.  */
        status =  TX_FEATURE_NOT_ENABLED;
    }
    else
    {

        /* Trace not enabled, return an error.  */
        status =  TX_FEATURE_NOT_ENABLED;
    }

    /* Return completion status.  */
    return(status);
#endif
}

