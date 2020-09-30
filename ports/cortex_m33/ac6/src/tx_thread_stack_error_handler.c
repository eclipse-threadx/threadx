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

/* Define the global function pointer for stack error handling. If a stack error is 
   detected and the application has registered a stack error handler, it will be 
   called via this function pointer.  */

VOID    (*_tx_thread_application_stack_error_handler)(TX_THREAD *thread_ptr);

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _tx_thread_stack_error_handler                    Cortex-M33/AC6    */
/*                                                            6.1         */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Scott Larson, Microsoft Corporation                                 */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function processes stack errors detected during run-time.      */
/*                                                                        */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    thread_ptr                            Thread control block pointer  */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    None                                                                */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _tx_thread_terminate                                                */
/*    _tx_thread_application_stack_error_handler                          */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    ThreadX internal code                                               */
/*                                                                        */
/*  RELEASE HISTORY                                                       */
/*                                                                        */
/*    DATE              NAME                      DESCRIPTION             */
/*                                                                        */
/*  09-30-2020     Scott Larson             Initial Version 6.1           */
/*                                                                        */
/**************************************************************************/
VOID  _tx_thread_stack_error_handler(TX_THREAD *thread_ptr)
{

    #ifndef TX_THREAD_NO_TERMINATE_STACK_ERROR
    /* Is there a thread?  */
    if (thread_ptr)
    {
        /* Terminate the current thread.  */
        _tx_thread_terminate(_tx_thread_current_ptr);
    }
    #endif
    
    /* Determine if the application has registered an error handler.  */
    if (_tx_thread_application_stack_error_handler != TX_NULL)
    {

        /* Yes, an error handler is present, simply call the application error handler.  */
        (_tx_thread_application_stack_error_handler)(thread_ptr);
    }

}

