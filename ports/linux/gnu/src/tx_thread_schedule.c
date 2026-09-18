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

// Portions of this file were generated with AI assistance.


/**************************************************************************/
/**************************************************************************/
/**                                                                       */
/** ThreadX Component                                                     */
/**                                                                       */
/**   Thread                                                              */
/**                                                                       */
/**************************************************************************/
/**************************************************************************/


#define TX_SOURCE_CODE


/* Include necessary system files.  */

#include "tx_api.h"
#include "tx_thread.h"
#include "tx_timer.h"
#include <stdio.h>
#include <errno.h>

extern sem_t _tx_linux_timer_semaphore;
extern sem_t _tx_linux_isr_semaphore;
extern UINT _tx_linux_timer_waiting;
extern pthread_t _tx_linux_timer_id;
/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _tx_thread_schedule                                 Linux/GNU       */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    William E. Lamie, Microsoft Corporation                             */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function waits for a thread control block pointer to appear in */
/*    the _tx_thread_execute_ptr variable.  Once a thread pointer appears */
/*    in the variable, the corresponding thread is resumed.               */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    None                                                                */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    None                                                                */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    tx_linux_mutex_lock                                                 */
/*    tx_linux_mutex_unlock                                               */
/*    _tx_linux_debug_entry_insert                                        */
/*    _tx_linux_thread_resume                                             */
/*    tx_linux_sem_post                                                   */
/*    sem_trywait                                                         */
/*    tx_linux_sem_wait                                                   */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    _tx_initialize_kernel_enter          ThreadX entry function         */
/*                                                                        */
/**************************************************************************/
VOID   _tx_thread_schedule(VOID)
{
struct timespec ts;

    /* Set timer. */
    ts.tv_sec = 0;
    ts.tv_nsec = 200000;

    /* Loop forever.  */
    while(1)
    {

        /* Wait for a thread to execute and all ISRs to complete.  */
        while(1)
        {

            /* Lock Linux mutex.  */
            tx_linux_mutex_lock(_tx_linux_mutex);

            /* Determine if there is a thread ready to execute AND all ISRs
               are complete.  */
            if ((_tx_thread_execute_ptr != TX_NULL) && (_tx_thread_system_state == 0))
            {

                /* Get out of this loop and schedule the thread!  */
                break;
            }
            else
            {

                /* Unlock linux mutex. */
                tx_linux_mutex_unlock(_tx_linux_mutex);

                /* Don't waste all the processor time here in the master thread...  */
#ifdef TX_LINUX_NO_IDLE_ENABLE
                while(!sem_trywait(&_tx_linux_timer_semaphore));
                tx_linux_sem_post(&_tx_linux_timer_semaphore);
                /*nanosleep(&ts, &ts);*/

                clock_gettime(CLOCK_REALTIME, &ts);
                ts.tv_nsec += 200000;
                if (ts.tv_nsec > 1000000000)
                {
                    ts.tv_nsec -= 1000000000;
                    ts.tv_sec++;
                }
                sem_timedwait(&_tx_linux_semaphore_no_idle, &ts);
#else
                nanosleep(&ts, &ts);
#endif /* TX_LINUX_NO_IDLE_ENABLE */
            }
        }

        /* Yes! We have a thread to execute. Note that the critical section is already
           active from the scheduling loop above.  */

        /* Setup the current thread pointer.  */
        _tx_thread_current_ptr =  _tx_thread_execute_ptr;

        /* Increment the run count for this thread.  */
        _tx_thread_current_ptr -> tx_thread_run_count++;

        /* Setup time-slice, if present.  */
        _tx_timer_time_slice =  _tx_thread_current_ptr -> tx_thread_time_slice;

        /* Determine how the thread was suspended.  */
        if (_tx_thread_current_ptr -> tx_thread_linux_suspension_type)
        {

            /* Debug entry.  */
            _tx_linux_debug_entry_insert("SCHEDULE-resume_thread", __FILE__, __LINE__);

            /* Pseudo interrupt suspension.  The thread is not waiting on
               its run semaphore.  */
            _tx_linux_thread_resume(_tx_thread_current_ptr -> tx_thread_linux_thread_id);
        }
        else
        {

            /* Debug entry.  */
            _tx_linux_debug_entry_insert("SCHEDULE-release_sem", __FILE__, __LINE__);

            /* Make sure semaphore is 0. */
            while(!sem_trywait(&_tx_thread_current_ptr -> tx_thread_linux_thread_run_semaphore));

            /* Let the thread run again by releasing its run semaphore.  */
            tx_linux_sem_post(&_tx_thread_current_ptr -> tx_thread_linux_thread_run_semaphore);

            /* Block timer ISR. */
            if(_tx_linux_timer_waiting)
            {

                /* It is woken up by timer ISR. */
                /* Let ThreadX thread wake up first. */
                tx_linux_sem_wait(&_tx_linux_semaphore);

                /* Wake up timer ISR. */
                tx_linux_sem_post_nolock(&_tx_linux_isr_semaphore);
            }
            else
            {

                /* It is woken up by TX_THREAD. */
                /* Suspend timer thread and let ThreadX thread wake up first. */
                _tx_linux_thread_suspend(_tx_linux_timer_id);
                tx_linux_sem_wait(&_tx_linux_semaphore);
                _tx_linux_thread_resume(_tx_linux_timer_id);

            }
        }

        /* Unlock linux mutex. */
        tx_linux_mutex_unlock(_tx_linux_mutex);

        /* Debug entry.  */
        _tx_linux_debug_entry_insert("SCHEDULE-self_suspend_sem", __FILE__, __LINE__);

        /* Now suspend the main thread so the application thread can run.  */
        tx_linux_sem_wait(&_tx_linux_semaphore);

        /* Debug entry.  */
        _tx_linux_debug_entry_insert("SCHEDULE-wake_up", __FILE__, __LINE__);

    }
}

