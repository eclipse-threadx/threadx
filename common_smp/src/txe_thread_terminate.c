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
#include "tx_thread.h"


/**************************************************************************/ 
/*                                                                        */ 
/*  FUNCTION                                               RELEASE        */ 
/*                                                                        */ 
/*    _txe_thread_terminate                               PORTABLE C      */ 
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    William E. Lamie, Microsoft Corporation                             */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */ 
/*    This function checks for errors in the thread terminate function    */ 
/*    call.                                                               */ 
/*                                                                        */ 
/*  INPUT                                                                 */ 
/*                                                                        */ 
/*    thread_ptr                            Pointer to thread to suspend  */ 
/*                                                                        */ 
/*  OUTPUT                                                                */ 
/*                                                                        */ 
/*    TX_THREAD_ERROR                       Invalid thread pointer        */ 
/*    TX_CALLER_ERROR                       Invalid caller of function    */ 
/*    status                                Actual completion status      */ 
/*                                                                        */ 
/*  CALLS                                                                 */ 
/*                                                                        */ 
/*    _tx_thread_terminate                  Actual thread terminate       */ 
/*                                            function                    */ 
/*                                                                        */ 
/*  CALLED BY                                                             */ 
/*                                                                        */ 
/*    Application code                                                    */ 
/*                                                                        */ 
/*  RELEASE HISTORY                                                       */ 
/*                                                                        */ 
/*    DATE              NAME                      DESCRIPTION             */
/*                                                                        */
/*  09-30-2020     William E. Lamie         Initial Version 6.1           */
/*                                                                        */
/**************************************************************************/
UINT  _txe_thread_terminate(TX_THREAD *thread_ptr)
{

UINT        status;


    /* Check for an invalid thread pointer.  */
    if (thread_ptr == TX_NULL)
    {

        /* Thread pointer is invalid, return appropriate error code.  */
        status =  TX_THREAD_ERROR;
    }
    
    /* Now check for invalid thread ID.  */
    else if (thread_ptr -> tx_thread_id != TX_THREAD_ID)
    {

        /* Thread pointer is invalid, return appropriate error code.  */
        status =  TX_THREAD_ERROR;
    }

    /* Check for invalid caller of this function.  */
    else if (TX_THREAD_GET_SYSTEM_STATE() != ((ULONG) 0))
    {

        /* Invalid caller of this function, return appropriate error code.  */
        status =  TX_CALLER_ERROR;
    }
    else
    {

        /* Call actual thread terminate function.  */
        status =  _tx_thread_terminate(thread_ptr);
    }

    /* Return completion status.  */
    return(status);
}

