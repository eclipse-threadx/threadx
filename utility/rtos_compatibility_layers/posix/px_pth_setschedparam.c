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
/*    pthread_setschedparam                               PORTABLE C      */ 
/*                                                           6.1.7        */
/*  AUTHOR                                                                */
/*                                                                        */
/*    William E. Lamie, Microsoft Corporation                             */
/*                                                                        */
/*  DESCRIPTION                                                           */ 
/*                                                                        */ 
/*    This function changes the scheduling parameters of a pthread.       */
/*                                                                        */
/*                                                                        */
/*  INPUT                                                                 */ 
/*                                                                        */
/*    thread                         POSIX thread ID                      */
/*    policy                         Address of the thread attributes     */
/*    sched_param                    Address of structure to contain the  */
/*                                   returned scheduling parameters       */ 
/*                                                                        */ 
/*                                                                        */ 
/*  OUTPUT                                                                */ 
/*                                                                        */ 
/*     0                             if successful                        */ 
/*     Value                         in case of any error                 */ 
/*                                                                        */ 
/*  CALLS                                                                 */ 
/*                                                                        */ 
/*    posix_tid2tcb                                                       */
/*    posix_tcb2thread                                                    */
/*    tx_thread_priority_change                                           */
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
INT pthread_setschedparam(pthread_t thread, INT policy, const struct sched_param *param)
{
    TX_INTERRUPT_SAVE_AREA
    POSIX_TCB  *thread_tcb;
    TX_THREAD  *TheThread;
    UINT        Tmp;
    ULONG       UTmp;



    thread_tcb=posix_tid2tcb(thread);

    if(thread_tcb==NULL)
    {
        return(ESRCH);
    }
    if (!(( policy == SCHED_FIFO )||(policy== SCHED_RR)))
    {
        return(ENOTSUP);
    }


    TX_DISABLE
    thread_tcb->sched_policy=policy;
    thread_tcb->sched_attr.sched_priority=param->sched_priority;
    thread_tcb->current_priority= param->sched_priority;

    TheThread=posix_tcb2thread(thread_tcb);

    tx_thread_priority_change(TheThread, (TX_LOWEST_PRIORITY - thread_tcb->current_priority + 1),&Tmp);
    thread_tcb->orig_priority=(ULONG) Tmp;

    if(policy==SCHED_RR) thread_tcb->time_slice=SCHED_RR_TIME_SLICE;
    else thread_tcb->time_slice=0;
    tx_thread_time_slice_change(TheThread, thread_tcb->time_slice, &UTmp);

    TX_RESTORE

    return(NO_ERROR);
}





