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
/**   Thread                                                              */
/**                                                                       */
/**************************************************************************/
/**************************************************************************/

#include "tx_port.h"

    .section .text
/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _tx_thread_context_save                            RISC-V64/GNU     */
/*                                                           6.2.1        */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Scott Larson, Microsoft Corporation                                 */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function saves the context of an executing thread in the       */
/*    beginning of interrupt processing.  The function also ensures that  */
/*    the system stack is used upon return to the calling ISR.            */
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
/*    ISRs                                                                */
/*                                                                        */
/*  RELEASE HISTORY                                                       */
/*                                                                        */
/*    DATE              NAME                      DESCRIPTION             */
/*                                                                        */
/*  03-08-2023      Scott Larson            Initial Version 6.2.1         */
/*                                                                        */
/**************************************************************************/
/* VOID   _tx_thread_context_save(VOID)
{  */
    .global      _tx_thread_context_save
_tx_thread_context_save:

    /* Upon entry to this routine, it is assumed that interrupts are locked
       out and the interrupt stack fame has been allocated and x1 (ra) has
       been saved on the stack. */

    STORE   x5, 19*REGBYTES(sp)                         // First store t0 and t1
    STORE   x6, 18*REGBYTES(sp)

    la      x5, _tx_thread_system_state                 // Pickup address of system state
    LOAD    x6, 0(x5)                                   // Pickup system state

    /* Check for a nested interrupt condition.  */
    /* if (_tx_thread_system_state++)
    {  */
    beqz    x6, _tx_thread_not_nested_save              // If 0, first interrupt condition
    addi    x6, x6, 1                                   // Increment the interrupt counter
    STORE   x6, 0(x5)                                   // Store the interrupt counter

    /* Nested interrupt condition.
       Save the reset of the scratch registers on the stack and return to the
       calling ISR.  */

    STORE   x7,  17*REGBYTES(sp)                        // Store t2
    STORE   x8,  12*REGBYTES(sp)                        // Store s0
    STORE   x10, 27*REGBYTES(sp)                        // Store a0
    STORE   x11, 26*REGBYTES(sp)                        // Store a1
    STORE   x12, 25*REGBYTES(sp)                        // Store a2
    STORE   x13, 24*REGBYTES(sp)                        // Store a3
    STORE   x14, 23*REGBYTES(sp)                        // Store a4
    STORE   x15, 22*REGBYTES(sp)                        // Store a5
    STORE   x16, 21*REGBYTES(sp)                        // Store a6
    STORE   x17, 20*REGBYTES(sp)                        // Store a7
    STORE   x28, 16*REGBYTES(sp)                        // Store t3
    STORE   x29, 15*REGBYTES(sp)                        // Store t4
    STORE   x30, 14*REGBYTES(sp)                        // Store t5
    STORE   x31, 13*REGBYTES(sp)                        // Store t6
    csrr    t0, mepc                                    // Load exception program counter
    STORE   t0, 30*REGBYTES(sp)                         // Save it on the stack

    /* Save floating point scratch registers.  */
#if defined(__riscv_float_abi_single)
    fsw     f0, 31*REGBYTES(sp)                         // Store ft0
    fsw     f1, 32*REGBYTES(sp)                         // Store ft1
    fsw     f2, 33*REGBYTES(sp)                         // Store ft2
    fsw     f3, 34*REGBYTES(sp)                         // Store ft3
    fsw     f4, 35*REGBYTES(sp)                         // Store ft4
    fsw     f5, 36*REGBYTES(sp)                         // Store ft5
    fsw     f6, 37*REGBYTES(sp)                         // Store ft6
    fsw     f7, 38*REGBYTES(sp)                         // Store ft7
    fsw     f10,41*REGBYTES(sp)                         // Store fa0
    fsw     f11,42*REGBYTES(sp)                         // Store fa1
    fsw     f12,43*REGBYTES(sp)                         // Store fa2
    fsw     f13,44*REGBYTES(sp)                         // Store fa3
    fsw     f14,45*REGBYTES(sp)                         // Store fa4
    fsw     f15,46*REGBYTES(sp)                         // Store fa5
    fsw     f16,47*REGBYTES(sp)                         // Store fa6
    fsw     f17,48*REGBYTES(sp)                         // Store fa7
    fsw     f28,59*REGBYTES(sp)                         // Store ft8
    fsw     f29,60*REGBYTES(sp)                         // Store ft9
    fsw     f30,61*REGBYTES(sp)                         // Store ft10
    fsw     f31,62*REGBYTES(sp)                         // Store ft11
    csrr    t0, fcsr
    STORE   t0, 63*REGBYTES(sp)                         // Store fcsr
