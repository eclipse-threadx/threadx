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
#include "tx_posix.h"   /* Posix API */
#include "px_int.h"     /* Posix helper functions */


/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    sem_init                                             PORTABLE C     */
/*                                                           6.1.7        */
/*  AUTHOR                                                                */
/*                                                                        */
/*    William E. Lamie, Microsoft Corporation                             */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function initializes a nameless sempaphore.                    */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    sem                           Semaphore pointer                     */
/*    pshared                       Shared semphore (TRUE/FALSE)          */
/*    value                         Initial value                         */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    EINVAL                        if error occurs                       */
/*    NO_ERROR                      successful                            */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    tx_semaphore_create           creates actual THreadx semaphore      */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    POSIX internal Code                                                 */
/*                                                                        */
/*  RELEASE HISTORY                                                       */
/*                                                                        */
/*    DATE              NAME                      DESCRIPTION             */
/*                                                                        */
/*  06-02-2021     William E. Lamie         Initial Version 6.1.7         */
/*                                                                        */
/**************************************************************************/
INT sem_init(sem_t *sem , INT pshared, UINT value)
{
    INT result;

    if((sem==NULL)||(value > SEM_VALUE_MAX)||(sem->in_use==TX_TRUE))
    {
        result = EINVAL; /* general error */

    }
    else 
    {
        if(tx_semaphore_create(&(sem->sem),"",value)) result = EINVAL;
        else
        {            /* initialize semaphore fields */
            sem->count = 0;
            sem->in_use = TX_TRUE;
            sem->psemId = 0;
            sem->refCnt = value;
            sem->sem_name = "";
            sem->unlink_flag =TX_FALSE;

            result = NO_ERROR;
        }
    }


    return result;
}
