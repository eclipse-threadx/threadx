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
#include "tx_thread.h"  /* Internal ThreadX thread management.  */

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    sigaddset                                           PORTABLE C      */
/*                                                           6.x          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    William E. Lamie, Microsoft Corporation                             */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function adds the individual signal specified by the signo to  */
/*    the signal set pointed to by set.                                   */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    set                            Pointer to set of signals            */
/*    signo                          Signal                               */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    OK                        If successful                             */
/*    ERROR                     If error occurs                           */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    posix_set_pthread_errno                                             */
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
int   sigaddset(sigset_t *set, int signo)
{

    /* Determine if the desired signal is valid.  */
    if ((signo < 0) || (signo > SIGRTMAX))
    {
    
        /* Return an error.  */
        posix_set_pthread_errno(EINVAL);
        return(ERROR);
    }

    /* All is okay, set the appropriate signal bit.  */
    set -> signal_set =  set -> signal_set | (((ULONG) 1) << signo);

    /* Return success!  */
    return(OK);    
}
