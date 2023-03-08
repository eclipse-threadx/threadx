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
/*  03-08-2023      Scott Larson            Initial Version 6.2.1         */
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

    LOAD    t0, 4*REGBYTES(a0)                          // Pickup end of stack area
    li      t1, ~15                                     // Build 16-byte alignment mask
    and     t0, t0, t1                                  // Make sure 16-byte alignment

    /* Actually build the stack frame.  */

#if defined(__riscv_float_abi_single) || defined(__riscv_float_abi_double)
    addi    t0, t0, -65*REGBYTES
#else
    addi    t0, t0, -32*REGBYTES                        // Allocate space for the stack frame
#endif
    li      t1, 1                                       // Build stack type
    STORE   t1,  0*REGBYTES(t0)                         // Place stack type on the top
    STORE   x0,  1*REGBYTES(t0)                         // Initial s11
    STORE   x0,  2*REGBYTES(t0)                         // Initial s10
    STORE   x0,  3*REGBYTES(t0)                         // Initial s9
    STORE   x0,  4*REGBYTES(t0)                         // Initial s8
    STORE   x0,  5*REGBYTES(t0)                         // Initial s7
    STORE   x0,  6*REGBYTES(t0)                         // Initial s6
    STORE   x0,  7*REGBYTES(t0)                         // Initial s5
    STORE   x0,  8*REGBYTES(t0)                         // Initial s4
    STORE   x0,  9*REGBYTES(t0)                         // Initial s3
    STORE   x0, 10*REGBYTES(t0)                         // Initial s2
    STORE   x0, 11*REGBYTES(t0)                         // Initial s1
    STORE   x0, 12*REGBYTES(t0)                         // Initial s0
    STORE   x0, 13*REGBYTES(t0)                         // Initial t6
    STORE   x0, 14*REGBYTES(t0)                         // Initial t5
    STORE   x0, 15*REGBYTES(t0)                         // Initial t4
    STORE   x0, 16*REGBYTES(t0)                         // Initial t3
    STORE   x0, 17*REGBYTES(t0)                         // Initial t2
    STORE   x0, 18*REGBYTES(t0)                         // Initial t1
    STORE   x0, 19*REGBYTES(t0)                         // Initial t0
    STORE   x0, 20*REGBYTES(t0)                         // Initial a7
    STORE   x0, 21*REGBYTES(t0)                         // Initial a6
    STORE   x0, 22*REGBYTES(t0)                         // Initial a5
    STORE   x0, 23*REGBYTES(t0)                         // Initial a4
    STORE   x0, 24*REGBYTES(t0)                         // Initial a3
    STORE   x0, 25*REGBYTES(t0)                         // Initial a2
    STORE   x0, 26*REGBYTES(t0)                         // Initial a1
    STORE   x0, 27*REGBYTES(t0)                         // Initial a0
    STORE   x0, 28*REGBYTES(t0)                         // Initial ra
    STORE   a1, 30*REGBYTES(t0)                         // Initial mepc
#if defined(__riscv_float_abi_single) || defined(__riscv_float_abi_double)
    STORE   x0, 31*REGBYTES(t0)                         // Inital ft0
    STORE   x0, 32*REGBYTES(t0)                         // Inital ft1
    STORE   x0, 33*REGBYTES(t0)                         // Inital ft2
    STORE   x0, 34*REGBYTES(t0)                         // Inital ft3
    STORE   x0, 35*REGBYTES(t0)                         // Inital ft4
    STORE   x0, 36*REGBYTES(t0)                         // Inital ft5
    STORE   x0, 37*REGBYTES(t0)                         // Inital ft6
    STORE   x0, 38*REGBYTES(t0)                         // Inital ft7
    STORE   x0, 39*REGBYTES(t0)                         // Inital fs0
    STORE   x0, 40*REGBYTES(t0)                         // Inital fs1
    STORE   x0, 41*REGBYTES(t0)                         // Inital fa0
    STORE   x0, 42*REGBYTES(t0)                         // Inital fa1
    STORE   x0, 43*REGBYTES(t0)                         // Inital fa2
    STORE   x0, 44*REGBYTES(t0)                         // Inital fa3
    STORE   x0, 45*REGBYTES(t0)                         // Inital fa4
    STORE   x0, 46*REGBYTES(t0)                         // Inital fa5
    STORE   x0, 47*REGBYTES(t0)                         // Inital fa6
    STORE   x0, 48*REGBYTES(t0)                         // Inital fa7
    STORE   x0, 49*REGBYTES(t0)                         // Inital fs2
    STORE   x0, 50*REGBYTES(t0)                         // Inital fs3
    STORE   x0, 51*REGBYTES(t0)                         // Inital fs4
    STORE   x0, 52*REGBYTES(t0)                         // Inital fs5
    STORE   x0, 53*REGBYTES(t0)                         // Inital fs6
    STORE   x0, 54*REGBYTES(t0)                         // Inital fs7
    STORE   x0, 55*REGBYTES(t0)                         // Inital fs8
    STORE   x0, 56*REGBYTES(t0)                         // Inital fs9
    STORE   x0, 57*REGBYTES(t0)                         // Inital fs10
    STORE   x0, 58*REGBYTES(t0)                         // Inital fs11
    STORE   x0, 59*REGBYTES(t0)                         // Inital ft8
    STORE   x0, 60*REGBYTES(t0)                         // Inital ft9
    STORE   x0, 61*REGBYTES(t0)                         // Inital ft10
    STORE   x0, 62*REGBYTES(t0)                         // Inital ft11
    csrr    a1, fcsr                                    // Read fcsr and use it for initial value for each thread
    STORE   a1, 63*REGBYTES(t0)                         // Initial fscr
    STORE   x0, 64*REGBYTES(t0)                         // Reserved word (0)
#else
    STORE   x0, 31*REGBYTES(t0)                         // Reserved word (0)
#endif

    /* Setup stack pointer.  */
    /* thread_ptr -> tx_thread_stack_ptr =  t0;  */

    STORE   t0, 2*REGBYTES(a0)                          // Save stack pointer in thread's
    ret                                                 //   control block and return
/* }  */
