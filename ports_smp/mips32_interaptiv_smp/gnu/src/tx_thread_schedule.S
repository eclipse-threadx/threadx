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


#define UserLocal     $4,2
#define C0_TCBind     $2,2
#define C0_TCContext  $2,5
#define C0_TCHalt     $2,4


#ifdef TX_THREAD_SMP_WAKEUP_LOGIC
    .globl      TX_MIPS32_1004K_VPE_YIELD
#endif

    .text
    .set        noreorder
/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _tx_thread_schedule                           MIPS32_interAptiv/GNU */
/*                                                           6.2.1        */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Scott Larson, Microsoft Corporation                                 */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function waits for a thread control block pointer to appear in */
/*    the _tx_thread_execute_ptr variable.  Once a thread pointer appears */
/*    in the variable, the corresponding thread is resumed.               */
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
/*    _tx_initialize_kernel_enter          ThreadX entry function         */
/*    _tx_thread_system_return             Return to system from thread   */
/*    _tx_thread_context_restore           Restore thread's context       */
/*                                                                        */
/*  RELEASE HISTORY                                                       */
/*                                                                        */
/*    DATE              NAME                      DESCRIPTION             */
/*                                                                        */
/*  03-08-2023      Scott Larson            Initial Version 6.2.1         */
/*                                                                        */
/**************************************************************************/
/* VOID   _tx_thread_schedule(VOID)
{  */
    .globl  _tx_thread_schedule_idle_system
_tx_thread_schedule_idle_system:

#ifndef TX_THREAD_SMP_WAKEUP_LOGIC

    ei                                              # Enable interrupts
    ehb                                             #
    li      $11,-1                                  #
    yield   $11, $11                                # Yield so this VPE does not consume all the cycles
    lw      $8, ($9)                                # Pickup next thread to execute
    beq     $8, $0, _tx_thread_schedule_idle_system # If not-NULL, check the thread's ready bit
    nop                                             # Delay slot

#else

    la      $8, TX_MIPS32_1004K_VPE_YIELD           # Get call-out address
    jalr    $8                                      # Make the call
    or      $4, $0, $9                              # Pass the properly indexed _tx_thread_execute_ptr[x]

#endif

    .globl  _tx_thread_schedule
_tx_thread_schedule:


    /* Enable interrupts.  */

    ei                                          # Enable interrupts
    ehb                                         #

    /* Disable interrupts.  */

    di                                          # Disable interrupts
    ehb                                         #

    /* Pickup the executing VPE number.  */

    mfc0    $25, UserLocal                      # Pickup VPE ID
    sll     $25, $25, 2                         # Build index based on VPE number

    /* Calculate the execute pointer for this VPE.  */

    la      $9, _tx_thread_execute_ptr          # Pickup starting address of execute list
    addu    $9, $9, $25                         # Build address of execute pointer for this TC

    /* Wait for a thread to execute.  */
    /* do
    {  */

_tx_thread_schedule_loop:

    lw      $8, ($9)                                # Pickup next thread to execute
    beq     $8, $0, _tx_thread_schedule_idle_system # If not-NULL, check the thread's ready bit
    nop                                             # Delay slot

_tx_thread_check_ready_bit:
    lw      $9, 152($8)                         # Pickup the thread's VPE control register
    andi    $10, $9, 0x8000                     # Pickup ready bit (bit 15)
    bne     $10, $0, _tx_thread_is_ready        # If ready bit is set, actually schedule the thread
    andi    $9, $9, 0x7FFF                      # Clear the ready bit (bit 15)

    b       _tx_thread_schedule                 # Resume at the top of the scheduling loop
    nop

_tx_thread_is_ready:


    sw      $9, 152($8)                         # Store the cleared ready bit to prevent any other VPE from scheduling this thread
    sync

    /* }
    while(_tx_thread_execute_ptr[VPE] == TX_NULL);  */


_tx_thread_schedule_thread:

    /* Yes! We have a thread to execute. Interrupts and multithreading are locked out.
       Pickup the thread's register context, enable multithreading, and transfer control to
       the thread.  */

    /* Save this thread in the context register of the TC.  */

    mtc0   $8, C0_TCContext                     # Set TCContext to current thread
    ehb                                         #


#ifdef TX_ENABLE_EVENT_LOGGING
    or      $16, $8, $0                         # Save thread pointer into non-volatile
    or      $4, $8, $0                          # Move thread pointer into input register
    la      $9, _tx_el_thread_running           # Build address of thread running event routine
    jal     $9                                  # Call event logging routine
    nop                                         # Delay slot
    or      $8, $16, $0                         # Recover thread pointer
