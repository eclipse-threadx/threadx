/***************************************************************************
 * Copyright (c) 2025 10xEngineers
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
/**   Thread                                                              */
/**                                                                       */
/**************************************************************************/
/**************************************************************************/


    .section .text
/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _tx_thread_stack_build                             RISC-V32/GNU     */
/*                                                           6.4.x        */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Akif Ejaz, 10xEngineers                                             */
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
/*    function_ptr                          Pointer to return function    */
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
/**************************************************************************/
/* VOID   _tx_thread_stack_build(TX_THREAD *thread_ptr, VOID (*function_ptr)(VOID))
{  */
    .global  _tx_thread_stack_build
_tx_thread_stack_build:

    /* Build a fake interrupt frame.  The form of the fake interrupt stack
       on the RISC-V should look like the following after it is built:
                    Reg     Index
       Stack Top:   1        0      Interrupt stack frame type
                    x27      1      Initial s11
                    x26      2      Initial s10
                    x25      3      Initial s9
                    x24      4      Initial s8
                    x23      5      Initial s7
                    x22      6      Initial s6
                    x21      7      Initial s5
                    x20      8      Initial s4
                    x19      9      Initial s3
                    x18     10      Initial s2
                    x9      11      Initial s1
                    x8      12      Initial s0
                    x31     13      Initial t6
                    x30     14      Initial t5
                    x29     15      Initial t4
                    x28     16      Initial t3
                    x7      17      Initial t2
                    x6      18      Initial t1
                    x5      19      Initial t0
                    x17     20      Initial a7
                    x16     21      Initial a6
                    x15     22      Initial a5
                    x14     23      Initial a4
                    x13     24      Initial a3
                    x12     25      Initial a2
                    x11     26      Initial a1
                    x10     27      Initial a0
                    x1      28      Initial ra
                    --      29      reserved
                    mepc    30      Initial mepc
If floating point support:
                    f0      31      Initial ft0
                    f1      32      Initial ft1
                    f2      33      Initial ft2
                    f3      34      Initial ft3
                    f4      35      Initial ft4
                    f5      36      Initial ft5
                    f6      37      Initial ft6
                    f7      38      Initial ft7
                    f8      39      Initial fs0
                    f9      40      Initial fs1
                    f10     41      Initial fa0
                    f11     42      Initial fa1
                    f12     43      Initial fa2
                    f13     44      Initial fa3
                    f14     45      Initial fa4
                    f15     46      Initial fa5
                    f16     47      Initial fa6
                    f17     48      Initial fa7
                    f18     49      Initial fs2
                    f19     50      Initial fs3
                    f20     51      Initial fs4
                    f21     52      Initial fs5
                    f22     53      Initial fs6
                    f23     54      Initial fs7
                    f24     55      Initial fs8
                    f25     56      Initial fs9
                    f26     57      Initial fs10
                    f27     58      Initial fs11
                    f28     59      Initial ft8
                    f29     60      Initial ft9
                    f30     61      Initial ft10
                    f31     62      Initial ft11
                    fscr    63      Initial fscr

    Stack Bottom: (higher memory address)  */

    lw      t0, 16(a0)                                  // Pickup end of stack area
    li      t1, ~15                                     // Build 16-byte alignment mask
    and     t0, t0, t1                                  // Make sure 16-byte alignment

    /* Actually build the stack frame.  */

#if defined(__riscv_float_abi_single) || defined(__riscv_float_abi_double)
    addi    t0, t0, -65*4
#else
    addi    t0, t0, -32*4                               // Allocate space for the stack frame
