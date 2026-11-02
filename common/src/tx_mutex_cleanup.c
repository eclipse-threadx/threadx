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
/** ThreadX Component                                                     */
/**                                                                       */
/**   Mutex                                                               */
/**                                                                       */
/**************************************************************************/
/**************************************************************************/

#define TX_SOURCE_CODE


/* Include necessary system files.  */

#include "tx_api.h"
#include "tx_thread.h"
#include "tx_mutex.h"


/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _tx_mutex_cleanup                                   PORTABLE C      */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    William E. Lamie, Microsoft Corporation                             */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function processes mutex timeout and thread terminate          */
/*    actions that require the mutex data structures to be cleaned        */
/*    up.                                                                 */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    thread_ptr                        Pointer to suspended thread's     */
/*                                        control block                   */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    None                                                                */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _tx_thread_system_resume          Resume thread service             */
/*    _tx_thread_system_ni_resume       Non-interruptable resume thread   */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    _tx_thread_timeout                Thread timeout processing         */
/*    _tx_thread_terminate              Thread terminate processing       */
/*    _tx_thread_wait_abort             Thread wait abort processing      */
/*                                                                        */
/**************************************************************************/
VOID  _tx_mutex_cleanup(TX_THREAD  *thread_ptr, ULONG suspension_sequence)
{

#ifndef TX_NOT_INTERRUPTABLE
TX_INTERRUPT_SAVE_AREA
#endif

TX_MUTEX            *mutex_ptr;
UINT                suspended_count;
TX_THREAD           *next_thread;
TX_THREAD           *previous_thread;


#ifndef TX_NOT_INTERRUPTABLE

    /* Disable interrupts to remove the suspended thread from the mutex.  */
    TX_DISABLE

    /* Determine if the cleanup is still required.  */
    if (thread_ptr -> tx_thread_suspend_cleanup == &(_tx_mutex_cleanup))
    {

        /* Check for valid suspension sequence.  */
        if (suspension_sequence == thread_ptr -> tx_thread_suspension_sequence)
        {

            /* Setup pointer to mutex control block.  */
            mutex_ptr =  TX_VOID_TO_MUTEX_POINTER_CONVERT(thread_ptr -> tx_thread_suspend_control_block);

            /* Check for NULL mutex pointer.  */
            if (mutex_ptr != TX_NULL)
            {

                /* Determine if the mutex ID is valid.  */
                if (mutex_ptr -> tx_mutex_id == TX_MUTEX_ID)
                {

                    /* Determine if there are any thread suspensions.  */
                    if (mutex_ptr -> tx_mutex_suspended_count != TX_NO_SUSPENSIONS)
                    {
#else

                        /* Setup pointer to mutex control block.  */
                        mutex_ptr =  TX_VOID_TO_MUTEX_POINTER_CONVERT(thread_ptr -> tx_thread_suspend_control_block);
#endif

                        /* Yes, we still have thread suspension!  */

                        /* Clear the suspension cleanup flag.  */
                        thread_ptr -> tx_thread_suspend_cleanup =  TX_NULL;

                        /* Decrement the suspension count.  */
                        mutex_ptr -> tx_mutex_suspended_count--;

                        /* Pickup the suspended count.  */
                        suspended_count =  mutex_ptr -> tx_mutex_suspended_count;

                        /* Remove the suspended thread from the list.  */

                        /* See if this is the only suspended thread on the list.  */
                        if (suspended_count == TX_NO_SUSPENSIONS)
                        {

                            /* Yes, the only suspended thread.  */

                            /* Update the head pointer.  */
                            mutex_ptr -> tx_mutex_suspension_list =  TX_NULL;
                        }
                        else
                        {

                            /* At least one more thread is on the same suspension list.  */

                            /* Update the links of the adjacent threads.  */
                            next_thread =                                   thread_ptr -> tx_thread_suspended_next;
                            previous_thread =                               thread_ptr -> tx_thread_suspended_previous;
                            next_thread -> tx_thread_suspended_previous =   previous_thread;
                            previous_thread -> tx_thread_suspended_next =   next_thread;

                            /* Determine if we need to update the head pointer.  */
                            if (mutex_ptr -> tx_mutex_suspension_list == thread_ptr)
                            {

                                /* Update the list head pointer.  */
                                mutex_ptr -> tx_mutex_suspension_list =         next_thread;
                            }
                        }

                        /* Now we need to determine if this cleanup is from a terminate, timeout,
                           or from a wait abort.  */
                        if (thread_ptr -> tx_thread_state == TX_MUTEX_SUSP)
                        {

                            /* Timeout condition and the thread still suspended on the mutex.
                               Setup return error status and resume the thread.  */

#ifdef TX_MUTEX_ENABLE_PERFORMANCE_INFO

                            /* Increment the total timeouts counter.  */
                            _tx_mutex_performance_timeout_count++;

                            /* Increment the number of timeouts on this semaphore.  */
                            mutex_ptr -> tx_mutex_performance_timeout_count++;
#endif

                            /* Setup return status.  */
                            thread_ptr -> tx_thread_suspend_status =  TX_NOT_AVAILABLE;

#ifdef TX_NOT_INTERRUPTABLE

                            /* Resume the thread!  */
                            _tx_thread_system_ni_resume(thread_ptr);
#else

                            /* Temporarily disable preemption.  */
                            _tx_thread_preempt_disable++;

                            /* Restore interrupts.  */
                            TX_RESTORE

                            /* Resume the thread!  */
                            _tx_thread_system_resume(thread_ptr);

                            /* Disable interrupts.  */
                            TX_DISABLE
#endif
                        }
#ifndef TX_NOT_INTERRUPTABLE
                    }
                }
            }
        }
    }

    /* Restore interrupts.  */
    TX_RESTORE
#endif
}