#elif defined(__riscv_float_abi_double)
    fsd     f0, 31*REGBYTES(sp)                         // Store ft0
    fsd     f1, 32*REGBYTES(sp)                         // Store ft1
    fsd     f2, 33*REGBYTES(sp)                         // Store ft2
    fsd     f3, 34*REGBYTES(sp)                         // Store ft3
    fsd     f4, 35*REGBYTES(sp)                         // Store ft4
    fsd     f5, 36*REGBYTES(sp)                         // Store ft5
    fsd     f6, 37*REGBYTES(sp)                         // Store ft6
    fsd     f7, 38*REGBYTES(sp)                         // Store ft7
    fsd     f10,41*REGBYTES(sp)                         // Store fa0
    fsd     f11,42*REGBYTES(sp)                         // Store fa1
    fsd     f12,43*REGBYTES(sp)                         // Store fa2
    fsd     f13,44*REGBYTES(sp)                         // Store fa3
    fsd     f14,45*REGBYTES(sp)                         // Store fa4
    fsd     f15,46*REGBYTES(sp)                         // Store fa5
    fsd     f16,47*REGBYTES(sp)                         // Store fa6
    fsd     f17,48*REGBYTES(sp)                         // Store fa7
    fsd     f28,59*REGBYTES(sp)                         // Store ft8
    fsd     f29,60*REGBYTES(sp)                         // Store ft9
    fsd     f30,61*REGBYTES(sp)                         // Store ft10
    fsd     f31,62*REGBYTES(sp)                         // Store ft11
    csrr    t0, fcsr
    STORE   t0, 63*REGBYTES(sp)                         // Store fcsr
#endif

#ifdef TX_ENABLE_EXECUTION_CHANGE_NOTIFY
    call    _tx_execution_isr_enter                     // Call the ISR execution enter function
#endif

    ret                                                 // Return to calling ISR

_tx_thread_not_nested_save:
    /* }  */

    /* Otherwise, not nested, check to see if a thread was running.  */
    /* else if (_tx_thread_current_ptr)
    {  */
    addi    x6, x6, 1                                   // Increment the interrupt counter
    STORE   x6, 0(x5)                                   // Store the interrupt counter

    /* Not nested: Find the user thread that was running and load our SP */

    LOAD    x5, _tx_thread_current_ptr                  // Pickup current thread pointer
    beqz    x5, _tx_thread_idle_system_save             // If NULL, idle system was interrupted

    /* Save the standard scratch registers.  */

    STORE   x7,  17*REGBYTES(sp)                        // Store t2
    STORE   x8,  12*REGBYTES(sp)                        // Store s0
    STORE   x10, 27*REGBYTES(sp)                        // Store a0
    STORE   x11, 26*REGBYTES(sp)                        // Store a1
    STORE   x12, 25*REGBYTES(sp)                        // Store a2
    STORE   x13, 24*REGBYTES(sp)                        // Store a3
    STORE   x14, 23*REGBYTES(sp)                        // Store a4
    STORE   x15, 22*REGBYTES(sp)                        // Store a5
    STORE   x16, 21*REGBYTES(sp)                        // Store a6
    STORE   x17, 20*REGBYTES(sp)                        // Store a7
    STORE   x28, 16*REGBYTES(sp)                        // Store t3
    STORE   x29, 15*REGBYTES(sp)                        // Store t4
    STORE   x30, 14*REGBYTES(sp)                        // Store t5
    STORE   x31, 13*REGBYTES(sp)                        // Store t6

    csrr    t0, mepc                                    // Load exception program counter
    STORE   t0, 30*REGBYTES(sp)                         // Save it on the stack

    /* Save floating point scratch registers.  */
