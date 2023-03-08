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


    INITIAL_SR  =   0xFF03                  # Interrupt enable previous

    .text
    .set    noreorder
/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _tx_thread_stack_build                        MIPS32_interAptiv/GNU */
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
    .globl  _tx_thread_stack_build
_tx_thread_stack_build:


    /* Build a fake interrupt frame.  The form of the fake interrupt stack
       on the MIPS32_interAptiv should look like the following after it is built:

       Stack Top:      1       (00)    Interrupt stack frame type
                       $30     (04)    Initial S8
                       $23     (08)    Initial S7
                       $22     (12)    Initial S6
                       $21     (16)    Initial S5
                       $20     (20)    Initial S4
                       $19     (24)    Initial S3
                       $18     (28)    Initial S2
                       $17     (32)    Initial S1
                       $16     (36)    Initial S0
                       hi      (40)    Initial HI register
                       lo      (44)    Initial LO register
                       $25     (48)    Initial t9
                       $24     (52)    Initial t8
                       $15     (56)    Initial t7
                       $14     (60)    Initial t6
                       $13     (64)    Initial t5
                       $12     (68)    Initial t4
                       $11     (72)    Initial t3
                       $10     (76)    Initial t2
                       $9      (80)    Initial t1
                       $8      (84)    Initial t0
                       $7      (88)    Initial a3
                       $6      (92)    Initial a2
                       $5      (96)    Initial a1
                       $4      (100)   Initial a0
                       $3      (104)   Initial v1
                       $2      (108)   Initial v0
                       $1      (112)   Initial at
                       $31     (116)   Initial ra
                       SR      (120)   Initial SR
                       EPC     (124)   Initial EPC

#ifdef TX_ENABLE_64BIT_FPU_SUPPORT

                       $f31    (128)   Initial f31
                       $f30    (136)   Initial f30
                       $f29    (144)   Initial f29
                       $f28    (152)   Initial f28
                       $f27    (160)   Initial f27
                       $f26    (168)   Initial f26
                       $f25    (176)   Initial f25
                       $f24    (184)   Initial f24
                       $f23    (192)   Initial f23
                       $f22    (200)   Initial f22
                       $f21    (208)   Initial f21
                       $f20    (216)   Initial f20
                       $f19    (224)   Initial f19
                       $f18    (232)   Initial f18
                       $f17    (240)   Initial f17
                       $f16    (248)   Initial f16
                       $f15    (256)   Initial f15
                       $f14    (264)   Initial f14
                       $f13    (272)   Initial f13
                       $f12    (280)   Initial f12
                       $f11    (288)   Initial f11
                       $f10    (296)   Initial f10
                       $f9     (304)   Initial f9
                       $f8     (312)   Initial f8
                       $f7     (320)   Initial f7
                       $f6     (328)   Initial f6
                       $f5     (336)   Initial f5
                       $f4     (344)   Initial f4
                       $f3     (352)   Initial f3
                       $f2     (360)   Initial f2
                       $f1     (368)   Initial f1
                       $f0     (376)   Initial f0
                       FCR31   (384)   Initial fcr31
                       unused  (388)   Unused Word
#endif

    Stack Bottom: (higher memory address)  */

    lw      $8, 16($4)                      # Pickup end of stack area
    ori     $9, $0, 7                       # Build double alignment mask
    not     $9, $9                          #
    and     $8, $8, $9                      # Make sure double word alignment

    /* Actually build the stack frame.  */

    subu    $8, $8, 392                     # Allocate space for the stack frame
    ori     $9, $0, 1                       # Build stack type
    sw      $9, ($8)                        # Place stack type on the top
    sw      $0, 4($8)                       # Initial s8
    sw      $0, 8($8)                       # Initial s7
    sw      $0, 12($8)                      # Initial s6
    sw      $0, 16($8)                      # Initial s5
    sw      $0, 20($8)                      # Initial s4
    sw      $0, 24($8)                      # Initial s3
    sw      $0, 28($8)                      # Initial s2
    sw      $0, 32($8)                      # Initial s1
    sw      $0, 36($8)                      # Initial s0
    sw      $0, 40($8)                      # Initial hi
    sw      $0, 44($8)                      # Initial lo
    sw      $0, 48($8)                      # Initial t9
    sw      $0, 52($8)                      # Initial t8
    sw      $0, 56($8)                      # Initial t7
    sw      $0, 60($8)                      # Initial t6
    sw      $0, 64($8)                      # Initial t5
    sw      $0, 68($8)                      # Initial t4
    sw      $0, 72($8)                      # Initial t3
    sw      $0, 76($8)                      # Initial t2
    sw      $0, 80($8)                      # Initial t1
    sw      $0, 84($8)                      # Initial t0
    sw      $0, 88($8)                      # Initial a3
    sw      $0, 92($8)                      # Initial a2
    sw      $0, 96($8)                      # Initial a1
    sw      $0, 100($8)                     # Initial a0
    sw      $0, 104($8)                     # Initial v1
    sw      $0, 108($8)                     # Initial v0
    sw      $0, 112($8)                     # Initial at
    sw      $0, 116($8)                     # Initial ra
    mfc0    $10, $12                        # Pickup current SR
    li      $9,0xDFFFFFFC                   # Preserve upper portion of SR - except for FP
    and     $9, $10, $9                     # Clear the lower SR bits
    ori     $9, $9, INITIAL_SR              # Build initial SR
    sw      $9, 120($8)                     # Initial SR
    sw      $5, 124($8)                     # Initial EPC

