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
/*    _tx_mutex_priority_change                           PORTABLE C      */
/*                                                           6.1.6        */
/*  AUTHOR                                                                */
/*                                                                        */
/*    William E. Lamie, Microsoft Corporation                             */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function changes the priority of the specified thread for the  */
/*    priority inheritance option of the mutex service.                   */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    thread_ptr                            Pointer to thread to suspend  */
/*    new_priority                          New thread priority           */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    None                                                                */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _tx_thread_system_resume          Resume thread                     */
/*    _tx_thread_system_ni_resume       Non-interruptable resume thread   */
/*    _tx_thread_system_suspend         Suspend thread                    */
/*    _tx_thread_system_ni_suspend      Non-interruptable suspend thread  */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    _tx_mutex_get                     Inherit priority                  */
/*    _tx_mutex_put                     Restore previous priority         */
/*                                                                        */
/**************************************************************************/
VOID  _tx_mutex_priority_change(TX_THREAD *thread_ptr, UINT new_priority)
{

#ifndef TX_NOT_INTERRUPTABLE

TX_INTERRUPT_SAVE_AREA
#endif

TX_THREAD       *execute_ptr;
TX_THREAD       *next_execute_ptr;
UINT            original_priority;
#ifndef TX_DISABLE_PREEMPTION_THRESHOLD
ULONG           priority_bit;
#if TX_MAX_PRIORITIES > 32
UINT            map_index;
#endif
#endif



#ifndef TX_NOT_INTERRUPTABLE

    /* Lockout interrupts while the thread is being suspended.  */
    TX_DISABLE
#endif

    /* Determine if this thread is currently ready.  */
    if (thread_ptr -> tx_thread_state != TX_READY)
    {

        /* Change thread priority to the new mutex priority-inheritance priority.  */
        thread_ptr -> tx_thread_priority =  new_priority;

        /* Determine how to setup the thread's preemption-threshold.  */
        if (thread_ptr -> tx_thread_user_preempt_threshold < new_priority)
        {

            /* Change thread preemption-threshold to the user's preemption-threshold.  */
            thread_ptr -> tx_thread_preempt_threshold =  thread_ptr -> tx_thread_user_preempt_threshold;
        }
        else
        {

            /* Change the thread preemption-threshold to the new threshold.  */
            thread_ptr -> tx_thread_preempt_threshold =  new_priority;
        }

#ifndef TX_NOT_INTERRUPTABLE
        /* Restore interrupts.  */
        TX_RESTORE
#endif
    }
    else
    {

        /* Pickup the next thread to execute.  */
        execute_ptr =  _tx_thread_execute_ptr;

        /* Save the original priority.  */
        original_priority =  thread_ptr -> tx_thread_priority;

#ifdef TX_NOT_INTERRUPTABLE

        /* Increment the preempt disable flag.  */
        _tx_thread_preempt_disable++;

        /* Set the state to priority change.  */
        thread_ptr -> tx_thread_state =    TX_PRIORITY_CHANGE;

        /* Call actual non-interruptable thread suspension routine.  */
        _tx_thread_system_ni_suspend(thread_ptr, ((ULONG) 0));

        /* At this point, the preempt disable flag is still set, so we still have
           protection against all preemption.  */

        /* Change thread priority to the new mutex priority-inheritance priority.  */
        thread_ptr -> tx_thread_priority =  new_priority;

        /* Determine how to setup the thread's preemption-threshold.  */
        if (thread_ptr -> tx_thread_user_preempt_threshold < new_priority)
        {

            /* Change thread preemption-threshold to the user's preemption-threshold.  */
            thread_ptr -> tx_thread_preempt_threshold =  thread_ptr -> tx_thread_user_preempt_threshold;
        }
        else
        {

            /* Change the thread preemption-threshold to the new threshold.  */
            thread_ptr -> tx_thread_preempt_threshold =  new_priority;
        }

        /* Resume the thread with the new priority.  */
        _tx_thread_system_ni_resume(thread_ptr);

        /* Decrement the preempt disable flag.  */
        _tx_thread_preempt_disable--;
#else

        /* Increment the preempt disable flag.  */
        _tx_thread_preempt_disable =  _tx_thread_preempt_disable + ((UINT) 2);

        /* Set the state to priority change.  */
        thread_ptr -> tx_thread_state =    TX_PRIORITY_CHANGE;

        /* Set the suspending flag. */
        thread_ptr -> tx_thread_suspending =  TX_TRUE;

        /* Setup the timeout period.  */
        thread_ptr -> tx_thread_timer.tx_timer_internal_remaining_ticks =  ((ULONG) 0);

        /* Restore interrupts.  */
        TX_RESTORE

        /* The thread is ready and must first be removed from the list.  Call the
           system suspend function to accomplish this.  */
        _tx_thread_system_suspend(thread_ptr);

        /* Disable interrupts.  */
        TX_DISABLE

        /* At this point, the preempt disable flag is still set, so we still have
           protection against all preemption.  */

        /* Change thread priority to the new mutex priority-inheritance priority.  */
        thread_ptr -> tx_thread_priority =  new_priority;

        /* Determine how to setup the thread's preemption-threshold.  */
        if (thread_ptr -> tx_thread_user_preempt_threshold < new_priority)
        {

            /* Change thread preemption-threshold to the user's preemption-threshold.  */
            thread_ptr -> tx_thread_preempt_threshold =  thread_ptr -> tx_thread_user_preempt_threshold;
        }
        else
        {

            /* Change the thread preemption-threshold to the new threshold.  */
            thread_ptr -> tx_thread_preempt_threshold =  new_priority;
        }

        /* Restore interrupts.  */
        TX_RESTORE

        /* Resume the thread with the new priority.  */
        _tx_thread_system_resume(thread_ptr);
#endif

        /* Optional processing extension.  */
        TX_MUTEX_PRIORITY_CHANGE_EXTENSION

#ifndef TX_NOT_INTERRUPTABLE

        /* Disable interrupts.  */
        TX_DISABLE
#endif

        /* Pickup the next thread to execute.  */
        next_execute_ptr =  _tx_thread_execute_ptr;

        /* Determine if this thread is not the next thread to execute.  */
        if (thread_ptr != next_execute_ptr)
        {

            /* Make sure the thread is still ready.  */
            if (thread_ptr -> tx_thread_state == TX_READY)
            {

                /* Now check and see if this thread has an equal or higher priority.  */
                if (thread_ptr -> tx_thread_priority <= next_execute_ptr -> tx_thread_priority)
                {

                    /* Now determine if this thread was the previously executing thread.  */
                    if (thread_ptr == execute_ptr)
                    {

                        /* Yes, this thread was previously executing before we temporarily suspended and resumed
                           it in order to change the priority. A lower or same priority thread cannot be the next thread
                           to execute in this case since this thread really didn't suspend.  Simply reset the execute
                           pointer to this thread.  */
                        _tx_thread_execute_ptr =  thread_ptr;

                        /* Determine if we moved to a lower priority. If so, move the thread to the front of its priority list.  */
                        if (original_priority < new_priority)
                        {

                            /* Ensure that this thread is placed at the front of the priority list.  */
                            _tx_thread_priority_list[thread_ptr -> tx_thread_priority] =  thread_ptr;
                        }
                    }
                }
                else
                {

                    /* Now determine if this thread's preemption-threshold needs to be enforced.  */
                    if (thread_ptr -> tx_thread_preempt_threshold < thread_ptr -> tx_thread_priority)
                    {

                        /* Yes, preemption-threshold is in force for this thread. */

                        /* Compare the next thread to execute thread's priority against the thread's preemption-threshold.  */
                        if (thread_ptr -> tx_thread_preempt_threshold <= next_execute_ptr -> tx_thread_priority)
                        {

                            /* We must swap execute pointers to enforce the preemption-threshold of a thread coming out of
                               priority inheritance.  */
                            _tx_thread_execute_ptr =  thread_ptr;

                            /* Determine if we moved to a lower priority. If so, move the thread to the front of its priority list.  */
                            if (original_priority < new_priority)
                            {

                                /* Ensure that this thread is placed at the front of the priority list.  */
                                _tx_thread_priority_list[thread_ptr -> tx_thread_priority] =  thread_ptr;
                            }
                        }

#ifndef TX_DISABLE_PREEMPTION_THRESHOLD

                        else
                        {

                            /* In this case, we need to mark the preempted map to indicate a thread executed above the
                               preemption-threshold.  */

#if TX_MAX_PRIORITIES > 32

                            /* Calculate the index into the bit map array.  */
                            map_index =  (thread_ptr -> tx_thread_priority)/ ((UINT) 32);

                            /* Set the active bit to remember that the preempt map has something set.  */
                            TX_DIV32_BIT_SET(thread_ptr -> tx_thread_priority, priority_bit)
                            _tx_thread_preempted_map_active =  _tx_thread_preempted_map_active | priority_bit;
#endif

                            /* Remember that this thread was preempted by a thread above the thread's threshold.  */
                            TX_MOD32_BIT_SET(thread_ptr -> tx_thread_priority, priority_bit)
                            _tx_thread_preempted_maps[MAP_INDEX] =  _tx_thread_preempted_maps[MAP_INDEX] | priority_bit;
                        }
#endif
                    }
                }
            }
        }

#ifndef TX_NOT_INTERRUPTABLE

        /* Restore interrupts.  */
        TX_RESTORE
#endif
    }
}