#if defined(__riscv_float_abi_single)
    fsw     f0, 31*REGBYTES(sp)                         // Store ft0
    fsw     f1, 32*REGBYTES(sp)                         // Store ft1
    fsw     f2, 33*REGBYTES(sp)                         // Store ft2
    fsw     f3, 34*REGBYTES(sp)                         // Store ft3
    fsw     f4, 35*REGBYTES(sp)                         // Store ft4
    fsw     f5, 36*REGBYTES(sp)                         // Store ft5
    fsw     f6, 37*REGBYTES(sp)                         // Store ft6
    fsw     f7, 38*REGBYTES(sp)                         // Store ft7
    fsw     f10,41*REGBYTES(sp)                         // Store fa0
    fsw     f11,42*REGBYTES(sp)                         // Store fa1
    fsw     f12,43*REGBYTES(sp)                         // Store fa2
    fsw     f13,44*REGBYTES(sp)                         // Store fa3
    fsw     f14,45*REGBYTES(sp)                         // Store fa4
    fsw     f15,46*REGBYTES(sp)                         // Store fa5
    fsw     f16,47*REGBYTES(sp)                         // Store fa6
    fsw     f17,48*REGBYTES(sp)                         // Store fa7
    fsw     f28,59*REGBYTES(sp)                         // Store ft8
    fsw     f29,60*REGBYTES(sp)                         // Store ft9
    fsw     f30,61*REGBYTES(sp)                         // Store ft10
    fsw     f31,62*REGBYTES(sp)                         // Store ft11
    csrr    t0, fcsr
    STORE   t0, 63*REGBYTES(sp)                         // Store fcsr
#elif defined(__riscv_float_abi_double)
    fsd     f0, 31*REGBYTES(sp)                         // Store ft0
    fsd     f1, 32*REGBYTES(sp)                         // Store ft1
    fsd     f2, 33*REGBYTES(sp)                         // Store ft2
    fsd     f3, 34*REGBYTES(sp)                         // Store ft3
    fsd     f4, 35*REGBYTES(sp)                         // Store ft4
    fsd     f5, 36*REGBYTES(sp)                         // Store ft5
    fsd     f6, 37*REGBYTES(sp)                         // Store ft6
    fsd     f7, 38*REGBYTES(sp)                         // Store ft7
    fsd     f10,41*REGBYTES(sp)                         // Store fa0
    fsd     f11,42*REGBYTES(sp)                         // Store fa1
    fsd     f12,43*REGBYTES(sp)                         // Store fa2
    fsd     f13,44*REGBYTES(sp)                         // Store fa3
    fsd     f14,45*REGBYTES(sp)                         // Store fa4
    fsd     f15,46*REGBYTES(sp)                         // Store fa5
    fsd     f16,47*REGBYTES(sp)                         // Store fa6
    fsd     f17,48*REGBYTES(sp)                         // Store fa7
    fsd     f28,59*REGBYTES(sp)                         // Store ft8
    fsd     f29,60*REGBYTES(sp)                         // Store ft9
    fsd     f30,61*REGBYTES(sp)                         // Store ft10
    fsd     f31,62*REGBYTES(sp)                         // Store ft11
    csrr    t0, fcsr
    STORE   t0, 63*REGBYTES(sp)                         // Store fcsr
#endif

    /* Save the current stack pointer in the thread's control block.  */
    /* _tx_thread_current_ptr -> tx_thread_stack_ptr =  sp;  */

    /* Switch to the system stack.  */
    /* sp =  _tx_thread_system_stack_ptr;  */

    LOAD    t1, _tx_thread_current_ptr                  // Pickup current thread pointer
    STORE   sp, 2*REGBYTES(t1)                          // Save stack pointer

#ifdef TX_ENABLE_EXECUTION_CHANGE_NOTIFY
    /* _tx_execution_isr_enter is called with thread stack pointer */
    call    _tx_execution_isr_enter                     // Call the ISR execution enter function
#endif


    LOAD    sp, _tx_thread_system_stack_ptr             // Switch to system stack
    ret                                                 // Return to calling ISR

    /* }
    else
    {  */

_tx_thread_idle_system_save:


#ifdef TX_ENABLE_EXECUTION_CHANGE_NOTIFY
    call    _tx_execution_isr_enter                     // Call the ISR execution enter function
#endif

    /* Interrupt occurred in the scheduling loop.  */

    /* }
}  */
#if defined(__riscv_float_abi_single) || defined(__riscv_float_abi_double)
    addi    sp, sp, 65*REGBYTES                         // Recover stack frame - with floating point enabled
#else
    addi    sp, sp, 32*REGBYTES                         // Recover the reserved stack space
#endif
    ret                                                 // Return to calling ISR