#endif

    /* Setup the current thread pointer.  */
    /* _tx_thread_current_ptr[VPE] =  _tx_thread_execute_ptr[VPE];  */

    la      $9, _tx_thread_current_ptr          # Pickup current thread pointer address
    addu    $9, $9, $25                         # Offset to VPE specific entry
    sw      $8, ($9)                            # Set current thread pointer

    /* Increment the run count for this thread.  */
    /* _tx_thread_current_ptr[VPE] -> tx_thread_run_count++;  */

    lw      $10, 4($8)                          # Pickup run count
    lw      $11, 24($8)                         # Pickup time slice value
    addu    $10, $10, 1                         # Increment run count
    sw      $10, 4($8)                          # Store new run count

    /* Setup time-slice, if present.  */
    /* _tx_timer_time_slice[VPE] =  _tx_thread_current_ptr[VPE] -> tx_thread_time_slice;  */

    la      $10, _tx_timer_time_slice           # Pickup time-slice variable address
    addu    $10, $10, $25                       # Offset to VPE specific time-slice

    /* Switch to the thread's stack.  */
    /* SP =  _tx_thread_current_ptr[VPE] -> tx_thread_stack_ptr;  */

    lw      $29, 8($8)                          # Switch to thread's stack
    lw      $15, 176($8)                        # Pickup FPU enable flag in TX_THREAD structure
    sw      $11, ($10)                          # Store new time-slice

    /* Determine if an interrupt frame or a synchronous task suspension frame
       is present.  */

    lw      $10, ($29)                          # Pickup stack type
    beqz    $10, _tx_thread_synch_return        # If 0, solicited thread return
    nop                                         # Delay slot

#ifdef TX_ENABLE_64BIT_FPU_SUPPORT

    /* Check if FPU is enabled for this thread. Note that threads with FPU enabled will only be
       scheduled in VPE 0.  */


    lw      $9,120($29)                         # Recover SR
    li      $10,0xDFFFFFFF                      # Mask for FPU enable bit
    mfc0    $8, UserLocal                       # Pickup VPE ID
    and     $9, $9, $10                         # Build SR with FPU enable bit masked
    bne     $8, $0, _tx_skip_fpu_int_restore    # If not VPE 0, skip FPU restore
    li      $10, 0x20000000                     # Build FPU enable bit
    or      $9, $9, $10                         # Build SR with FPU enable
    beq     $15, $0, _tx_skip_fpu_int_restore   # If FPU not enabled, skip FPU restore
    nop
    lw      $8, 384($29)                        # Recover fcr31
    ctc1    $8, $31                             # Setup fcr31
    ldc1    $f31, 128($29)                      # Recover f31
    ldc1    $f30, 136($29)                      # Recover f30
    ldc1    $f29, 144($29)                      # Recover f29
    ldc1    $f28, 152($29)                      # Recover f28
    ldc1    $f27, 160($29)                      # Recover f27
    ldc1    $f26, 168($29)                      # Recover f26
    ldc1    $f25, 176($29)                      # Recover f25
    ldc1    $f24, 184($29)                      # Recover f24
    ldc1    $f23, 192($29)                      # Recover f23
    ldc1    $f22, 200($29)                      # Recover f22
    ldc1    $f21, 208($29)                      # Recover f21
    ldc1    $f20, 216($29)                      # Recover f20
    ldc1    $f19, 224($29)                      # Recover f19
    ldc1    $f18, 232($29)                      # Recover f18
    ldc1    $f17, 240($29)                      # Recover f17
    ldc1    $f16, 248($29)                      # Recover f16
    ldc1    $f15, 256($29)                      # Recover f15
    ldc1    $f14, 264($29)                      # Recover f14
    ldc1    $f13, 272($29)                      # Recover f13
    ldc1    $f12, 280($29)                      # Recover f12
    ldc1    $f11, 288($29)                      # Recover f11
    ldc1    $f10, 296($29)                      # Recover f10
    ldc1    $f9,  304($29)                      # Recover f9
    ldc1    $f8,  312($29)                      # Recover f8
    ldc1    $f7,  320($29)                      # Recover f7
    ldc1    $f6,  328($29)                      # Recover f6
    ldc1    $f5,  336($29)                      # Recover f5
    ldc1    $f4,  344($29)                      # Recover f4
    ldc1    $f3,  352($29)                      # Recover f3
    ldc1    $f2,  360($29)                      # Recover f2
    ldc1    $f1,  368($29)                      # Recover f1
    ldc1    $f0,  376($29)                      # Recover f0

_tx_skip_fpu_int_restore:
    sw      $9,120($29)                         # Store new SR

