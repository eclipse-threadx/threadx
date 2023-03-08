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
/**   Initialize                                                          */
/**                                                                       */
/**************************************************************************/
/**************************************************************************/

    .section .data
    .global  __tx_free_memory_start
__tx_free_memory_start:


    .section .text
/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _tx_initialize_low_level                           RISC-V64/GNU     */
/*                                                           6.2.1        */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Scott Larson, Microsoft Corporation                                 */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function is responsible for any low-level processor            */
/*    initialization, including setting up interrupt vectors, setting     */
/*    up a periodic timer interrupt source, saving the system stack       */
/*    pointer for use in ISR processing later, and finding the first      */
/*    available RAM memory address for tx_application_define.             */
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
/*    None                                                                */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    _tx_initialize_kernel_enter           ThreadX entry function        */
/*                                                                        */
/*  RELEASE HISTORY                                                       */
/*                                                                        */
/*    DATE              NAME                      DESCRIPTION             */
/*                                                                        */
/*  03-08-2023      Scott Larson            Initial Version 6.2.1         */
/*                                                                        */
/**************************************************************************/
/* VOID   _tx_initialize_low_level(VOID)
{  */
    .global  _tx_initialize_low_level
_tx_initialize_low_level:
    sd      sp, _tx_thread_system_stack_ptr, t0     // Save system stack pointer

    la      t0, __tx_free_memory_start              // Pickup first free address
    sd      t0, _tx_initialize_unused_memory, t1    // Save unused memory address

#ifdef __riscv_flen
    fscsr x0
#endif

    ret


    /* Define the actual timer interrupt/exception handler.  */

    .global  timer1_plic_IRQHandler
    //.global  __minterrupt_000007
    //EXTWEAK __require_minterrupt_vector_table
timer1_plic_IRQHandler:
//__minterrupt_000007:
    //REQUIRE __require_minterrupt_vector_table


    /* Before calling _tx_thread_context_save, we have to allocate an interrupt
       stack frame and save the current value of x1 (ra). */
//#if defined(__riscv_float_abi_single) || defined(__riscv_float_abi_double)
//    addi    sp, sp, -520                            // Allocate space for all registers - with floating point enabled
//#else
//    addi    sp, sp, -256                            // Allocate space for all registers - without floating point enabled
//#endif
//    sd      x1, 224(sp)                             // Store RA
//    call    _tx_thread_context_save                 // Call ThreadX context save

    /* Call the ThreadX timer routine.  */
    call    _tx_timer_interrupt                     // Call timer interrupt handler
    call    timer1_interrupt
    ret
    /* Timer interrupt processing is done, jump to ThreadX context restore.  */
//    j       _tx_thread_context_restore              // Jump to ThreadX context restore function. Note: this does not return!
