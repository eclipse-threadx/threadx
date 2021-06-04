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
/*    sched.h                                             PORTABLE C      */
/*                                                           6.x          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    William E. Lamie, Microsoft Corporation                             */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This file defines the constants, structures, etc.needed to          */
/*    implement the Evacuation Kit for POSIX Users (POSIX)                */
/*                                                                        */
/*                                                                        */
/*  RELEASE HISTORY                                                       */
/*                                                                        */
/*    DATE              NAME                      DESCRIPTION             */
/*                                                                        */
/*  xx-xx-xxxx     William E. Lamie         Initial Version 6.x           */
/*                                                                        */
/**************************************************************************/

#ifndef _SCHED_H
#define _SCHED_H


struct sched_param 
{
    INT     sched_priority;
};


#define   SCHED_FIFO                    0
#define   SCHED_RR                      1
#define   SCHED_OTHER                   SCHED_RR

INT                   sched_get_priority_max(INT policy);
INT                   sched_get_priority_min(INT policy);

INT    sched_yield(void);

#endif
