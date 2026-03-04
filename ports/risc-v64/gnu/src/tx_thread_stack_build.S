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
/**   Thread                                                              */
/**                                                                       */
/**************************************************************************/
/**************************************************************************/

    .section .text
/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _tx_thread_stack_build                             RISC-V64/GNU     */
/*                                                           6.2.1        */
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
/*    function_ptr                          Pointer to return function    */
/*                                                                        */
/*  OUTPUT                                                                 */
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
                    f0      31      Inital ft0
                    f1      32      Inital ft1
                    f2      33      Inital ft2
                    f3      34      Inital ft3
                    f4      35      Inital ft4
                    f5      36      Inital ft5
                    f6      37      Inital ft6
                    f7      38      Inital ft7
                    f8      39      Inital fs0
                    f9      40      Inital fs1
                    f10     41      Inital fa0
                    f11     42      Inital fa1
                    f12     43      Inital fa2
                    f13     44      Inital fa3
                    f14     45      Inital fa4
                    f15     46      Inital fa5
                    f16     47      Inital fa6
                    f17     48      Inital fa7
                    f18     49      Inital fs2
                    f19     50      Inital fs3
                    f20     51      Inital fs4
                    f21     52      Inital fs5
                    f22     53      Inital fs6
                    f23     54      Inital fs7
                    f24     55      Inital fs8
                    f25     56      Inital fs9
                    f26     57      Inital fs10
                    f27     58      Inital fs11
                    f28     59      Inital ft8
                    f29     60      Inital ft9
                    f30     61      Inital ft10
                    f31     62      Inital ft11
                    fscr    63      Inital fscr

    Stack Bottom: (higher memory address)  */

    ld      t0, 32(a0)                                  // Pickup end of stack area
    li      t1, ~15                                     // Build 16-byte alignment mask
    and     t0, t0, t1                                  // Make sure 16-byte alignment

    /* Actually build the stack frame.  */

#if defined(__riscv_float_abi_single) || defined(__riscv_float_abi_double)
    addi    t0, t0, -65*8
#else
    addi    t0, t0, -32*8                               // Allocate space for the stack frame
#endif
    li      t1, 1                                       // Build stack type
    sd      t1,  0*8(t0)                                // Place stack type on the top
    sd      zero,  1*8(t0)                              // Initial s11
    sd      zero,  2*8(t0)                              // Initial s10
    sd      zero,  3*8(t0)                              // Initial s9
    sd      zero,  4*8(t0)                              // Initial s8
    sd      zero,  5*8(t0)                              // Initial s7
    sd      zero,  6*8(t0)                              // Initial s6
    sd      zero,  7*8(t0)                              // Initial s5
    sd      zero,  8*8(t0)                              // Initial s4
    sd      zero,  9*8(t0)                              // Initial s3
    sd      zero, 10*8(t0)                              // Initial s2
    sd      zero, 11*8(t0)                              // Initial s1
    sd      zero, 12*8(t0)                              // Initial s0
    sd      zero, 13*8(t0)                              // Initial t6
    sd      zero, 14*8(t0)                              // Initial t5
    sd      zero, 15*8(t0)                              // Initial t4
    sd      zero, 16*8(t0)                              // Initial t3
    sd      zero, 17*8(t0)                              // Initial t2
    sd      zero, 18*8(t0)                              // Initial t1
    sd      zero, 19*8(t0)                              // Initial t0
    sd      zero, 20*8(t0)                              // Initial a7
    sd      zero, 21*8(t0)                              // Initial a6
    sd      zero, 22*8(t0)                              // Initial a5
    sd      zero, 23*8(t0)                              // Initial a4
    sd      zero, 24*8(t0)                              // Initial a3
    sd      zero, 25*8(t0)                              // Initial a2
    sd      zero, 26*8(t0)                              // Initial a1
    sd      zero, 27*8(t0)                              // Initial a0
    sd      zero, 28*8(t0)                              // Initial ra
    sd      a1, 30*8(t0)                                // Initial mepc (thread entry point)
#if defined(__riscv_float_abi_single) || defined(__riscv_float_abi_double)
    sd      zero, 31*8(t0)                              // Initial ft0
    sd      zero, 32*8(t0)                              // Initial ft1
    sd      zero, 33*8(t0)                              // Initial ft2
    sd      zero, 34*8(t0)                              // Initial ft3
    sd      zero, 35*8(t0)                              // Initial ft4
    sd      zero, 36*8(t0)                              // Initial ft5
    sd      zero, 37*8(t0)                              // Initial ft6
    sd      zero, 38*8(t0)                              // Initial ft7
    sd      zero, 39*8(t0)                              // Initial fs0
    sd      zero, 40*8(t0)                              // Initial fs1
    sd      zero, 41*8(t0)                              // Initial fa0
    sd      zero, 42*8(t0)                              // Initial fa1
    sd      zero, 43*8(t0)                              // Initial fa2
    sd      zero, 44*8(t0)                              // Initial fa3
    sd      zero, 45*8(t0)                              // Initial fa4
    sd      zero, 46*8(t0)                              // Initial fa5
    sd      zero, 47*8(t0)                              // Initial fa6
    sd      zero, 48*8(t0)                              // Initial fa7
    sd      zero, 49*8(t0)                              // Initial fs2
    sd      zero, 50*8(t0)                              // Initial fs3
    sd      zero, 51*8(t0)                              // Initial fs4
    sd      zero, 52*8(t0)                              // Initial fs5
    sd      zero, 53*8(t0)                              // Initial fs6
    sd      zero, 54*8(t0)                              // Initial fs7
    sd      zero, 55*8(t0)                              // Initial fs8
    sd      zero, 56*8(t0)                              // Initial fs9
    sd      zero, 57*8(t0)                              // Initial fs10
    sd      zero, 58*8(t0)                              // Initial fs11
    sd      zero, 59*8(t0)                              // Initial ft8
    sd      zero, 60*8(t0)                              // Initial ft9
    sd      zero, 61*8(t0)                              // Initial ft10
    sd      zero, 62*8(t0)                              // Initial ft11
    csrr    a1, fcsr                                    // Read fcsr for initial value
    sd      a1, 63*8(t0)                                // Initial fcsr
    sd      zero, 64*8(t0)                              // Reserved word (0)
#else
    sd      zero, 31*8(t0)                              // Reserved word (0)
#endif

    /* Setup stack pointer.  */
    /* thread_ptr -> tx_thread_stack_ptr =  t0;  */

    sd      t0, 16(a0)                                  // Save stack pointer in thread's
    ret                                                 //   control block and return
/* }  */
