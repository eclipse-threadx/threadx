/***************************************************************************
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

#include "tx_port_tricore.h"


/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _tx_thread_stack_build                              TriCore/GNU     */
/*                                                           6.5.2        */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Andreas Neubauer, safecubed GmbH                                    */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function builds the initial context of a thread. It takes two  */
/*    CSAs from the free context list: a lower context and an upper       */
/*    context. The thread starts at function_ptr when the scheduler       */
/*    first restores its context.                                         */
/*                                                                        */
/*    A CSA chain that the thread still holds, as after tx_thread_reset,  */
/*    is released first.                                                  */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    thread_ptr                            Pointer to thread control blk */
/*    function_ptr                          Function the thread starts at */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    None                                                                */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _tx_thread_csa_release                Release CSA chain of thread   */
/*    _tx_thread_decode_link_word           Convert link word to address  */
/*    TX_MEMSET                             Clear CSA                     */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    _tx_thread_create                     Create thread service         */
/*    _tx_thread_reset                      Reset thread service          */
/*                                                                        */
/*  RELEASE HISTORY                                                       */
/*                                                                        */
/*    DATE              NAME                      DESCRIPTION             */
/*                                                                        */
/*  11-04-2026     Andreas Neubauer         Initial Version 6.5.2         */
/*                                                                        */
/**************************************************************************/
VOID   _tx_thread_stack_build(TX_THREAD *thread_ptr, VOID (*function_ptr)(VOID))
{

    TX_INTERRUPT_SAVE_AREA
    UINT first_csa_pcxi_link_word = 0;
    UINT second_csa_pcxi_link_word = 0;
    UINT lcx_link_word;
    UINT * first_csa_ptr = TX_NULL;
    UINT * second_csa_ptr = TX_NULL;
    UINT * thread_stack_ptr = (UINT *)((UINT)thread_ptr->tx_thread_stack_end & ~(TX_PORT_TRICORE_STACK_ALIGNMENT - 1U));


    /* tx_thread_reset rebuilds a thread that still holds its CSA chain.  */
    _tx_thread_csa_release(thread_ptr);

    TX_DISABLE

    _dsync();

    /* The CSA that LCX points to and the CSAs after it are the reserve of
       the free context list depletion trap (FCD). The thread shall not
       receive one of them.  */
    lcx_link_word = __mfcr(TX_PORT_TRICORE_CSFR_LCX) & TX_PORT_TRICORE_CSA_LINK_MASK;

    /* Get first free Context Save Area. */
    first_csa_pcxi_link_word = __mfcr(TX_PORT_TRICORE_CSFR_FCX) & TX_PORT_TRICORE_CSA_LINK_MASK;
    first_csa_ptr = _tx_thread_decode_link_word(first_csa_pcxi_link_word);

    if( first_csa_ptr != TX_NULL )
    {
        /* Get second free Context Save Area. */
        second_csa_pcxi_link_word = first_csa_ptr[TX_PORT_TRICORE_CSA_PCXI_LINK_WORD] & TX_PORT_TRICORE_CSA_LINK_MASK;
        second_csa_ptr = _tx_thread_decode_link_word(second_csa_pcxi_link_word);
    }

    if( ( second_csa_ptr != TX_NULL ) && ( second_csa_pcxi_link_word != lcx_link_word ) )
    {
        /* Remove the two CSAs from the free CSA list by setting FCX to the
           link word of the second CSA.  */
        __mtcr(TX_PORT_TRICORE_CSFR_FCX, second_csa_ptr[ TX_PORT_TRICORE_CSA_PCXI_LINK_WORD ] & TX_PORT_TRICORE_CSA_LINK_MASK);
        _isync();
    }
    else
    {

        /* The first SVLCX takes the first CSA. The second SVLCX takes the
           CSA that LCX points to and raises FCD, or raises FCU if the list
           ends at the first CSA.  */
        _svlcx();
        _svlcx();
        while (TX_TRUE)
        {

        }
    }

    TX_RESTORE

    /* Initialise the first CSA as lower context. */
    TX_MEMSET(first_csa_ptr, 0, TX_PORT_TRICORE_CSA_SIZE);
    first_csa_ptr[TX_PORT_TRICORE_CSA_PCXI_LINK_WORD]               = TX_PORT_TRICORE_CSA_INITIAL_LOWER_PCXI |
                                                                      second_csa_pcxi_link_word;
    first_csa_ptr[TX_PORT_TRICORE_CSA_LOWER_CONTEXT_A11_RA]         = ( UINT ) function_ptr;

    /* Initialise the second CSA as upper context. */
    TX_MEMSET(second_csa_ptr, 0, TX_PORT_TRICORE_CSA_SIZE);
    second_csa_ptr[TX_PORT_TRICORE_CSA_PCXI_LINK_WORD]              = TX_PORT_TRICORE_CSA_INITIAL_UPPER_PCXI |
                                                                      TX_PORT_TRICORE_CSA_LINK_WORD_END_OF_CHAIN;
    second_csa_ptr[TX_PORT_TRICORE_CSA_UPPER_CONTEXT_A10_STACK_PTR] = ( UINT ) thread_stack_ptr;
    second_csa_ptr[TX_PORT_TRICORE_CSA_UPPER_CONTEXT_PSW]           = TX_PORT_TRICORE_CSA_INITIAL_SYSTEM_PSW;

    thread_ptr->tx_thread_csa = first_csa_pcxi_link_word;

    /* Setup a thread stack pointer.   */
    thread_ptr -> tx_thread_stack_ptr = thread_stack_ptr;

}


/* Returns the CSA chain of a completed or terminated thread to the free
   context list. The last CSA of the chain links to no CSA.  */
VOID   _tx_thread_csa_release(TX_THREAD *thread_ptr)
{

    TX_INTERRUPT_SAVE_AREA
    UINT head_csa_link_word;
    UINT * csa_ptr;

    head_csa_link_word = thread_ptr -> tx_thread_csa & TX_PORT_TRICORE_CSA_LINK_MASK;

    if (head_csa_link_word != 0U)
    {
        /* A free CSA links by its address only.  */
        csa_ptr = _tx_thread_decode_link_word(head_csa_link_word);
        while ((csa_ptr[TX_PORT_TRICORE_CSA_PCXI_LINK_WORD] & TX_PORT_TRICORE_CSA_LINK_MASK) != TX_PORT_TRICORE_CSA_LINK_WORD_END_OF_CHAIN)
        {
            csa_ptr[TX_PORT_TRICORE_CSA_PCXI_LINK_WORD] &= TX_PORT_TRICORE_CSA_LINK_MASK;
            csa_ptr = _tx_thread_decode_link_word(csa_ptr[TX_PORT_TRICORE_CSA_PCXI_LINK_WORD]);
        }

        TX_DISABLE
        _dsync();
        csa_ptr[TX_PORT_TRICORE_CSA_PCXI_LINK_WORD] = __mfcr(TX_PORT_TRICORE_CSFR_FCX);
        __mtcr(TX_PORT_TRICORE_CSFR_FCX, head_csa_link_word);
        _isync();
        TX_RESTORE

        thread_ptr -> tx_thread_csa = 0;
    }
}
