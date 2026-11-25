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
#pragma comment (lib, "Synchronization.lib")


static VOID  _tx_win32_dispatch_wait(TX_THREAD *execute_thread);


/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _tx_thread_context_restore                        Win32/Visual      */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    William E. Lamie, Microsoft Corporation                             */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function restores the interrupt context if it is processing a  */
/*    nested interrupt.  If not, it returns to the interrupt thread if no */
/*    preemption is necessary.  Otherwise, if preemption is necessary or  */
/*    if no thread was running, the function returns to the scheduler.    */
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
/*    ReleaseSemaphore                      Win32 release semaphore       */
/*    ResumeThread                          Win32 resume thread           */
/*    _tx_win32_critical_section_obtain     Obtain critical section       */
/*    _tx_win32_critical_section_release    Release critical section      */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    ISRs                                  Interrupt Service Routines    */
/*                                                                        */
/**************************************************************************/
VOID   _tx_thread_context_restore(VOID)
{
TX_THREAD   *execute_thread;

    /* Enter critical section to ensure other threads are not playing with
       the core ThreadX data structures.  */
    _tx_win32_critical_section_obtain(&_tx_win32_critical_section);

    /* Debug entry.  */
    _tx_win32_debug_entry_insert("CONTEXT_RESTORE", __FILE__, __LINE__);

    /* Decrement the nested interrupt count.  */
    _tx_thread_system_state--;

    /* Pickup the execute thread pointer.  */
    execute_thread =  _tx_thread_execute_ptr;

    /* Determine if this is the first nested interrupt and if a ThreadX
       application thread was running at the time.  */
    if ((!_tx_thread_system_state) && (_tx_thread_current_ptr))
    {

        /* Yes, this is the first and last interrupt processed.  */

        /* Check to see if preemption is required.  */
        if ((_tx_thread_preempt_disable == 0) && (_tx_thread_current_ptr != _tx_thread_execute_ptr))
        {

            /* Preempt the running application thread.  We don't need to suspend the
               application thread since that is done in the context save processing.  */

            /* Indicate that this thread was suspended asynchronously.  */
            _tx_thread_current_ptr -> tx_thread_win32_suspension_type =  1;

            /* Save the remaining time-slice and disable it.  */
            if (_tx_timer_time_slice)
            {

                _tx_thread_current_ptr -> tx_thread_time_slice =  _tx_timer_time_slice;
                _tx_timer_time_slice =  0;
            }

            /* Clear the current thread pointer.  */
            _tx_thread_current_ptr =  TX_NULL;

            /* Wakeup the system thread by setting the system semaphore.  */
            ReleaseSemaphore(_tx_win32_scheduler_semaphore, 1, NULL);
            _tx_win32_scheduler_wake();

            /* If this interrupt made a solicited wakeup ready, let that thread run
               before the interrupt returns.  */
            if ((execute_thread != TX_NULL) &&
                (execute_thread -> tx_thread_win32_suspension_type == 0))
            {

                _tx_win32_dispatch_wait(execute_thread);
            }
        }
        else
        {

            /* Since preemption is not required, resume the interrupted thread.  */
            ResumeThread(_tx_thread_current_ptr -> tx_thread_win32_thread_handle);
        }
    }
    else if ((!_tx_thread_system_state) && (execute_thread != TX_NULL))
    {

        /* This interrupt made a thread ready while the scheduler was idle.  Hold
           the interrupt until a solicited wakeup has started running.  */
        _tx_win32_scheduler_wake();

        if (execute_thread -> tx_thread_win32_suspension_type == 0)
        {

            _tx_win32_dispatch_wait(execute_thread);
        }
    }

    /* Leave Win32 critical section.  */
    _tx_win32_critical_section_release_all(&_tx_win32_critical_section);
}


/* Waits, with the ThreadX lock released, until the scheduler dispatches a thread
   or the thread this interrupt made ready is no longer the next to run.  */
static VOID  _tx_win32_dispatch_wait(TX_THREAD *execute_thread)
{
LONG    dispatch_count;
ULONG   spin_count;


    _tx_win32_isr_waiting_count++;
    dispatch_count =  _tx_win32_dispatch_count;
    _tx_win32_critical_section_release_all(&_tx_win32_critical_section);

    /* Spin briefly, then block, so that several waiting interrupt threads cannot
       keep the lower-priority host thread being started off the processor.  */
    spin_count =  TX_WIN32_HANDOFF_SPIN_COUNT;
    while ((_tx_win32_dispatch_count == dispatch_count) && (_tx_thread_execute_ptr == execute_thread))
    {
        if (spin_count != 0U)
        {
            spin_count--;
            (VOID) SwitchToThread();
        }
        else
        {
            (VOID) WaitOnAddress(&_tx_win32_dispatch_count, &dispatch_count, sizeof(LONG), 1U);
        }
    }

    _tx_win32_critical_section_obtain(&_tx_win32_critical_section);
    _tx_win32_isr_waiting_count--;
}
