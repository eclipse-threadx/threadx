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
#include "tx_trace.h"


/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _tx_trace_disable                                   PORTABLE C      */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    William E. Lamie, Microsoft Corporation                             */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function disables trace inside of ThreadX.                     */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    None                                                                */
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
/*  RELEASE HISTORY                                                       */
/*                                                                        */
/*    DATE              NAME                      DESCRIPTION             */
/*                                                                        */
/*  05-19-2020     William E. Lamie         Initial Version 6.0           */
/*  09-30-2020     Yuxin Zhou               Modified comment(s),          */
/*                                            resulting in version 6.1    */
/*                                                                        */
/**************************************************************************/
UINT  _tx_trace_disable(VOID)
{

#ifdef TX_ENABLE_EVENT_TRACE
UINT     status;


    /* Determine if trace is already disabled.  */
    if (_tx_trace_buffer_current_ptr == TX_NULL)
    {

        /* Yes, trace is already disabled.  */
        status =  TX_NOT_DONE;
    }
    else
    {

        /* Otherwise, simply clear the current pointer and registery start pointer to disable the trace.  */
        _tx_trace_buffer_current_ptr =  TX_NULL;
        _tx_trace_registry_start_ptr =  TX_NULL;

        /* Successful completion.  */
        status =  TX_SUCCESS;
    }

    /* Return completion status.  */
    return(status);

#else

    /* Trace not enabled, return an error.  */
    return(TX_FEATURE_NOT_ENABLED);
#endif
}

