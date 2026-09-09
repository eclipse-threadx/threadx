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
/** POSIX wrapper for THREADX                                             */
/**                                                                       */
/**                                                                       */
/**                                                                       */
/**************************************************************************/
/**************************************************************************/

/* Include necessary system files.  */

#include "tx_api.h"    /* Threadx API */
#include "pthread.h"  /* Posix API */
#include "px_int.h"    /* Posix helper functions */


/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    pthread_self                                        PORTABLE C      */
/*                                                           6.1.7        */
/*  AUTHOR                                                                */
/*                                                                        */
/*    William E. Lamie, Microsoft Corporation                             */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*                                                                        */
/*    This function returns thread ID of the calling pthread        .     */
/*                                                                        */
/*                                                                        */
/*                                                                        */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*  Nothing                                                               */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    thread_ID               pthread handle of the calling thread        */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application Code                                                    */
/*                                                                        */
/**************************************************************************/
pthread_t pthread_self(VOID)
{

TX_THREAD   *thread_ptr;
pthread_t    thread_ID;

    /* Get the thread identifier of the currently running thread */
    thread_ptr = tx_thread_identify();

    /* Convert thread identifier to posix thread ID */

    thread_ID = posix_thread2tid(thread_ptr);

    /* Only a pthread has the signal fields read below: they belong to the
       POSIX_TCB that surrounds the thread, and reading them through anything
       else runs off the end of a plain TX_THREAD.  A zero ID means the caller
       is not a pthread, either because it is a ThreadX thread created directly
       or because there is no current thread at all, so leave it alone.  */
    if (thread_ID != (pthread_t) 0)
    {

        /* Determine if this thread is actually the signal thread helper.  */
        if (((POSIX_TCB *) thread_ptr) -> signals.signal_handler)
        {

            /* Yes, override the thread_ID with the non-signal thread ID.  */
            thread_ID =  (pthread_t) ((POSIX_TCB *) thread_ptr) -> signals.base_thread_ptr;
        }
    }


    /* All done.  */
    return(thread_ID);
}
