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

#include "tx_api.h"     /* Threadx API */
#include "pthread.h"    /* Posix API */
#include "px_int.h"     /* Posix helper functions */


/**************************************************************************/ 
/*                                                                        */ 
/*  FUNCTION                                               RELEASE        */ 
/*                                                                        */ 
/*    pthread_mutex_destroy                               PORTABLE C      */ 
/*                                                           6.1.7        */
/*  AUTHOR                                                                */
/*                                                                        */
/*    William E. Lamie, Microsoft Corporation                             */
/*                                                                        */
/*  DESCRIPTION                                                           */ 
/*                                                                        */ 
/*    This function shall destroy the mutex object referenced by mutex;   */
/*    the mutex object becomes,in effect, uninitialized.A destroyed mutex */
/*    object can be reinitialized using pthread_mutex_init()              */
/*    It shall be safe to destroy an initialized mutex that is unlocked.  */
/*    Attempting to destroy a locked mutex results in undefined behavior. */
/*                                                                        */
/*  INPUT                                                                 */ 
/*                                                                        */
/*    mutex                          Address of the mutex                 */
/*                                                                        */ 
/*  OUTPUT                                                                */ 
/*                                                                        */ 
/*     0                             If successful                        */ 
/*     Value                         In case of any error                 */ 
/*                                                                        */ 
/*  CALLS                                                                 */ 
/*                                                                        */ 
/*    tx_mutex_delete                ThreadX Mutex service                */ 
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
INT pthread_mutex_destroy(pthread_mutex_t *mutex)
{

TX_INTERRUPT_SAVE_AREA

TX_MUTEX       *mutex_ptr;
INT             status,retval;
        
    /* Disable interrupts.  */
    TX_DISABLE

    mutex_ptr = (TX_MUTEX*) mutex;

    status = tx_mutex_delete(mutex_ptr);
    if (status == TX_SUCCESS)
    {
        mutex->in_use = TX_FALSE;
        retval = OK;    
    }
    else
    {
        posix_errno  = EINVAL;
	    posix_set_pthread_errno(EINVAL);
        retval = EINVAL;
    }

    TX_RESTORE
    return(retval);
}
