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
/** POSIX wrapper for THREADX                                             */ 
/**                                                                       */
/**                                                                       */
/**                                                                       */
/**************************************************************************/
/**************************************************************************/

/* Include necessary system files.  */

#include "tx_api.h"    /* Threadx API */
#include "pthread.h"  /* Posix API */
#include "tx_posix.h"  /* Posix API */
#include "px_int.h"    /* Posix helper functions */



/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    sched_yield                                         PORTABLE C      */
/*                                                           6.1.7        */
/*  AUTHOR                                                                */
/*                                                                        */
/*    William E. Lamie, Microsoft Corporation                             */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This routine forces the running thread to relinquish the CPU        */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    void                                 .                              */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    OK                         If successful                            */
/*    ERROR                      IF fails                                 */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    posix_in_thread_context    Checks thread is called from its context.*/
/*    tx_thread_relinquish       Relinquishes processor control           */
/*    posix_internal_error       Returns generic error                    */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application Code                                                    */
/*                                                                        */
/*  RELEASE HISTORY                                                       */
/*                                                                        */
/*    DATE              NAME                      DESCRIPTION             */
/*                                                                        */
/*  06-02-2021     William E. Lamie         Initial Version 6.1.7         */
/*                                                                        */
/**************************************************************************/
INT sched_yield(VOID)
{

    /* Make sure we're calling this routine from a thread context.  */
    if (!posix_in_thread_context())
    {
        /* return POSIX error.  */
        posix_internal_error(444);

        /* return error.  */
        return (ERROR);
    }

    /* This relinquishes the CPU.  */
    tx_thread_relinquish();
    return(OK);
}