#ifdef TX_MUTEX_INHERIT_CHAIN_MAX
/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _tx_mutex_priority_chain                            PORTABLE C      */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function passes an inherited priority along a chain of         */
/*    priority inheritance mutexes. While the thread is waiting on such a */
/*    mutex, the mutex owner inherits the priority and the owner becomes  */
/*    the next thread in the chain. At most TX_MUTEX_INHERIT_CHAIN_MAX    */
/*    mutexes are followed. The walk ends at the first owner already at   */
/*    the priority or higher, so it also ends on a lock cycle.            */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    thread_ptr                        Thread that inherited priority    */
/*    new_priority                      Inherited priority                */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    None                                                                */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _tx_mutex_priority_change         Inherit thread priority           */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    _tx_mutex_get                     Inherit priority                  */
/*                                                                        */
/**************************************************************************/
VOID  _tx_mutex_priority_chain(TX_THREAD *thread_ptr, UINT new_priority)
{

#ifndef TX_NOT_INTERRUPTABLE
TX_INTERRUPT_SAVE_AREA
#endif

TX_MUTEX        *mutex_ptr;
TX_THREAD       *owner_ptr;
UINT            chain_length;


    /* Follow the chain for at most TX_MUTEX_INHERIT_CHAIN_MAX mutexes.  */
    chain_length =  ((UINT) 0);
    while ((thread_ptr != TX_NULL) && (chain_length < ((UINT) TX_MUTEX_INHERIT_CHAIN_MAX)))
    {

        /* Default to the end of the chain.  */
        owner_ptr =  TX_NULL;

#ifndef TX_NOT_INTERRUPTABLE

        /* Disable interrupts.  */
        TX_DISABLE
#endif

        /* Determine if this thread is waiting on a mutex.  */
        if ((thread_ptr -> tx_thread_state == TX_MUTEX_SUSP) && (thread_ptr -> tx_thread_suspend_cleanup != TX_NULL))
        {

            /* Pickup the mutex the thread is waiting on.  */
            mutex_ptr =  TX_VOID_TO_MUTEX_POINTER_CONVERT(thread_ptr -> tx_thread_suspend_control_block);

            /* Determine if priority inheritance is in effect.  */
            if (mutex_ptr -> tx_mutex_inherit == TX_TRUE)
            {

                /* Determine if this is the highest priority to raise for this mutex.  */
                if (mutex_ptr -> tx_mutex_highest_priority_waiting > new_priority)
                {

                    /* Remember this priority.  */
                    mutex_ptr -> tx_mutex_highest_priority_waiting =  new_priority;
                }

                /* Pickup the mutex owner.  */
                owner_ptr =  mutex_ptr -> tx_mutex_owner;

                /* Determine if we have to update inherit priority level of the mutex owner.  */
                if (owner_ptr -> tx_thread_inherit_priority > new_priority)
                {

                    /* Remember the new priority inheritance priority.  */
                    owner_ptr -> tx_thread_inherit_priority =  new_priority;
                }

                /* Determine if the owner already runs at the priority or higher.  */
                if (owner_ptr -> tx_thread_priority <= new_priority)
                {

                    /* Yes, the chain ends here.  */
                    owner_ptr =  TX_NULL;
                }
            }
        }

#ifndef TX_NOT_INTERRUPTABLE

        /* Restore interrupts.  */
        TX_RESTORE
#endif

        /* Determine if there is an owner to raise.  */
        if (owner_ptr != TX_NULL)
        {

            /* Raise the owner's priority to the inherited priority.  */
            _tx_mutex_priority_change(owner_ptr, new_priority);
        }

        /* Move to the next thread in the chain.  */
        thread_ptr =  owner_ptr;
        chain_length++;
    }
}
#endif
