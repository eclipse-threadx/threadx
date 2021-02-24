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

#define TX_SOURCE_CODE


/* Include necessary system files.  */

#include "tx_api.h"
#include "tx_trace.h"
#include "tx_thread.h"

#ifndef TX_THREAD_LOCAL_STORAGE_SLOTS
#define TX_THREAD_LOCAL_STORAGE_SLOTS 0
#endif

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _tx_thread_local_storage_set                        PORTABLE C      */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Alex Earl                                                           */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function sets the value of a thread local variable             */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    thread_ptr                            Thread control block pointer  */
/*    index                                 Index of thread local data    */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    return status                         Thread local return status    */
/*                                                                        */
/*  CALLS                                                                 */
/*  _tx_thread_identify                                                   */
/*                                                                        */
/*  CALLED BY                                                             */
/*  Application code                                                      */
/*                                                                        */
/*  RELEASE HISTORY                                                       */
/*                                                                        */
/*    DATE              NAME                      DESCRIPTION             */
/*                                                                        */
/*  01-27-2021     Alex Earl                Initial Version 6.0           */
/*                                                                        */
/**************************************************************************/
UINT  _tx_thread_local_storage_set(TX_THREAD *thread_ptr, UINT index, VOID *value)
{

    #if TX_THREAD_LOCAL_STORAGE_SLOTS > 0
    if(index < TX_THREAD_LOCAL_STORAGE_SLOTS)
    {

        /* Check if we need to get the current thread pointer */
        if(NULL == thread_ptr)
        {

            /* Retrieve the current thread pointer */
            thread_ptr = _tx_thread_identify();
        }

        if(NULL != thread_ptr)
        {

            /* If we have a thread pointer, retrieve the data from the index location */
            thread_ptr->tx_thread_local_storage_pointers[index] = value;
            return(TX_SUCCESS);
        }
        else
        {

            return(TX_THREAD_ERROR);
        }
    }
    #endif

    return(TX_NOT_AVAILABLE);
}

