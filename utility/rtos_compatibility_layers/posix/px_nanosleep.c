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

#include "tx_api.h"    /* Threadx API */
#include "pthread.h"  /* Posix API */
#include "px_int.h"    /* Posix helper functions */


/**************************************************************************/ 
/*                                                                        */ 
/*  FUNCTION                                               RELEASE        */ 
/*                                                                        */ 
/*  nanosleep                                              PORTABLE C     */ 
/*                                                           6.1.7        */
/*  AUTHOR                                                                */
/*                                                                        */
/*    William E. Lamie, Microsoft Corporation                             */
/*                                                                        */
/*  DESCRIPTION                                                           */ 
/*                                                                        */
/*    This function shall cause the current thread to be suspended from   */
/*    execution until the time interval specified by the req argument has */
/*    elapsed                                                             */
/*                                                                        */
/*  INPUT                                                                 */ 
/*                                                                        */
/*   req                          Is the number of real-time (as opposed  */
/*                                to CPU-time) seconds and nanoseconds to */
/*                                suspend the calling thread.             */
/*   rem                          Points to a structure to receive the    */
/*                                remaining time if the function is       */
/*                                interrupted by a signal. This pointer   */
/*                                may be NULL.                            */
/*                                                                        */
/*  OUTPUT                                                                */ 
/*                                                                        */ 
/*   zero                         If the function returns because the     */
/*                                requested time has elapsed.             */
/*                                                                        */
/*   -1                           If this functions fails if req argument */
/*                                specified a value less than zero or     */
/*                                greater than or equal to 1 000 million. */
/*                                                                        */
/*                                                                        */
/*  CALLS                                                                 */ 
/*                                                                        */
/*    tx_thread_sleep             ThreadX thread sleep service            */
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
INT nanosleep(struct timespec *req, struct timespec *rem)
{

ULONG    timer_ticks;
    
    /* Check for valid inputs */
    /* The nanosecond value must be greater than zero or less than  1 000 million. */
    if ( (!req) || ((req->tv_nsec) <= 0) || (req->tv_nsec > 999999999 ))      /* 08-11-2005 */

    {
        posix_errno = EINVAL;
        posix_set_pthread_errno(EINVAL);
        return(ERROR);
    }
    
    /* Convert sleep time into Clock ticks */
    /* Also add some padding so that the thread will sleep no less than the
       specified time */
   
    timer_ticks = (ULONG) ( ( req->tv_sec * CPU_TICKS_PER_SECOND ) + ( req->tv_nsec/ NANOSECONDS_IN_CPU_TICK) + 1 );      /* 08-11-2005 */

    /* Now call ThreadX thread sleep service */
    tx_thread_sleep(timer_ticks);
    
    /* Sleep completed */
    if ( rem )                   /* 08-11-2005 */
    {
	rem->tv_nsec =  0;
	rem->tv_sec = 0;         /* 08-11-2005 */
    }	
    return(OK);
    
}
