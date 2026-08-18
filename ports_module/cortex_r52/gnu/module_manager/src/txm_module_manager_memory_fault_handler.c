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
/**   Module Manager                                                      */
/**                                                                       */
/**************************************************************************/
/**************************************************************************/

#define TX_SOURCE_CODE

#include "tx_api.h"
#include "tx_thread.h"
#include "txm_module.h"


/* This handler is architecture-neutral: it terminates the faulting thread and
   calls the notification callback.  The fault registers are captured before it
   runs, in the abort vector, because DFSR, DFAR, IFSR and IFAR must be read
   before anything else can fault and overwrite them -- and on this core the
   abort is taken in Abort mode with its own banked lr and sp, so the capture has
   to happen there rather than here.

   Data aborts and prefetch aborts both arrive here.  A module can violate its
   protection either way: writing outside its data region, or branching outside
   its code region.  Which pair of registers is meaningful depends on which it
   was, and the fault info structure carries both.  */

/* Define the user's fault notification callback function pointer.  This is
   setup via the txm_module_manager_memory_fault_notify API.  */

VOID    (*_txm_module_manager_fault_notify)(TX_THREAD *, TXM_MODULE_INSTANCE *);


/* Define a macro that can be used to allocate global variables useful to
   store information about the last fault. This macro is defined in
   txm_module_port.h and is usually populated in the assembly language
   fault handling prior to the code calling _txm_module_manager_memory_fault_handler.  */

TXM_MODULE_MANAGER_FAULT_INFO


/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _txm_module_manager_memory_fault_handler           Cortex-R52       */
/*                                                           6.1.8        */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Scott Larson, Microsoft Corporation                                 */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function handles a fault associated with a memory protected    */
/*    module.                                                             */
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
/*    _tx_thread_terminate              Terminate thread                  */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Fault handler                                                       */
/*                                                                        */
/**************************************************************************/
VOID  _txm_module_manager_memory_fault_handler(VOID)
{

TXM_MODULE_INSTANCE     *module_instance_ptr;
TX_THREAD               *thread_ptr;

    /* Pickup the current thread.  */
    thread_ptr =  _tx_thread_current_ptr;

    /* Initialize the module instance pointer to NULL.  */
    module_instance_ptr =  TX_NULL;

    /* Is there a thread?  */
    if (thread_ptr)
    {
        /* Pickup the module instance.  */
        module_instance_ptr =  thread_ptr -> tx_thread_module_instance_ptr;

        /* Terminate the current thread.  */
        _tx_thread_terminate(_tx_thread_current_ptr);
    }

    /* Determine if there is a user memory fault notification callback.  */
    if (_txm_module_manager_fault_notify)
    {
        /* Yes, call the user's notification memory fault callback.  */
        (_txm_module_manager_fault_notify)(thread_ptr, module_instance_ptr);
    }
}