#endif
    li      t1, 1                                       // Build stack type
    sw      t1,  0*4(t0)                                // Place stack type on the top
    sw      zero,  1*4(t0)                              // Initial s11
    sw      zero,  2*4(t0)                              // Initial s10
    sw      zero,  3*4(t0)                              // Initial s9
    sw      zero,  4*4(t0)                              // Initial s8
    sw      zero,  5*4(t0)                              // Initial s7
    sw      zero,  6*4(t0)                              // Initial s6
    sw      zero,  7*4(t0)                              // Initial s5
    sw      zero,  8*4(t0)                              // Initial s4
    sw      zero,  9*4(t0)                              // Initial s3
    sw      zero, 10*4(t0)                              // Initial s2
    sw      zero, 11*4(t0)                              // Initial s1
    sw      zero, 12*4(t0)                              // Initial s0
    sw      zero, 13*4(t0)                              // Initial t6
    sw      zero, 14*4(t0)                              // Initial t5
    sw      zero, 15*4(t0)                              // Initial t4
    sw      zero, 16*4(t0)                              // Initial t3
    sw      zero, 17*4(t0)                              // Initial t2
    sw      zero, 18*4(t0)                              // Initial t1
    sw      zero, 19*4(t0)                              // Initial t0
    sw      zero, 20*4(t0)                              // Initial a7
    sw      zero, 21*4(t0)                              // Initial a6
    sw      zero, 22*4(t0)                              // Initial a5
    sw      zero, 23*4(t0)                              // Initial a4
    sw      zero, 24*4(t0)                              // Initial a3
    sw      zero, 25*4(t0)                              // Initial a2
    sw      zero, 26*4(t0)                              // Initial a1
    sw      zero, 27*4(t0)                              // Initial a0
    sw      zero, 28*4(t0)                              // Initial ra
    sw      a1, 30*4(t0)                                // Initial mepc (thread entry point)
#if defined(__riscv_float_abi_single) || defined(__riscv_float_abi_double)
    sw      zero, 31*4(t0)                              // Initial ft0
    sw      zero, 32*4(t0)                              // Initial ft1
    sw      zero, 33*4(t0)                              // Initial ft2
    sw      zero, 34*4(t0)                              // Initial ft3
    sw      zero, 35*4(t0)                              // Initial ft4
    sw      zero, 36*4(t0)                              // Initial ft5
    sw      zero, 37*4(t0)                              // Initial ft6
    sw      zero, 38*4(t0)                              // Initial ft7
    sw      zero, 39*4(t0)                              // Initial fs0
    sw      zero, 40*4(t0)                              // Initial fs1
    sw      zero, 41*4(t0)                              // Initial fa0
    sw      zero, 42*4(t0)                              // Initial fa1
    sw      zero, 43*4(t0)                              // Initial fa2
    sw      zero, 44*4(t0)                              // Initial fa3
    sw      zero, 45*4(t0)                              // Initial fa4
    sw      zero, 46*4(t0)                              // Initial fa5
    sw      zero, 47*4(t0)                              // Initial fa6
    sw      zero, 48*4(t0)                              // Initial fa7
    sw      zero, 49*4(t0)                              // Initial fs2
    sw      zero, 50*4(t0)                              // Initial fs3
    sw      zero, 51*4(t0)                              // Initial fs4
    sw      zero, 52*4(t0)                              // Initial fs5
    sw      zero, 53*4(t0)                              // Initial fs6
    sw      zero, 54*4(t0)                              // Initial fs7
    sw      zero, 55*4(t0)                              // Initial fs8
    sw      zero, 56*4(t0)                              // Initial fs9
    sw      zero, 57*4(t0)                              // Initial fs10
    sw      zero, 58*4(t0)                              // Initial fs11
    sw      zero, 59*4(t0)                              // Initial ft8
    sw      zero, 60*4(t0)                              // Initial ft9
    sw      zero, 61*4(t0)                              // Initial ft10
    sw      zero, 62*4(t0)                              // Initial ft11
    csrr    a1, fcsr                                    // Read fcsr for initial value
    sw      a1, 63*4(t0)                                // Initial fcsr
    sw      zero, 64*4(t0)                              // Reserved word (0)
#else
    sw      zero, 31*4(t0)                              // Reserved word (0)
#endif

    /* Setup stack pointer.  */
    /* thread_ptr -> tx_thread_stack_ptr =  t0;  */

    sw      t0, 8(a0)                                   // Save stack pointer in thread's
    ret                                                 //   control block and return
/* }  */
