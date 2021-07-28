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
/*  pthread_cond_destroy                                  PORTABLE C      */ 
/*                                                           6.x          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    William E. Lamie, Microsoft Corporation                             */
/*                                                                        */
/*  DESCRIPTION                                                           */ 
/*                                                                        */ 
/*    The pthread_cond_destroy function shall destroy the given condition */
/*    variable specified by cond; the object becomes, in effect,          */
/*    uninitialized.                                                      */
/*    A destroyed condition variable object can be reinitialized using    */
/*    pthread_cond_init;referencing the object after it has been destroyed*/
/*    returns error.                                                      */
/*                                                                        */
/*  INPUT                                                                 */ 
/*                                                                        */
/*     cond                               condition variable object       */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */                                  
/*     OK                                 If successful                   */
/*     EINVAL                             If error                        */ 
/*                                                                        */
/*  CALLS                                                                 */ 
/*                                                                        */
/*     tx_semaphore_delete                Deletes a semaphore internal to */
/*                                        to the cond variable            */
/*                                                                        */
/*  CALLED BY                                                             */ 
/*                                                                        */ 
/*    Application Code                                                    */ 
/*                                                                        */ 
/*  RELEASE HISTORY                                                       */ 
/*                                                                        */ 
/*    DATE              NAME                      DESCRIPTION             */
/*                                                                        */
/*  xx-xx-xxxx     William E. Lamie         Initial Version 6.x           */
/*                                                                        */
/**************************************************************************/
INT    pthread_cond_destroy(pthread_cond_t *cond)
{

TX_SEMAPHORE     *semaphore_ptr;
UINT              status;
    
    if (cond->in_use == TX_FALSE)
    {
        posix_errno = EINVAL;
        posix_set_pthread_errno(EINVAL);
        return(EINVAL);
    }   
    else
    {
        cond->in_use = TX_FALSE;
        /* Get the condition variable's internal semaphore.  */
        /* Simply convert the Condition variable control block into a semaphore  a cast */ 
        semaphore_ptr = (&( cond->cond_semaphore ));
        status = tx_semaphore_delete(semaphore_ptr);
        if (status != TX_SUCCESS)
        {
            posix_errno = EINVAL;
	        posix_set_pthread_errno(EINVAL);
            return(EINVAL);
        }    
        return(OK);
    }   
}

