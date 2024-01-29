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
/*    posix_find_sem                                      PORTABLE C      */
/*                                                           6.1.7        */
/*  AUTHOR                                                                */
/*                                                                        */
/*    William E. Lamie, Microsoft Corporation                             */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This routine returns sem descriptor indicating that name of         */
/*    in the semaphore exists.                                            */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    const char * name        Name of the semaphore.                     */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    sem                      If successful                              */
/*    ERROR                    If fails                                   */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*     None                                                               */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    POSIX internal Code                                                 */
/*                                                                        */
/*  RELEASE HISTORY                                                       */
/*                                                                        */
/*    DATE              NAME                      DESCRIPTION             */
/*                                                                        */
/*  06-02-2021      William E. Lamie        Initial Version 6.1.7         */
/*  08-02-2021      Scott Larson            Removed unneeded semicolon,   */
/*                                            resulting in version 6.1.8  */
/*                                                                        */
/**************************************************************************/
sem_t* posix_find_sem(const CHAR * name)
{

sem_t                  *sem;
ULONG                   index;
ULONG                   match;
CHAR                   *dummy_name;
CHAR                   *dummy_sem_name;
ULONG                   namelength;

    /* For checking the name.  */
    for(index = 0,sem = posix_sem_pool;index < SEM_NSEMS_MAX;index ++,sem ++)
    {
        /* Assume the worst case.  */
        match = TX_FALSE;

        dummy_sem_name = sem->sem_name;

        for(namelength = 0 ,dummy_name = (CHAR *)name ; namelength < 10 ;
                  namelength ++, dummy_name++,dummy_sem_name ++)
        {
            /* Copy name.  */
            if(* dummy_name == * dummy_sem_name)
            {
                /* End of the string.  */
                if(* dummy_name == '\0')
                {
                    match = TX_TRUE;
                    break;
                }
            }/*  Copy name.  */
            else
                break;
        }

        /* Stop searching.  */
        if ( match == TX_TRUE)
        {
            break;
        }
    }/* For each semaphore.  */
    if(match == TX_TRUE)
    {
        return(sem);
    }

    /* return NULL.  */
    sem = NULL;
    return(sem);
}
