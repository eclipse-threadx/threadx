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

#include "tx_api.h"     /* Threadx API */
#include "pthread.h"    /* Posix API */
#include "px_int.h"     /* Posix helper functions */


/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    clock_getres                                        PORTABLE C      */
/*                                                           6.1.7        */
/*  AUTHOR                                                                */
/*                                                                        */
/*    William E. Lamie, Microsoft Corporation                             */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This subroutine returns the Clock resolution of the Threadx real    */
/*    time clock in nanoseconds                                           */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    clockid_t, tspec                                                    */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*     error code                                                         */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application Code                                                    */
/*                                                                        */
/**************************************************************************/
INT clock_getres(clockid_t t, struct timespec * tspec)
{

 if(t==CLOCK_REALTIME)
 {
     if(tspec) tspec->tv_nsec= NANOSECONDS_IN_CPU_TICK;
     return(OK);
 }
 else return(EINVAL);

}