/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _tx_mutex_thread_release                            PORTABLE C      */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    William E. Lamie, Microsoft Corporation                             */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function releases all mutexes owned by the thread. This        */
/*    function is called when the thread completes or is terminated.      */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    thread_ptr                        Pointer to thread's control block */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    None                                                                */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _tx_mutex_put                     Release the mutex                 */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    _tx_thread_shell_entry            Thread completion processing      */
/*    _tx_thread_terminate              Thread terminate processing       */
/*                                                                        */
/*  RELEASE HISTORY                                                       */
/*                                                                        */
/*    DATE              NAME                      DESCRIPTION             */
/*                                                                        */
/*  05-19-2020     William E. Lamie         Initial Version 6.0           */
/*  09-30-2020     Yuxin Zhou               Modified comment(s),          */
/*                                            resulting in version 6.1    */
/*                                                                        */
/**************************************************************************/
VOID  _tx_mutex_thread_release(TX_THREAD  *thread_ptr)
{

TX_INTERRUPT_SAVE_AREA

TX_MUTEX    *mutex_ptr;
#ifdef TX_MISRA_ENABLE
UINT        status;
#endif


    /* Disable interrupts.  */
    TX_DISABLE

    /* Temporarily disable preemption.  */
    _tx_thread_preempt_disable++;

    /* Loop to look at all the mutexes.  */
    do
    {

        /* Pickup the mutex head pointer.  */
        mutex_ptr =  thread_ptr -> tx_thread_owned_mutex_list;

        /* Determine if there is a mutex.  */
        if (mutex_ptr != TX_NULL)
        {

            /* Yes, set the ownership count to 1.  */
            mutex_ptr -> tx_mutex_ownership_count =  ((UINT) 1);

            /* Restore interrupts.   */
            TX_RESTORE

#ifdef TX_MISRA_ENABLE
            /* Release the mutex.  */
            do
            {
                status =  _tx_mutex_put(mutex_ptr);
            } while (status != TX_SUCCESS);
#else
            _tx_mutex_put(mutex_ptr);
#endif

            /* Disable interrupts.  */
            TX_DISABLE

            /* Move to the next mutex.  */
            mutex_ptr =  thread_ptr -> tx_thread_owned_mutex_list;
        }
    } while (mutex_ptr != TX_NULL);

    /* Restore preemption.  */
    _tx_thread_preempt_disable--;

    /* Restore interrupts.  */
    TX_RESTORE
}