#endif

    /* Recover standard registers.  */

    lw      $8,124($29)                         # Recover EPC
    lw      $9,120($29)                         # Recover SR
    mtc0    $8, $14                             # Setup EPC
    ehb                                         #
    lw      $30, 4($29)                         # Recover s8

    mtc0    $9, $12                             # Restore SR
    ehb                                         # Clear hazards
    lw      $23, 8($29)                         # Recover s7
    lw      $22, 12($29)                        # Recover s6
    lw      $21, 16($29)                        # Recover s5
    lw      $20, 20($29)                        # Recover s4
    lw      $19, 24($29)                        # Recover s3
    lw      $18, 28($29)                        # Recover s2
    lw      $17, 32($29)                        # Recover s1
    lw      $16, 36($29)                        # Recover s0
    lw      $8,  40($29)                        # Recover hi
    lw      $9,  44($29)                        # Recover low
    mthi    $8                                  # Setup hi
    mtlo    $9                                  # Setup lo
    lw      $25, 48($29)                        # Recover t9
    lw      $24, 52($29)                        # Recover t8
    lw      $15, 56($29)                        # Recover t7
    lw      $14, 60($29)                        # Recover t6
    lw      $13, 64($29)                        # Recover t5
    lw      $12, 68($29)                        # Recover t4
    lw      $11, 72($29)                        # Recover t3
    lw      $10, 76($29)                        # Recover t2
    lw      $9,  80($29)                        # Recover t1
    lw      $8,  84($29)                        # Recover t0
    lw      $7,  88($29)                        # Recover a3
    lw      $6,  92($29)                        # Recover a2
    lw      $5,  96($29)                        # Recover a1
    lw      $4, 100($29)                        # Recover a0
    lw      $3, 104($29)                        # Recover v1
    lw      $2, 108($29)                        # Recover v0
    .set    noat
    lw      $1, 112($29)                        # Recover at
    .set    at
    lw      $31,116($29)                        # Recover ra
    addu    $29, $29, 392                       # Recover stack frame
    emt                                         # Enable multithreading again
    eret                                        # Return to point of interrupt

_tx_thread_synch_return:

#ifdef TX_ENABLE_64BIT_FPU_SUPPORT

    /* Check if FPU is enabled for this thread. Note that threads with FPU enabled will only be
       scheduled in VPE 0.  */

    lw      $9,52($29)                          # Recover SR
    li      $10,0xDFFFFFFF                      # Mask for FPU enable bit
    mfc0    $8, UserLocal                       # Pickup VPE ID
    and     $9, $9, $10                         # Build SR with FPU enable bit masked
    bne     $8, $0, _tx_skip_fpu_sync_restore   # If not TC 0, skip FPU restore
    li      $10, 0x20000000                     # Build FPU enable bit
    or      $9, $9, $10                         # Build SR with FPU enable
    beq     $15, $0, _tx_skip_fpu_sync_restore  # If FPU not enabled, skip FPU restore
    nop
    lw      $8, 152($29)                        # Recover fcr31
    ctc1    $8, $31                             # Setup fcr31
    ldc1    $f31, 56($29)                       # Recover f31
    ldc1    $f30, 64($29)                       # Recover f30
    ldc1    $f29, 72($29)                       # Recover f29
    ldc1    $f28, 80($29)                       # Recover f28
    ldc1    $f27, 88($29)                       # Recover f27
    ldc1    $f26, 96($29)                       # Recover f26
    ldc1    $f25, 104($29)                      # Recover f25
    ldc1    $f24, 112($29)                      # Recover f24
    ldc1    $f23, 120($29)                      # Recover f23
    ldc1    $f22, 128($29)                      # Recover f22
    ldc1    $f21, 136($29)                      # Recover f21
    ldc1    $f20, 144($29)                      # Recover f20
_tx_skip_fpu_sync_restore:
    sw      $9,52($29)                          # Store new SR
#endif

   /* Recover standard preserved registers.  */

    lw      $30, 4($29)                         # Recover s8
    lw      $23, 8($29)                         # Recover s7
    lw      $22, 12($29)                        # Recover s6
    lw      $21, 16($29)                        # Recover s5
    lw      $20, 20($29)                        # Recover s4
    lw      $19, 24($29)                        # Recover s3
    lw      $18, 28($29)                        # Recover s2
    lw      $17, 32($29)                        # Recover s1
    lw      $16, 36($29)                        # Recover s0
    lw      $8,  40($29)                        # Recover hi
    lw      $9,  44($29)                        # Recover low
    mthi    $8                                  # Setup hi
    mtlo    $9                                  # Setup lo
    lw      $8,  52($29)                        # Recover SR
    lw      $31, 48($29)                        # Recover ra
    addu    $29, $29, 160                       # Recover stack space
    mtc0    $8, $12                             # Restore SR
    ehb                                         # Clear hazards
    emt                                         # Enable multithreading
    jr.hb   $31                                 # Return to thread
    nop                                         #

/* }  */

