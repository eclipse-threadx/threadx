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
/** ThreadX Component                                                     */
/**                                                                       */
/**   Module Manager                                                      */
/**                                                                       */
/**************************************************************************/
/**************************************************************************/

#define TX_SOURCE_CODE

#include "tx_api.h"
#include "tx_trace.h"
#include "tx_initialize.h"
#include "tx_thread.h"
#include "tx_timer.h"
#include "txm_module.h"


/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _txm_module_manager_thread_create                   PORTABLE C      */
/*                                                           6.0.1        */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Scott Larson, Microsoft Corporation                                 */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function creates a thread and places it on the list of created */
/*    threads.                                                            */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    thread_ptr                        Thread control block pointer      */
/*    name                              Pointer to thread name string     */
/*    shell_function                    Shell function of the thread      */
/*    entry_function                    Entry function of the thread      */
/*    entry_input                       32-bit input value to thread      */
/*    stack_start                       Pointer to start of stack         */
/*    stack_size                        Stack size in bytes               */
/*    priority                          Priority of thread                */
/*                                        (default 0-31)                  */
/*    preempt_threshold                 Preemption threshold              */
/*    time_slice                        Thread time-slice value           */
/*    auto_start                        Automatic start selection         */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                            Completion status                 */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _txm_module_manager_thread_stack_build Build initial thread stack   */
/*    _tx_thread_system_resume              Resume automatic start thread */
/*    _tx_thread_system_ni_resume           Noninterruptable resume thread*/
/*    _tx_thread_system_preempt_check       Check for preemption          */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    _txm_module_manager_start             Initiate module's start thread*/
/*    _txm_module_manager_stop              Initiate module's stop thread */
/*    _txm_module_manager_kernel_dispatch   Kernel dispatch function      */
/*                                                                        */
/*  RELEASE HISTORY                                                       */
/*                                                                        */
/*    DATE              NAME                      DESCRIPTION             */
/*                                                                        */
/*  06-30-2020     Scott Larson             Initial Version 6.0.1         */
/*                                                                        */
/**************************************************************************/
UINT  _txm_module_manager_thread_create(TX_THREAD *thread_ptr, CHAR *name, VOID (*shell_function)(TX_THREAD *, TXM_MODULE_INSTANCE *), 
                               VOID (*entry_function)(ULONG), ULONG entry_input,
                               VOID *stack_start, ULONG stack_size, UINT priority, UINT preempt_threshold,
                               ULONG time_slice, UINT auto_start, UINT thread_control_block_size, TXM_MODULE_INSTANCE *module_instance)
{

TX_INTERRUPT_SAVE_AREA

TX_THREAD                       *next_thread;
TX_THREAD                       *previous_thread;
#ifndef TX_DISABLE_PREEMPTION_THRESHOLD
TX_THREAD                       *saved_thread_ptr;
UINT                            saved_threshold =  0;
#endif
#ifdef TX_ENABLE_STACK_CHECKING
ULONG                           new_stack_start;
#endif
TXM_MODULE_THREAD_ENTRY_INFO    *thread_entry_info;
VOID                            *stack_end;
ULONG                           i;
#ifndef TX_TIMER_PROCESS_IN_ISR
TX_THREAD                       *current_thread;
#endif
#if TXM_MODULE_MEMORY_PROTECTION
ULONG                           status;
#endif

    /* First, check for an invalid thread pointer.  */
    if (thread_ptr == TX_NULL)
    {

        /* Thread pointer is invalid, return appropriate error code.  */
        return(TX_THREAD_ERROR);
    }

    /* Now check for invalid thread control block size.  */
    else if (thread_control_block_size != (sizeof(TX_THREAD)))
    {

        /* Thread pointer is invalid, return appropriate error code.  */
        return(TX_THREAD_ERROR);
    }

    /* Disable interrupts.  */
    TX_DISABLE

    /* Increment the preempt disable flag.  */
    _tx_thread_preempt_disable++;

    /* Restore interrupts.  */
    TX_RESTORE

    /* Next see if it is already in the created list.  */
    next_thread =  _tx_thread_created_ptr;
    stack_end   =  (VOID *) (((UCHAR *) ((VOID *) stack_start)) + (stack_size - 1));
    for (i = 0; i < _tx_thread_created_count; i++)
    {

        /* Determine if this thread matches the thread in the list.  */
        if (thread_ptr == next_thread)
        {
        
            break;
        }

        /* Check the stack pointer to see if it overlaps with this thread's stack.  */
 
        /*lint -e{946} suppress pointer comparison, since this is necessary. */
        if (((UCHAR *) ((VOID *) stack_start)) >= ((UCHAR *) ((VOID *) next_thread -> tx_thread_stack_start)))
        {

            /*lint -e{946} suppress pointer comparison, since this is necessary. */
            if (((UCHAR *) ((VOID *) stack_start)) < ((UCHAR *) ((VOID *) next_thread -> tx_thread_stack_end)))
            {
        
                /* This stack overlaps with an existing thread, clear the stack pointer to 
                   force a stack error below.  */
                stack_start =  TX_NULL;
                break;
            }
        }

        /* Check the end of the stack to see if it is inside this thread's stack area as well.  */

        /*lint -e{946} suppress pointer comparison, since this is necessary. */
        if (((UCHAR *) ((VOID *) stack_end)) >= ((UCHAR *) ((VOID *) next_thread -> tx_thread_stack_start)))
        {

            /*lint -e{946} suppress pointer comparison, since this is necessary. */
            if (((UCHAR *) ((VOID *) stack_end)) < ((UCHAR *) ((VOID *) next_thread -> tx_thread_stack_end)))
            {
        
                /* This stack overlaps with an existing thread, clear the stack pointer to 
                   force a stack error below.  */
                stack_start =  TX_NULL;
                break;
            }
        }

        /* Move to the next thread.  */
        next_thread =  next_thread -> tx_thread_created_next;
    }

    /* Disable interrupts.  */
    TX_DISABLE

    /* Decrement the preempt disable flag.  */
    _tx_thread_preempt_disable--;
    
    /* Restore interrupts.  */
    TX_RESTORE

    /* Check for preemption.  */
    _tx_thread_system_preempt_check();

    /* At this point, check to see if there is a duplicate thread.  */
    if (thread_ptr == next_thread)
    {

        /* Thread is already created, return appropriate error code.  */
        return(TX_THREAD_ERROR);
    }

    /* Check for invalid starting address of stack.  */
    if (stack_start == TX_NULL)
    {

        /* Invalid stack or entry point, return appropriate error code.  */
        return(TX_PTR_ERROR);
    }

    /* Check for invalid thread entry point.  */
    if (entry_function == TX_NULL)
    {

        /* Invalid stack or entry point, return appropriate error code.  */
        return(TX_PTR_ERROR);
    }

    /* Check the stack size.  */
    if (stack_size < TX_MINIMUM_STACK)
    {

        /* Stack is not big enough, return appropriate error code.  */
        return(TX_SIZE_ERROR);
    }

    /* Check the priority specified.  */
    if (priority >= TX_MAX_PRIORITIES)
    {

        /* Invalid priority selected, return appropriate error code.  */
        return(TX_PRIORITY_ERROR);
    }

    /* Check preemption threshold. */
    if (preempt_threshold > priority)
    {

        /* Invalid preempt threshold, return appropriate error code.  */
        return(TX_THRESH_ERROR);
    }

    /* Check the start selection.  */
    if (auto_start > TX_AUTO_START)
    {

        /* Invalid auto start selection, return appropriate error code.  */
        return(TX_START_ERROR);
    }

#ifndef TX_TIMER_PROCESS_IN_ISR

    /* Pickup thread pointer.  */
    TX_THREAD_GET_CURRENT(current_thread)

    /* Check for invalid caller of this function.  First check for a calling thread.  */
    if (current_thread == &_tx_timer_thread)
    {

        /* Invalid caller of this function, return appropriate error code.  */
        return(TX_CALLER_ERROR);
    }
#endif

    /* Check for interrupt call.  */
    if (TX_THREAD_GET_SYSTEM_STATE() != 0)
    {
    
        /* Now, make sure the call is from an interrupt and not initialization.  */
        if (TX_THREAD_GET_SYSTEM_STATE() < TX_INITIALIZE_IN_PROGRESS)
        {
            
            /* Invalid caller of this function, return appropriate error code.  */
            return(TX_CALLER_ERROR);
        }
    }

#ifndef TX_DISABLE_STACK_FILLING

    /* Set the thread stack to a pattern prior to creating the initial
       stack frame.  This pattern is used by the stack checking routines
       to see how much has been used.  */
    TX_MEMSET(stack_start, ((UCHAR) TX_STACK_FILL), stack_size);
#endif

#ifdef TX_ENABLE_STACK_CHECKING

    /* Ensure that there are two ULONG of 0xEF patterns at the top and 
       bottom of the thread's stack. This will be used to check for stack
       overflow conditions during run-time.  */
    stack_size =  ((stack_size/sizeof(ULONG)) * sizeof(ULONG)) - sizeof(ULONG);

    /* Ensure the starting stack address is evenly aligned.  */
    new_stack_start =  ((((ULONG) stack_start) + (sizeof(ULONG) - 1) ) & (~(sizeof(ULONG) - 1)));

    /* Determine if the starting stack address is different.  */
    if (new_stack_start != ((ULONG) stack_start))
    {
    
        /* Yes, subtract another ULONG from the size to avoid going past the stack area.  */
        stack_size =  stack_size - sizeof(ULONG);
    }

    /* Update the starting stack pointer.  */
    stack_start =  (VOID *) new_stack_start;
#endif

    /* Allocate the thread entry information at the top of thread's stack - Leaving one 
       ULONG worth of 0xEF pattern between the actual stack and the entry info structure.  */
    stack_size =  stack_size - (sizeof(TXM_MODULE_THREAD_ENTRY_INFO) + (3*sizeof(ULONG)));

    /* Prepare the thread control block prior to placing it on the created
       list.  */

    /* Initialize thread control block to all zeros.  */
    TX_MEMSET(thread_ptr, 0, sizeof(TX_THREAD));
    
#if TXM_MODULE_MEMORY_PROTECTION
    /* If this is a memory protected module, allocate a kernel stack.  */
    if((module_instance -> txm_module_instance_property_flags) & TXM_MODULE_MEMORY_PROTECTION)
    {
        /* Allocate kernel stack space. */
        status = _txm_module_manager_object_allocate((VOID **) &(thread_ptr -> tx_thread_module_kernel_stack_start), TXM_MODULE_KERNEL_STACK_SIZE, module_instance);
        if(status)
        {
            return(status);
        }
        
#ifndef TX_DISABLE_STACK_FILLING
        /* Set the thread stack to a pattern prior to creating the initial
           stack frame.  This pattern is used by the stack checking routines
           to see how much has been used.  */
        TX_MEMSET(thread_ptr -> tx_thread_module_kernel_stack_start, ((UCHAR) TX_STACK_FILL), TXM_MODULE_KERNEL_STACK_SIZE);
#endif
        
        /* Align kernel stack pointer.  */
        thread_ptr -> tx_thread_module_kernel_stack_end = (VOID *) (((ALIGN_TYPE)(thread_ptr -> tx_thread_module_kernel_stack_start) + TXM_MODULE_KERNEL_STACK_SIZE) & ~0x07);

        /* Set kernel stack size.  */
        thread_ptr -> tx_thread_module_kernel_stack_size = TXM_MODULE_KERNEL_STACK_SIZE;
    }
    
    /* Place the stack parameters into the thread's control block.  */
    thread_ptr -> tx_thread_module_stack_start =    stack_start;
    thread_ptr -> tx_thread_module_stack_size =     stack_size;
#endif

    /* Place the supplied parameters into the thread's control block.  */
    thread_ptr -> tx_thread_name =              name;
    thread_ptr -> tx_thread_entry =             entry_function;
    thread_ptr -> tx_thread_entry_parameter =   entry_input;
    thread_ptr -> tx_thread_stack_start =       stack_start;
    thread_ptr -> tx_thread_stack_size =        stack_size;
    thread_ptr -> tx_thread_stack_end =         (VOID *) (((UCHAR *) stack_start) + (stack_size-1));
#if TXM_MODULE_MEMORY_PROTECTION
    thread_ptr -> tx_thread_module_stack_end =  thread_ptr -> tx_thread_stack_end;
#endif
    thread_ptr -> tx_thread_priority =          priority;
    thread_ptr -> tx_thread_user_priority =     priority;
    thread_ptr -> tx_thread_time_slice =        time_slice;
    thread_ptr -> tx_thread_new_time_slice =    time_slice;
    thread_ptr -> tx_thread_inherit_priority =  TX_MAX_PRIORITIES;
    
#ifndef TX_DISABLE_PREEMPTION_THRESHOLD

    /* Preemption-threshold is enabled, setup accordingly.  */
    thread_ptr -> tx_thread_preempt_threshold =       preempt_threshold;
    thread_ptr -> tx_thread_user_preempt_threshold =  preempt_threshold;
#else

    /* Preemption-threshold is disabled, determine if preemption-threshold was required.  */
    if (priority != preempt_threshold)
    {

        /* Preemption-threshold specified. Since specific preemption-threshold is not supported,
           disable all preemption.  */
        thread_ptr -> tx_thread_preempt_threshold =       0;
        thread_ptr -> tx_thread_user_preempt_threshold =  0;
    } 
    else
    {

        /* Preemption-threshold is not specified, just setup with the priority.  */
        thread_ptr -> tx_thread_preempt_threshold =       priority;
        thread_ptr -> tx_thread_user_preempt_threshold =  priority;
    }
#endif

    /* Now fill in the values that are required for thread initialization.  */
    thread_ptr -> tx_thread_state =  TX_SUSPENDED;

    /* Setup the necessary fields in the thread timer block.  */
    TX_THREAD_CREATE_TIMEOUT_SETUP(thread_ptr)

    /* Setup pointer to the thread entry information structure, which will live at the top of each
       module thread's stack. This will allow the module thread entry function to avoid direct 
       access to the actual thread control block.  */
    thread_entry_info =  (TXM_MODULE_THREAD_ENTRY_INFO *) (((UCHAR *) thread_ptr -> tx_thread_stack_end) + (2*sizeof(ULONG)) + 1);
    thread_entry_info =  (TXM_MODULE_THREAD_ENTRY_INFO *) (((ALIGN_TYPE)(thread_entry_info)) & (~0x3));
    
    /* Build the thread entry information structure.  */
    thread_entry_info -> txm_module_thread_entry_info_thread =                   thread_ptr;
    thread_entry_info -> txm_module_thread_entry_info_module =                   module_instance;
    thread_entry_info -> txm_module_thread_entry_info_data_base_address =        module_instance -> txm_module_instance_module_data_base_address;
    thread_entry_info -> txm_module_thread_entry_info_code_base_address =        module_instance -> txm_module_instance_code_start;
    thread_entry_info -> txm_module_thread_entry_info_entry =                    thread_ptr -> tx_thread_entry;
    thread_entry_info -> txm_module_thread_entry_info_parameter =                thread_ptr -> tx_thread_entry_parameter;
    thread_entry_info -> txm_module_thread_entry_info_callback_request_queue =   &(module_instance -> txm_module_instance_callback_request_queue);
    thread_entry_info -> txm_module_thread_entry_info_callback_request_thread =  &(module_instance -> txm_module_instance_callback_request_thread);

    /* Populate thread control block with some stock information from the module.  */
    TXM_MODULE_MANAGER_THREAD_SETUP(thread_ptr, module_instance)

#ifndef TX_DISABLE_NOTIFY_CALLBACKS
    thread_entry_info ->  txm_module_thread_entry_info_exit_notify =        thread_ptr -> tx_thread_entry_exit_notify;
#else
    thread_entry_info ->  txm_module_thread_entry_info_exit_notify =        TX_NULL;
#endif
    if (thread_ptr -> tx_thread_entry == module_instance -> txm_module_instance_start_thread_entry)
        thread_entry_info ->  txm_module_thread_entry_info_start_thread =   TX_TRUE;
    else
        thread_entry_info ->  txm_module_thread_entry_info_start_thread =   TX_FALSE;

    /* Place pointers to the thread info and module instance in the thread control block.  */
    thread_ptr -> tx_thread_module_instance_ptr =    (VOID *) module_instance;
    thread_ptr -> tx_thread_module_entry_info_ptr =  (VOID *) thread_entry_info;

    /* Place the thread entry information pointer in the thread control block so it can be picked up
       in the following stack build function. This is supplied to the module's shell entry function
       to avoid direct access to the actual thread control block. Note that this is overwritten
       with the actual stack pointer at the end of stack build.  */
    thread_ptr -> tx_thread_stack_ptr =  (VOID *) thread_entry_info;

    /* Call the target specific stack frame building routine to build the 
       thread's initial stack and to setup the actual stack pointer in the
       control block.  */
    _txm_module_manager_thread_stack_build(thread_ptr, shell_function);

#ifdef TX_ENABLE_STACK_CHECKING

    /* Setup the highest usage stack pointer.  */
    thread_ptr -> tx_thread_stack_highest_ptr =  thread_ptr -> tx_thread_stack_ptr;
#endif

    /* Prepare to make this thread a member of the created thread list.  */
    TX_DISABLE

    /* Load the thread ID field in the thread control block.  */
    thread_ptr -> tx_thread_id =  TX_THREAD_ID;

    /* Place the thread on the list of created threads.  First,
       check for an empty list.  */
    if (_tx_thread_created_count++ == 0)
    {

        /* The created thread list is empty.  Add thread to empty list.  */
        _tx_thread_created_ptr =                    thread_ptr;
        thread_ptr -> tx_thread_created_next =      thread_ptr;
        thread_ptr -> tx_thread_created_previous =  thread_ptr;
    }
    else
    {

        /* This list is not NULL, add to the end of the list.  */
        next_thread =  _tx_thread_created_ptr;
        previous_thread =  next_thread -> tx_thread_created_previous;

        /* Place the new thread in the list.  */
        next_thread -> tx_thread_created_previous =  thread_ptr;
        previous_thread -> tx_thread_created_next =  thread_ptr;

        /* Setup this thread's created links.  */
        thread_ptr -> tx_thread_created_previous =  previous_thread;
        thread_ptr -> tx_thread_created_next =      next_thread;    
    }

    /* If trace is enabled, register this object.  */
    TX_TRACE_OBJECT_REGISTER(TX_TRACE_OBJECT_TYPE_THREAD, thread_ptr, name, stack_start, stack_size)

    /* If trace is enabled, insert this event into the trace buffer.  */
    TX_TRACE_IN_LINE_INSERT(TX_TRACE_THREAD_CREATE, thread_ptr, priority, stack_start, stack_size, TX_TRACE_THREAD_EVENTS)

    /* Register thread in the thread array structure.  */
    TX_EL_THREAD_REGISTER(thread_ptr)

    /* Log this kernel call.  */
    TX_EL_THREAD_CREATE_INSERT

    /* Determine if an automatic start was requested.  If so, call the resume
       thread function and then check for a preemption condition.  */
    if (auto_start == TX_AUTO_START)
    {

#ifndef TX_DISABLE_PREEMPTION_THRESHOLD

        /* Determine if the create call is being called from initialization.  */
        if (TX_THREAD_GET_SYSTEM_STATE() >= TX_INITIALIZE_IN_PROGRESS)
        {

            /* Yes, this create call was made from initialization.  */

            /* Pickup the current thread execute pointer, which corresponds to the
               highest priority thread ready to execute.  Interrupt lockout is 
               not required, since interrupts are assumed to be disabled during 
               initialization.  */
            saved_thread_ptr =  _tx_thread_execute_ptr;

            /* Determine if there is thread ready for execution.  */
            if (saved_thread_ptr != TX_NULL)
            {
                
                /* Yes, a thread is ready for execution when initialization completes.  */

                /* Save the current preemption-threshold.  */
                saved_threshold =  saved_thread_ptr -> tx_thread_preempt_threshold;

                /* For initialization, temporarily set the preemption-threshold to the 
                   priority level to make sure the highest-priority thread runs once 
                   initialization is complete.  */
                saved_thread_ptr -> tx_thread_preempt_threshold =  saved_thread_ptr -> tx_thread_priority;
            }
        } 
        else
        {

            /* Simply set the saved thread pointer to NULL.  */
            saved_thread_ptr =  TX_NULL;
        }
#endif

#ifdef TX_NOT_INTERRUPTABLE

        /* Perform any additional activities for tool or user purpose.  */
        TX_THREAD_CREATE_EXTENSION(thread_ptr)

        /* Resume the thread!  */
        _tx_thread_system_ni_resume(thread_ptr);

        /* Restore previous interrupt posture.  */
        TX_RESTORE
#else

        /* Temporarily disable preemption.  */
        _tx_thread_preempt_disable++;

        /* Restore previous interrupt posture.  */
        TX_RESTORE

        /* Perform any additional activities for tool or user purpose.  */
        TX_THREAD_CREATE_EXTENSION(thread_ptr)

        /* Call the resume thread function to make this thread ready.  */ 
        _tx_thread_system_resume(thread_ptr);
#endif
 
#ifndef TX_DISABLE_PREEMPTION_THRESHOLD

        /* Determine if the thread's preemption-threshold needs to be restored.  */
        if (saved_thread_ptr != TX_NULL)
        {

            /* Yes, restore the previous highest-priority thread's preemption-threshold. This
               can only happen if this routine is called from initialization.  */
            saved_thread_ptr -> tx_thread_preempt_threshold =  saved_threshold;
        } 
#endif

        /* Interrupts are already restored, simply return success.  */
        return(TX_SUCCESS);
    }

    /* Restore interrupts.  */
    TX_RESTORE

    /* Return success.  */
    return(TX_SUCCESS);
}