/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _tx_mutex_inherit_priority_update                   PORTABLE C      */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function brings the priority inheritance of a mutex and of     */
/*    its owner up to date after the set of threads waiting on the mutex, */
/*    or the priority of one of them, has changed other than through      */
/*    tx_mutex_get or tx_mutex_put. The highest priority waiting on the   */
/*    mutex is recomputed from its suspension list, the owner's inherited */
/*    priority is recomputed from every priority inheritance mutex it     */
/*    owns, and the owner's priority is changed if it differs from the    */
/*    higher of that and its own user priority.                           */
/*                                                                        */
/*    Unless TX_NOT_INTERRUPTABLE is defined, the caller must have        */
/*    preemption disabled and interrupts enabled, as for                  */
/*    _tx_mutex_priority_change. Otherwise interrupts must be disabled.   */
/*                                                                        */
/*    The mutex must support priority inheritance. Its suspension list    */
/*    is reordered by priority here, which would break the FIFO order     */
/*    the waiters of any other mutex are served in.                       */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    mutex_ptr                         Pointer to mutex control block    */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    None                                                                */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _tx_mutex_prioritize              Move highest priority waiter to   */
/*                                        the front of the list           */
/*    _tx_mutex_priority_change         Change owner's priority           */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    ThreadX internal code                                               */
/*                                                                        */
/**************************************************************************/
VOID  _tx_mutex_inherit_priority_update(TX_MUTEX *mutex_ptr)
{

#ifndef TX_NOT_INTERRUPTABLE
TX_INTERRUPT_SAVE_AREA
#endif

TX_THREAD       *owner_ptr;
TX_THREAD       *suspended_thread;
TX_MUTEX        *next_mutex;
UINT            inheritance_priority;
UINT            new_priority;
#ifdef TX_MISRA_ENABLE
UINT            status;
#endif


    /* Determine if there is more than one thread suspended on the mutex.  */
    if (mutex_ptr -> tx_mutex_suspended_count > ((UINT) 1))
    {

        /* Yes, prioritize the list so the highest priority thread is placed at the
           front of the suspension list.  */
#ifdef TX_MISRA_ENABLE
        do
        {
            status =  _tx_mutex_prioritize(mutex_ptr);
        } while (status != TX_SUCCESS);
#else
        _tx_mutex_prioritize(mutex_ptr);
#endif
    }

#ifndef TX_NOT_INTERRUPTABLE

    /* Disable interrupts.  */
    TX_DISABLE
#endif

    /* Pickup the thread at the front of the suspension list.  */
    suspended_thread =  mutex_ptr -> tx_mutex_suspension_list;

    /* Determine if there still are threads suspended for this mutex.  */
    if (suspended_thread != TX_NULL)
    {

        /* Setup the highest priority thread waiting on this mutex.  */
        mutex_ptr -> tx_mutex_highest_priority_waiting =  suspended_thread -> tx_thread_priority;
    }
    else
    {

        /* No thread is waiting on this mutex.  */
        mutex_ptr -> tx_mutex_highest_priority_waiting =  ((UINT) TX_MAX_PRIORITIES);
    }

    /* Pickup the mutex owner.  */
    owner_ptr =  mutex_ptr -> tx_mutex_owner;

#ifndef TX_NOT_INTERRUPTABLE

    /* Restore interrupts.  */
    TX_RESTORE
#endif

    /* The owner is not NULL.  A thread could only suspend on this mutex while a
       thread owned it, since tx_mutex_get raises the owner's priority when it does,
       and the owner keeps the mutex for as long as this processing can run.  */

    /* Default the inheritance priority to disabled.  */
    inheritance_priority =  ((UINT) TX_MAX_PRIORITIES);

    /* Search the mutexes owned by this thread to determine the highest priority
       it still inherits.  */
    next_mutex =  owner_ptr -> tx_thread_owned_mutex_list;
    while (next_mutex != TX_NULL)
    {

        /* Does this mutex support priority inheritance?  */
        if (next_mutex -> tx_mutex_inherit == TX_TRUE)
        {

            /* Determine if highest priority field of the mutex is higher than the
               inheritance priority so far.  */
            if (next_mutex -> tx_mutex_highest_priority_waiting < inheritance_priority)
            {

                /* Use this priority.  */
                inheritance_priority =  next_mutex -> tx_mutex_highest_priority_waiting;
            }
        }

        /* Move mutex pointer to the next mutex in the list.  */
        next_mutex =  next_mutex -> tx_mutex_owned_next;

        /* Are we at the end of the list?  */
        if (next_mutex == owner_ptr -> tx_thread_owned_mutex_list)
        {

            /* Yes, set the next mutex to NULL.  */
            next_mutex =  TX_NULL;
        }
    }

#ifndef TX_NOT_INTERRUPTABLE

    /* Disable interrupts.  */
    TX_DISABLE
#endif

    /* Set the inherit priority to that of the highest priority thread waiting on
       any mutex the owner holds.  */
    owner_ptr -> tx_thread_inherit_priority =  inheritance_priority;

    /* The owner runs at the higher of its own priority and the inherited one.  */
    new_priority =  owner_ptr -> tx_thread_user_priority;
    if (inheritance_priority < new_priority)
    {

        /* Use the inherited priority.  */
        new_priority =  inheritance_priority;
    }

#ifndef TX_NOT_INTERRUPTABLE

    /* Restore interrupts.  */
    TX_RESTORE
#endif

    /* Determine if the owner's priority has to change.  */
    if (owner_ptr -> tx_thread_priority != new_priority)
    {

        /* Yes, change the priority of the owner.  */
        _tx_mutex_priority_change(owner_ptr, new_priority);
    }
}
