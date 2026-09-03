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

#if defined(__riscv_float_abi_double)
    .equ    TX_TRAP_FRAME_SIZE, 400
#else
    .equ    TX_TRAP_FRAME_SIZE, 128
#endif

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
                    mstatus 29      Initial mstatus seed 
                    mepc    30      Initial mepc
                    --      31      pad (0)
        If floating point support (double ABI), byte offsets from 
        the frame base:
                    f0-f31  128 + i*8   Initial FP registers (all 0)
                    fcsr    384         Initial fcsr (0)
                    pad     388-399     (0)

    Stack Bottom: (higher memory address)  */

    lw      t0, 16(a0)                                  // Pickup end of stack area
    li      t1, ~15                                     // Build 16-byte alignment mask
    and     t0, t0, t1                                  // Make sure 16-byte alignment

    /* Actually build the stack frame.  */

    addi    t0, t0, -TX_TRAP_FRAME_SIZE                 // Allocate space for the stack frame
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
    sw      zero, 31*4(t0)                              // Pad word (0)
#if defined(__riscv_float_abi_double)
    li      t1, 0x6000                                  // mstatus seed: FS = Dirty (bits 14:13 = 11)
    sw      t1, 29*4(t0)                                //   so the first dispatch restores the clean FP state

    /* Zero the whole FP area: f0-f31 double slots (128-383), fcsr (384)
       and the pad words up to the frame end.  Threads start with clean
       FP registers and fcsr = 0.  */
    addi    t1, t0, 32*4                                // First FP-area word (offset 128)
    addi    t2, t0, TX_TRAP_FRAME_SIZE                  // One past the frame end (offset 400)
_tx_thread_stack_build_fp_zero:
    sw      zero, 0(t1)                                 // Clear FP-area word
    addi    t1, t1, 4
    bne     t1, t2, _tx_thread_stack_build_fp_zero
#else
    sw      zero, 29*4(t0)                              // Initial mstatus slot (0, no FP gate)
#endif

    /* Setup stack pointer.  */
    /* thread_ptr -> tx_thread_stack_ptr =  t0;  */

    sw      t0, 8(a0)                                   // Save stack pointer in thread's
    ret                                                 //   control block and return
/* }  */