/* Define the ThreadX Linux mutex lock function.  The wait is timed and retried
   rather than left to pthread_mutex_lock, because a thread can be signalled into
   the port's suspend handler while it is parked on this mutex.  That handler does
   not return until the thread is resumed, so the wake-up the next unlock sends is
   delivered to a thread that never retries and is lost.  Any other thread parked
   on the mutex then waits on a mutex that is free.  Retrying on a timeout costs
   nothing when the mutex is handed over normally, and turns that lost wake-up into
   a delay of at most the retry period.  */

void  _tx_linux_mutex_lock_retry(pthread_mutex_t *mutex)
{

INT             linux_status;
struct timespec ts;


    do
    {

        /* Set the deadline for this attempt.  */
        clock_gettime(CLOCK_REALTIME, &ts);
        ts.tv_nsec =  ts.tv_nsec + TX_LINUX_MUTEX_RETRY_NSEC;
        if (ts.tv_nsec >= 1000000000)
        {

            ts.tv_nsec =  ts.tv_nsec - 1000000000;
            ts.tv_sec++;
        }

        linux_status =  pthread_mutex_timedlock(mutex, &ts);

        /* Anything but the deadline expiring is a real failure to obtain the
           mutex, so stop retrying.  */
        if ((linux_status != 0) && (linux_status != ETIMEDOUT))
        {

            break;
        }

    } while (linux_status != 0);
}


void _tx_thread_delete_port_completion(TX_THREAD *thread_ptr, UINT tx_saved_posture)
{
INT             linux_status;
sem_t           *threadrunsemaphore;
pthread_t       thread_id;
struct          timespec ts;

    thread_id = thread_ptr -> tx_thread_linux_thread_id;
    threadrunsemaphore = &(thread_ptr -> tx_thread_linux_thread_run_semaphore);
    ts.tv_sec = 0;
    ts.tv_nsec = 1000000;
    TX_RESTORE
    do
    {
        linux_status = pthread_cancel(thread_id);
        if(linux_status != EAGAIN)
        {
            break;
        }
        _tx_linux_thread_resume(thread_id);
        tx_linux_sem_post(threadrunsemaphore);
        nanosleep(&ts, &ts);
    } while (1);
    pthread_join(thread_id, NULL);
    sem_destroy(threadrunsemaphore);
    TX_DISABLE
}

void _tx_thread_reset_port_completion(TX_THREAD *thread_ptr, UINT tx_saved_posture)
{
INT             linux_status;
sem_t           *threadrunsemaphore;
pthread_t       thread_id;
struct          timespec ts;

    thread_id = thread_ptr -> tx_thread_linux_thread_id;
    threadrunsemaphore = &(thread_ptr -> tx_thread_linux_thread_run_semaphore);
    ts.tv_sec = 0;
    ts.tv_nsec = 1000000;
    TX_RESTORE
    do
    {
        linux_status = pthread_cancel(thread_id);
        if(linux_status != EAGAIN)
        {
            break;
        }
        _tx_linux_thread_resume(thread_id);
        tx_linux_sem_post(threadrunsemaphore);
        nanosleep(&ts, &ts);
    } while (1);
    pthread_join(thread_id, NULL);
    sem_destroy(threadrunsemaphore);
    TX_DISABLE
}
