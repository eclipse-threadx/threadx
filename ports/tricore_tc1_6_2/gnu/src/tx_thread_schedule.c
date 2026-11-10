/***************************************************************************
 * Copyright (c) 2024 Microsoft Corporation
 * Copyright (c) 2026 safecubed GmbH
 * Copyright (c) 2026 Eclipse ThreadX contributors
 *
 * This program and the accompanying materials are made available under the
 * terms of the MIT License which is available at
 * https://opensource.org/licenses/MIT.
 *
 * SPDX-License-Identifier: MIT
 **************************************************************************/

/* Portions of this file were generated with AI assistance. */


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
#include "tx_port_tricore.h"


#if (defined(TX_ENABLE_EXECUTION_CHANGE_NOTIFY) || defined(TX_EXECUTION_PROFILE_ENABLE))
VOID    _tx_execution_thread_enter(VOID);
VOID    _tx_execution_thread_exit(VOID);
#endif

#ifdef TX_LOW_POWER
VOID    tx_low_power_enter(VOID);
VOID    tx_low_power_exit(VOID);
#endif



/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _tx_thread_schedule                                 TriCore/GNU     */
/*                                                           6.5.2        */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Andreas Neubauer, safecubed GmbH                                    */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function starts the first thread. It clears                    */
/*    _tx_thread_preempt_disable, which is set during initialisation by   */
/*    TX_PORT_SPECIFIC_POST_INITIALIZATION, enables interrupts and enters */
/*    the scheduler through the system call. The scheduler waits until    */
/*    _tx_thread_execute_ptr holds a thread and switches to it. A timer   */
/*    interrupt that is already pending can start the first thread        */
/*    through the dispatch interrupt before the system call is reached.   */
/*    This function does not return.                                      */
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
/*    _tx_thread_syscall_handler            Through the system call       */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    _tx_initialize_kernel_enter          ThreadX entry function         */
/*                                                                        */
/*  RELEASE HISTORY                                                       */
/*                                                                        */
/*    DATE              NAME                      DESCRIPTION             */
/*                                                                        */
/*  11-04-2026     Andreas Neubauer         Initial Version 6.5.2         */
/*                                                                        */
/**************************************************************************/



VOID   _tx_thread_schedule(VOID)
{

    /* _tx_initialize_low_level has set up the timer and the dispatch
       interrupt. A timer interrupt that is already pending runs as soon as
       interrupts are enabled; a thread it makes ready starts through the
       dispatch interrupt, and the system call below is not reached.  */
    _tx_thread_preempt_disable = 0;
    __enable();

    __syscall(TX_PORT_TRICORE_SYSCALL_ID);
    while(TX_TRUE)
    {

    }

}


UINT * _tx_thread_decode_link_word( UINT link_word )
{
    UINT linked_context;

    linked_context = ((link_word & TX_PORT_TRICORE_CSA_LINK_SEGMENT_MASK) << (TX_PORT_TRICORE_CSA_ADDRESS_SEGMENT_OFF - TX_PORT_TRICORE_CSA_LINK_SEGMENT_OFF)) |
                     ((link_word & TX_PORT_TRICORE_CSA_LINK_OFFSET_MASK) << TX_PORT_TRICORE_CSA_ADDRESS_OFFSET_OFF);
    return ( UINT * ) linked_context;
}




/* Each of the following functions shall be a call of its own: call_depth counts
   the CSA frames that the calls create.  */

static VOID __attribute__( ( noinline ) ) _tx_thread_csa_save(UINT call_depth)
{
    UINT * csa_ptr;
    UINT * lower_csa_ptr;
    UINT * upper_csa_ptr;
    UINT lower_csa;

    __dsync();
    csa_ptr = _tx_thread_decode_link_word(__mfcr(TX_PORT_TRICORE_CSFR_PCXI));

    for (UINT i = 0U; i < (call_depth - 1U); i++)
    {
        csa_ptr = _tx_thread_decode_link_word(csa_ptr[TX_PORT_TRICORE_CSA_PCXI_LINK_WORD]);
    }

    lower_csa = csa_ptr[TX_PORT_TRICORE_CSA_PCXI_LINK_WORD];
    lower_csa_ptr = _tx_thread_decode_link_word(lower_csa);
    upper_csa_ptr = _tx_thread_decode_link_word(lower_csa_ptr[TX_PORT_TRICORE_CSA_PCXI_LINK_WORD]);

    _tx_thread_current_ptr->tx_thread_stack_ptr = (VOID *) upper_csa_ptr[TX_PORT_TRICORE_CSA_UPPER_CONTEXT_A10_STACK_PTR];
    _tx_thread_current_ptr->tx_thread_csa = lower_csa;
}