#ifdef TX_ENABLE_64BIT_FPU_SUPPORT
    sw      $0, 128($8)                     # Initial f31
    sw      $0, 132($8)                     #
    sw      $0, 136($8)                     # Initial f30
    sw      $0, 140($8)                     #
    sw      $0, 144($8)                     # Initial f29
    sw      $0, 148($8)                     #
    sw      $0, 152($8)                     # Initial f28
    sw      $0, 156($8)                     #
    sw      $0, 160($8)                     # Initial f27
    sw      $0, 164($8)                     #
    sw      $0, 168($8)                     # Initial f26
    sw      $0, 172($8)                     #
    sw      $0, 176($8)                     # Initial f25
    sw      $0, 180($8)                     #
    sw      $0, 184($8)                     # Initial f24
    sw      $0, 188($8)                     #
    sw      $0, 192($8)                     # Initial f23
    sw      $0, 196($8)                     #
    sw      $0, 200($8)                     # Initial f22
    sw      $0, 204($8)                     #
    sw      $0, 208($8)                     # Initial f21
    sw      $0, 212($8)                     #
    sw      $0, 216($8)                     # Initial f20
    sw      $0, 220($8)                     #
    sw      $0, 224($8)                     # Initial f19
    sw      $0, 228($8)                     #
    sw      $0, 232($8)                     # Initial f18
    sw      $0, 236($8)                     #
    sw      $0, 240($8)                     # Initial f17
    sw      $0, 244($8)                     #
    sw      $0, 248($8)                     # Initial f16
    sw      $0, 252($8)                     #
    sw      $0, 256($8)                     # Initial f15
    sw      $0, 260($8)                     #
    sw      $0, 264($8)                     # Initial f14
    sw      $0, 268($8)                     #
    sw      $0, 272($8)                     # Initial f13
    sw      $0, 276($8)                     #
    sw      $0, 280($8)                     # Initial f12
    sw      $0, 284($8)                     #
    sw      $0, 288($8)                     # Initial f11
    sw      $0, 292($8)                     #
    sw      $0, 296($8)                     # Initial f10
    sw      $0, 300($8)                     #
    sw      $0, 304($8)                     # Initial f9
    sw      $0, 308($8)                     #
    sw      $0, 312($8)                     # Initial f8
    sw      $0, 316($8)                     #
    sw      $0, 320($8)                     # Initial f7
    sw      $0, 324($8)                     #
    sw      $0, 328($8)                     # Initial f6
    sw      $0, 332($8)                     #
    sw      $0, 336($8)                     # Initial f5
    sw      $0, 340($8)                     #
    sw      $0, 344($8)                     # Initial f4
    sw      $0, 348($8)                     #
    sw      $0, 352($8)                     # Initial f3
    sw      $0, 356($8)                     #
    sw      $0, 360($8)                     # Initial f2
    sw      $0, 364($8)                     #
    sw      $0, 368($8)                     # Initial f1
    sw      $0, 372($8)                     #
    la      $9, _tx_thread_initial_fpu_control_register
    lw      $10, ($9)                       # Pickup initial FPU control registrer
    sw      $0, 376($8)                     # Initial f0
    sw      $0, 380($8)                     #
    sw      $10, 384($8)                    # Inherit initial fcr31
#endif

    /* Set bit indicating the thread is ready for scheduling.  */

    lw      $9, 152($4)                     # Pickup the thread's VPE control register
    ori     $9, $9, 0x8000                  # Set ready bit (bit 15)
    sw      $9, 152($4)                     # Make this thread ready for scheduling

    /* Setup stack pointer.  */
    /* thread_ptr -> tx_thread_stack_ptr =  t0;  */

    j       $31                             # Return to caller
    sw      $8, 8($4)                       # Save stack pointer in thread's
                                        /*  #   control block  */
/* }  */

