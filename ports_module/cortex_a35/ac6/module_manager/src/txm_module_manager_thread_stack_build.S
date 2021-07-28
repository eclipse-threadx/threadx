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


    .text
    .align 3
/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _txm_module_manager_thread_stack_build            Cortex-A35/AC6    */
/*                                                           6.1.3        */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Andres Mlinar, Microsoft Corporation                                */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function builds a stack frame on the supplied thread's stack.  */
/*    The stack frame results in a fake interrupt return to the supplied  */
/*    function pointer.                                                   */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    thread_ptr                            Pointer to thread             */
/*    function_ptr                          Pointer to entry function     */
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
/*    _tx_thread_create                     Create thread service         */
/*                                                                        */
/*  RELEASE HISTORY                                                       */
/*                                                                        */
/*    DATE              NAME                      DESCRIPTION             */
/*                                                                        */
/*  12-31-2020     Andres Mlinar            Initial Version 6.1.3         */
/*                                                                        */
/**************************************************************************/
// VOID   _txm_module_manager_thread_stack_build(TX_THREAD *thread_ptr, VOID (*function_ptr)(TX_THREAD *, TXM_MODULE_INSTANCE *))
// {
    .global  _txm_module_manager_thread_stack_build
    .type    _txm_module_manager_thread_stack_build, @function
_txm_module_manager_thread_stack_build:


    /* Build an interrupt frame.  On Cortex-A35 it should look like this:

       Stack Top:      SSPR        Initial SSPR
                       ELR         Point of interrupt
                       x28         Initial value for x28
                       not used    Not used
                       x26         Initial value for x26
                       x27         Initial value for x27
                       x24         Initial value for x24
                       x25         Initial value for x25
                       x22         Initial value for x22
                       x23         Initial value for x23
                       x20         Initial value for x20
                       x21         Initial value for x21
                       x18         Initial value for x18
                       x19         Initial value for x19
                       x16         Initial value for x16
                       x17         Initial value for x17
                       x14         Initial value for x14
                       x15         Initial value for x15
                       x12         Initial value for x12
                       x13         Initial value for x13
                       x10         Initial value for x10
                       x11         Initial value for x11
                       x8          Initial value for x8
                       x9          Initial value for x9
                       x6          Initial value for x6
                       x7          Initial value for x7
                       x4          Initial value for x4
                       x5          Initial value for x5
                       x2          Initial value for x2
                       x3          Initial value for x3
                       x0          Initial value for x0
                       x1          Initial value for x1
                       x29         Initial value for x29 (frame pointer)
                       x30         Initial value for x30 (link register)
                       0           For stack backtracing

       Stack Bottom: (higher memory address)
    */

    LDR     x4, [x0, #24]                       // Pickup end of stack area
    BIC     x4, x4, #0xF                        // Ensure 16-byte alignment

    LDR     x5, [x0, #8]                        // Pickup thread entry info pointer,which is in the stack pointer position of the thread control block.
                                                //   It was setup in the txm_module_manager_thread_create function. It will be overwritten later in this
                                                //   function with the actual, initial stack pointer.

    /* Actually build the stack frame.  */

    MOV     x2, #0                              // Build clear value
    MOV     x3, #0                              //

    STP     x2, x3, [x4, #-16]!                 // Set backtrace to 0
    STP     x2, x3, [x4, #-16]!                 // Set initial x29, x30
    STP     x0, x5, [x4, #-16]!                 // Set initial x0, x1
                                                //   x0->x0 is the thread control block pointer
                                                //   x5->x1 is the thread entry info pointer
    STP     x2, x3, [x4, #-16]!                 // Set initial x2, x3
    STP     x2, x3, [x4, #-16]!                 // Set initial x4, x5
    STP     x2, x3, [x4, #-16]!                 // Set initial x6, x7
    STP     x2, x3, [x4, #-16]!                 // Set initial x8, x9
    STP     x2, x3, [x4, #-16]!                 // Set initial x10, x11
    STP     x2, x3, [x4, #-16]!                 // Set initial x12, x13
    STP     x2, x3, [x4, #-16]!                 // Set initial x14, x15
    STP     x2, x3, [x4, #-16]!                 // Set initial x16, x17
    STP     x2, x3, [x4, #-16]!                 // Set initial x18, x19
    STP     x2, x3, [x4, #-16]!                 // Set initial x20, x21
    STP     x2, x3, [x4, #-16]!                 // Set initial x22, x23
    STP     x2, x3, [x4, #-16]!                 // Set initial x24, x25
    STP     x2, x3, [x4, #-16]!                 // Set initial x26, x27
    STP     x2, x3, [x4, #-16]!                 // Set initial x28
#ifdef EL1
    MOV     x2, #0x4                            // Build initial SPSR  (EL1)
#else
#ifdef EL2
    MOV     x2, #0x8                            // Build initial SPSR  (EL2)
#else
    MOV     x2, #0xC                            // Build initial SPSR  (EL3)
#endif
#endif
    MOV     x3, x1                              // Build initial ELR
    STP     x2, x3, [x4, #-16]!                 // Set initial SPSR & ELR

    /* Setup stack pointer.  */
    /* thread_ptr -> tx_thread_stack_ptr =  x2;  */

    STR     x4, [x0, #8]                        // Save stack pointer in thread's
    RET                                         // Return to caller

// }