static VOID __attribute__( ( noinline ) ) _tx_thread_csa_restore(UINT call_depth)
{
    UINT * csa_ptr;

    __dsync();
    csa_ptr = _tx_thread_decode_link_word(__mfcr(TX_PORT_TRICORE_CSFR_PCXI));

    for (UINT i = 0U; i < (call_depth - 1U); i++)
    {
        csa_ptr = _tx_thread_decode_link_word(csa_ptr[TX_PORT_TRICORE_CSA_PCXI_LINK_WORD]);
    }

    csa_ptr[TX_PORT_TRICORE_CSA_PCXI_LINK_WORD] = _tx_thread_current_ptr->tx_thread_csa;
}


static VOID __attribute__( ( noinline ) ) _tx_ts_handler(UINT call_depth)
{

    TX_THREAD *current_thread = _tx_thread_current_ptr;
    TX_THREAD *next_thread;

    if (current_thread != TX_NULL)
    {
        if (_tx_timer_time_slice != 0U)
        {
            current_thread->tx_thread_time_slice = _tx_timer_time_slice;
            _tx_timer_time_slice = 0;
        }

#if (defined(TX_ENABLE_EXECUTION_CHANGE_NOTIFY) || defined(TX_EXECUTION_PROFILE_ENABLE))
        __disable();
        _tx_execution_thread_exit();
        __enable();
#endif

        _tx_thread_csa_save(call_depth);
        _tx_thread_current_ptr = TX_NULL;
    }

    /* Increment the nested interrupt condition.  */
    _tx_thread_system_state++;


    /* Idle loop. ICR.CCPN blocks the dispatch interrupt here, so the loop
       masks interrupts through ICR.IE.  */
    while (TX_TRUE)
    {
        __disable();
        __nop();
        next_thread = _tx_thread_execute_ptr;
        if (next_thread != TX_NULL)
        {
            _tx_thread_current_ptr = next_thread;
            break;
        }
#ifdef TX_LOW_POWER
        tx_low_power_enter();
        tx_low_power_exit();
#endif
        __enable();
        __nop();
    }

#if (defined(TX_ENABLE_EXECUTION_CHANGE_NOTIFY) || defined(TX_EXECUTION_PROFILE_ENABLE))
    _tx_execution_thread_enter();
#endif

    /* Interrupts stay disabled until the return to next_thread, so the switch
       to next_thread satisfies every pending dispatch request.  */
    IfxSrc_clearRequest(&TX_PORT_TRICORE_DISPATCH_SRC);

    next_thread->tx_thread_run_count++;
    _tx_timer_time_slice = next_thread->tx_thread_time_slice;

    _tx_thread_system_state--;

    /* The CSA chain of the interrupted thread is still linked.  */
    if (current_thread != next_thread)
    {
        _tx_thread_csa_restore(call_depth);
    }

    return;
}



VOID _tx_thread_syscall_handler(UCHAR id )
{
    switch(id)
    {
        case TX_PORT_TRICORE_SYSCALL_ID:
            /* The dispatch interrupt shall not interrupt the scheduler. RFE
               restores the CCPN of the thread that the trap returns to.  */
            __set_ccpn_value(TX_PORT_TRICORE_DISPATCH_PRIORITY);

            _tx_ts_handler(TX_PORT_TRICORE_SYSCALL_CALL_DEPTH);
            break;

        default:
            break;
    }
}



IFX_INTERRUPT(_tx_thread_dispatch_isr, 0, TX_PORT_TRICORE_DISPATCH_PRIORITY)
{
    /* A thread that has disabled preemption keeps the CPU. It enters the
       scheduler through the system call once it enables preemption again.  */
    if (_tx_thread_preempt_disable == 0U)
    {
        _tx_ts_handler(TX_PORT_TRICORE_DISPATCH_CALL_DEPTH);
    }
}
