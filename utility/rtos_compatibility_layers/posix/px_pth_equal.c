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
/*    pthread_equal                                       PORTABLE C      */
/*                                                           6.1.7        */
/*  AUTHOR                                                                */
/*                                                                        */
/*    William E. Lamie, Microsoft Corporation                             */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*                                                                        */
/*    This function compares two pthread handles for equality.            */
/*                                                                        */
/*                                                                        */   
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    thread1                     pthread handle to the first thread      */
/*    thread2                     pthread handle to the second thread     */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    0                           The pthread handles do not refer to the */
/*                                same thread                             */
/*    1                           The pthread handles refer to the same   */
/*                                thread                                  */   
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
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
INT pthread_equal(pthread_t thread1, pthread_t thread2)
{
    if (thread1 == thread2)

        return(1);
    else
        return(0);
}
