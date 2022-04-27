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

    SECTION    CODE:CODE
    ALIGN 2
/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _txm_module_manager_thread_stack_build               RXv2/IAR       */
/*                                                           6.1.10       */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Scott Larson, Microsoft Corporation                                 */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function builds a stack frame on the supplied thread's stack.  */
/*    The stack frame results in a fake interrupt return to the supplied  */
/*    function pointer.                                                   */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    thread_ptr                            Pointer to thread control blk */
/*    function_ptr                          Pointer to shell function     */
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
/*  01-31-2022          Scott Larson        Initial Version 6.1.10        */
/*                                                                        */
/**************************************************************************/
// VOID   _txm_module_manager_thread_stack_build(TX_THREAD *thread_ptr, VOID (*function_ptr)(TX_THREAD *, TXM_MODULE_INSTANCE *))
// {
    PUBLIC  __txm_module_manager_thread_stack_build
__txm_module_manager_thread_stack_build:

    /* Build an interrupt frame.  The form of the fake interrupt stack
       on the Renesas RX should look like the following after it is built:
       
    Stack Top:  1       Interrupt stack frame type
                ACC0
                ACC1
                R6
                R7
                R8
                R9
                R10
                R11
                R12
                R13
                FPSW
                R14
                R15
                R3
                R4
                R5
                R1
                R2
                PC
                PSW
    Stack Bottom: (higher memory address)  */

    MOV.L   16[R1],R3                           // Pickup end of stack area
    BCLR    #0, R3                              // mask for 4-byte alignment
    BCLR    #1, R3

    /* Build the stack frame.  */

    MOV.L   #30000h, R4                         // make initial PSW (user stack, enable interrupts)
    BTST    #0,152[R1]                          // in user mode?
    BMC     #20,R4                              // if user mode, set mode bit of initial PSW
    MOV.L   R4, [-R3]                           // initial PSW
    MOV.L   R2, [-R3]                           // initial PC
    
    MOV.L   8[R1], R4                           // Pickup thread entry info pointer, which is in the stack pointer position of the thread control block.
                                                //   It was setup in the txm_module_manager_thread_create function. It will be overwritten later in this 
                                                //   function with the actual, initial stack pointer.
    MOV.L   R4,[-R3]                            // initial R2, which is the module entry information.
    MOV.L   R1,[-R3]                            // initial R1, which is the thread control block.
    MOV.L   #0, R2                              // Clear following registers to 0
    MOV.L   R2,[-R3]                            // initial R5
    MOV.L   R2,[-R3]                            // initial R4
    MOV.L   R2,[-R3]                            // initial R3
    MOV.L   R2,[-R3]                            // initial R15
    MOV.L   R2,[-R3]                            // initial R14
    MVFC    FPSW, R2                            // pickup FPSW
    MOV.L   R2, [-R3]                           // initial FPSW
    MOV.L   12[R4], R2                          // Pickup code base register from the module information
    MOV.L   R2,[-R3]                            // initial R13
    MOV.L   8[R4], R2                           // Pickup data base register from the module information
    MOV.L   R2,[-R3]                            // initial R12
    MOV.L   #0, R4                              // Clear following registers to 0
    MOV.L   R4,[-R3]                            // initial R11
    MOV.L   R4,[-R3]                            // initial R10
    MOV.L   R4,[-R3]                            // initial R9
    MOV.L   R4,[-R3]                            // initial R8
    MOV.L   R4,[-R3]                            // initial R7
    MOV.L   R4,[-R3]                            // initial R6
    MOV.L   R4,[-R3]                            // Accumulator 1
    MOV.L   R4,[-R3]
    MOV.L   R4,[-R3]
    MOV.L   R4,[-R3]                            // Accumulator 0
    MOV.L   R4,[-R3]
    MOV.L   R4,[-R3]
    MOV.L   #1, R4                              // Make this an interrupt stack frame
    MOV.L   R4,[-R3]

    /* Save stack pointer.  */
    // thread_ptr -> tx_thread_stack_ptr =  R3;

    MOV.L   R3, 8[R1]                           // store initial SP in thread control block
    RTS
// }
    END
