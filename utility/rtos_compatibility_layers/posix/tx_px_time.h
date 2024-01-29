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
/**   ThreadX Component                                                   */
/**                                                                       */
/**   POSIX Compliancy Wrapper (POSIX)                                    */
/**                                                                       */
/**************************************************************************/
/**************************************************************************/

/**************************************************************************/
/*                                                                        */
/*  EKP DEFINITIONS                                        RELEASE        */
/*                                                                        */
/*    tx_px_time.h                                        PORTABLE C      */
/*                                                           6.1.7        */
/*  AUTHOR                                                                */
/*                                                                        */
/*    William E. Lamie, Microsoft Corporation                             */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This file defines the constants, structures, etc. needed to         */
/*    implement time related functionality for the Evacuation Kit         */
/*    for POSIX Users (POSIX)                                             */
/*                                                                        */
/*                                                                        */
/*  RELEASE HISTORY                                                       */
/*                                                                        */
/*    DATE              NAME                      DESCRIPTION             */
/*                                                                        */
/*  06-02-2021     William E. Lamie         Initial Version 6.1.7         */
/*                                                                        */
/**************************************************************************/

#ifndef _TX_PX_TIME_H
#define _TX_PX_TIME_H

#ifndef _TIME_T
#define _TIME_T
typedef  ULONG  time_t;
#endif


struct timespec 
{
    ULONG     timeout_value;         /* time in terms of CPU ticks */
    time_t    tv_sec;                /* time in terms of Seconds */
    ULONG     tv_nsec;               /* time in terms of nano seconds*/
};

# define    CLOCK_REALTIME  1


INT clock_settime(clockid_t, const struct timespec *);
INT clock_gettime(clockid_t, struct timespec *);
INT clock_getres(clockid_t, struct timespec *);


INT nanosleep(struct timespec *req, struct timespec *rem);
UINT sleep(ULONG seconds);

#endif


