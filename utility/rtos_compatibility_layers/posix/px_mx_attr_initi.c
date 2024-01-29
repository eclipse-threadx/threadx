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
#include "px_int.h"    /* Posix helper functions */


/**************************************************************************/ 
/*                                                                        */ 
/*  FUNCTION                                               RELEASE        */ 
/*                                                                        */ 
/*    pthread_mutexattr_init                              PORTABLE C      */ 
/*                                                           6.1.7        */
/*  AUTHOR                                                                */
/*                                                                        */
/*    William E. Lamie, Microsoft Corporation                             */
/*                                                                        */
/*  DESCRIPTION                                                           */ 
/*                                                                        */ 
/*    This function initializes a pthread mutex attributes object to      */
/*    default values, if the object is already created this call will     */
/*    return an error.                                                    */
/*                                                                        */
/*  INPUT                                                                 */ 
/*                                                                        */
/*    attr                           Pointer to a mutex attributes        */
/*                                                                        */ 
/*  OUTPUT                                                                */ 
/*                                                                        */ 
/*     0                             If successful                        */ 
/*     Value                         In case of any error                 */ 
/*                                                                        */ 
/*  CALLS                                                                 */ 
/*                                                                        */ 
/*    posix_allocate_pthread_mutexattr  Get a new mutexattr object        */ 
/*    set_default_mutexattr             Set mutexattr with default values */
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
INT pthread_mutexattr_init(pthread_mutexattr_t *attr)
{

TX_INTERRUPT_SAVE_AREA


    /* Disable interrupts.  */ 
    TX_DISABLE

    /* Check this attributes object exists ? */
    if (attr->in_use  == TX_TRUE)
    {
        posix_errno = EINVAL;
	    posix_set_pthread_errno(EINVAL);
        TX_RESTORE
        return (EINVAL);
    }
    attr->in_use = TX_TRUE;
    set_default_mutexattr(attr);
    TX_RESTORE
    return(OK);
}
