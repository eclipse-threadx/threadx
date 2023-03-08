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
#define C0_VPECtl     $1,1

    .text
    .set        noreorder
/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _tx_thread_context_restore                    MIPS32_interAptiv/GNU */
/*                                                           6.2.1        */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Scott Larson, Microsoft Corporation                                 */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function restores the interrupt context if it is processing a  */
/*    nested interrupt.  If not, it returns to the interrupt thread if no */
/*    preemption is necessary.  Otherwise, if preemption is necessary or  */
/*    if no thread was running, the function returns to the scheduler.    */
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
/*    _tx_thread_schedule                   Thread scheduling routine     */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    ISRs                                  Interrupt Service Routines    */
/*                                                                        */
/*  RELEASE HISTORY                                                       */
/*                                                                        */
/*    DATE              NAME                      DESCRIPTION             */
/*                                                                        */
/*  03-08-2023      Scott Larson            Initial Version 6.2.1         */
/*                                                                        */
/**************************************************************************/
/* VOID   _tx_thread_context_restore(VOID)
{  */
    .globl  _tx_thread_context_restore
_tx_thread_context_restore:

    /* Lockout interrupts.  */

    di                                          # Disable interrupts
    ehb                                         #
    mfc0    $25, UserLocal                      # Pickup VPE ID
    sll     $24, $25, 2                         # Build index based on VPE number

    /* Determine if interrupts are nested.  */
    /* if (--_tx_thread_system_state[VPE])
    {  */

    la      $9, _tx_thread_system_state         # Pickup addr of nested interrupt count
    addu    $9, $9, $24                         # Index by VPE
    lw      $8, ($9)                            # Pickup nested interrupt count
    subu    $8, $8, 1                           # Decrement the nested interrupt counter
    beqz    $8,_tx_thread_not_nested_restore    # If 0, not nested restore
    sw      $8, ($9)                            # Store new nested count

    /* Interrupts are nested.  */

    /* Just recover the saved registers and return to the point of
       interrupt.  */

#ifdef TX_ENABLE_64BIT_FPU_SUPPORT

    /* Check if FPU is enabled for this thread. Note that threads with FPU enabled will only be
       scheduled in VPE 0.  */

    bne     $25, $0, _tx_skip_nest_restore      # If not VPE 0, skip FPU scratch restore
    nop                                         #

    lw      $8, 384($29)                        # Recover fcr31
    ctc1    $8, $31                             # Setup fcr31
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

_tx_skip_nest_restore:

#endif

    /* Recover standard registers.  */

    lw      $16, 36($29)                        # Recover s0
    lw      $8,  40($29)                        # Recover hi
    lw      $9,  44($29)                        # Recover low
    mthi    $8                                  # Setup hi
    mtlo    $9                                  # Setup lo
    lw      $8,124($29)                         # Recover EPC
    lw      $9,120($29)                         # Recover SR
    mtc0    $8, $14                             # Setup EPC
    ehb                                         #
    lw      $25, 48($29)                        # Recover t9
    mtc0    $9, $12                             # Restore SR
    ehb                                         #
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
    eret                                        # Return to point of interrupt
    nop                                         # Delay

    /* }  */
_tx_thread_not_nested_restore:

    /* Determine if a thread was interrupted and no preemption is required.  */
    /* else if (((_tx_thread_current_ptr[VPE]) && (_tx_thread_current_ptr[VPE] == _tx_thread_tc_execute_list[VPE])
               || (_tx_thread_preempt_disable))
    {  */

    la      $9, _tx_thread_current_ptr          # Pickup address of current ptr
    addu    $9, $9, $24                         # Build address of current pointer for this VPE
    lw      $8, ($9)                            # Pickup current thread pointer
    beqz    $8, _tx_thread_idle_system_restore  # If NULL, idle system restore
    nop                                         #
    la      $11, _tx_thread_execute_ptr         # Pickup address of execute thread pointer
    addu    $11, $11, $24                       # Add VPE index here to see if this is the thread
    lw      $10, ($11)                          # Pickup thread execute pointer
    beq     $8, $10, _tx_thread_no_preempt_restore # If the current and execute are the same then, restore the current thread
    nop                                         # Delay slot
    la      $10, _tx_thread_smp_protection      # Build address of protection structure
    lw      $11, 8($10)                         # Pickup the VPE with protection
    bne     $11, $25,_tx_thread_preempt_restore # If this is a different VPE, preempt current thread
    nop                                         #
    la      $13, _tx_thread_preempt_disable     # Pickup address of preempt disable flag
    lw      $12, ($13)                          # Pickup preempt disable flag
    beq     $12, $0, _tx_thread_preempt_restore # If not set, preempt interrupted thread
    nop                                         # Delay slot

_tx_thread_no_preempt_restore:

    /* Restore interrupted thread or ISR.  */

    /* Pickup the saved stack pointer.  */
    /* SP =  _tx_thread_current_ptr[VPE] -> tx_thread_stack_ptr;  */

    lw      $29, 8($8)                          # Switch back to thread's stack

    /* Recover the saved context and return to the point of interrupt.  */


#ifdef TX_ENABLE_64BIT_FPU_SUPPORT

    /* Check if FPU is enabled for this thread. Note that threads with FPU enabled will only be
       scheduled in VPE 0.  */

    lw      $15, 176($8)                        # Pickup FPU enable flag
    bne     $25, $0, _tx_skip_int_restore       # If not VPE 0, skip FPU scratch restore
    nop                                         # Delay
    beq     $15, $0, _tx_skip_int_restore       # If FPU not enabled, skip FPU scratch restore
    nop                                         #

    lw      $9, 384($29)                        # Recover fcr31
    ctc1    $9, $31                             # Setup fcr31
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

_tx_skip_int_restore:

#endif

    /* Recover standard registers.  */

    lw      $16, 36($29)                        # Recover s0
    lw      $8,  40($29)                        # Recover hi
    lw      $9,  44($29)                        # Recover low
    mthi    $8                                  # Setup hi
    mtlo    $9                                  # Setup lo
    lw      $8,124($29)                         # Recover EPC
    lw      $9,120($29)                         # Recover SR
    mtc0    $8, $14                             # Setup EPC
    ehb                                         #
    lw      $25, 48($29)                        # Recover t9
    mtc0    $9, $12                             # Restore SR
    ehb                                         #
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
    eret                                        # Return to point of interrupt
    nop                                         # Delay

    /* }
    else
    {  */
_tx_thread_preempt_restore:

    /* Save remaining context on the thread's stack.  */

    lw      $9, 8($8)                           # Pickup thread's stack pointer
    ori     $12, $0, 1                          # Build interrupt stack type
    sw      $12, ($9)                           # Store stack type

#ifdef TX_ENABLE_64BIT_FPU_SUPPORT

    /* Check if FPU is enabled for this thread. Note that threads with FPU enabled will only be
       scheduled in VPE 0.  */

    lw      $15, 176($8)                        # Pickup FPU enable flag
    bne     $25, $0, _tx_skip_preserved_save    # If not VPE 0, skip FPU preserved save
    nop                                         #
    beq     $15, $0, _tx_skip_preserved_save    # If FPU not enabled, skip FPU preserved save
    nop

    lw      $7, 384($9)                         # Recover fcr31
    ctc1    $7, $31                             # Setup fcr31
    sdc1    $f31, 128($9)                       # Store f31
    sdc1    $f30, 136($9)                       # Store f30
    sdc1    $f29, 144($9)                       # Store f29
    sdc1    $f28, 152($9)                       # Store f28
    sdc1    $f27, 160($9)                       # Store f27
    sdc1    $f26, 168($9)                       # Store f26
    sdc1    $f25, 176($9)                       # Store f25
    sdc1    $f24, 184($9)                       # Store f24
    sdc1    $f23, 192($9)                       # Store f23
    sdc1    $f22, 200($9)                       # Store f22
    sdc1    $f21, 208($9)                       # Store f21
    sdc1    $f20, 216($9)                       # Store f20
_tx_skip_preserved_save:
#endif

    /* Store standard preserved registers.  */

    sw      $30, 4($9)                          # Store s8
    sw      $23, 8($9)                          # Store s7
    sw      $22, 12($9)                         # Store s6
    sw      $21, 16($9)                         # Store s5
    sw      $20, 20($9)                         # Store s4
    sw      $19, 24($9)                         # Store s3
    sw      $18, 28($9)                         # Store s2
    sw      $17, 32($9)                         # Store s1
                                        /*      # Note: s0 is already stored!  */

#ifdef TX_ENABLE_EVENT_LOGGING
    or      $17, $24, $0                        # Save VPE index offset value
    or      $16, $8, $0                         # Save thread pointer into non-volatile
    or      $4, $8, $0                          # Move thread pointer into input register
    la      $9, _tx_el_thread_preempted         # Build address of thread preempted event routine
    jal     $9                                  # Call event logging routine
    nop                                         # Delay slot
    or      $8, $16, $0                         # Recover thread pointer
    or      $24, $17, $0                        # Reciver VPE index offset value
#endif

    /* Save the remaining time-slice and disable it.  */
    /* if (_tx_timer_time_slice[VPE])
    {  */

    la      $10, _tx_timer_time_slice           # Pickup time slice variable address
    addu    $10, $10, $24                       # Build index into time-slice

    /* Check for time-slice race condition.  */

    la      $12, _tx_timer_interrupt_active
__time_slice_wait:
    lw      $13, 0($12)
    bne     $13, $0, __time_slice_wait

    lw      $9, ($10)                           # Pickup time slice
    la      $12, _tx_thread_current_ptr         # Pickup current thread pointer address
    addu    $12, $12, $24                       # Build VPE index
    beqz    $9, _tx_thread_dont_save_ts         # If 0, skip time slice processing
    nop                                         # Delay slot

        /* _tx_thread_current_ptr[VPE] -> tx_thread_time_slice =  _tx_timer_time_slice[VPE]
        _tx_timer_time_slice[VPE] =  0;  */

    sw      $9, 24($8)                          # Save current time slice
    sw      $0, ($10)                           # Clear global time slice


    /* }  */
_tx_thread_dont_save_ts:


    /* Clear the current task pointer.  */
    /* _tx_thread_current_ptr[VPE] =  TX_NULL;  */

    sw      $0, ($12)                           # Clear current thread pointer

    /* Set bit indicating the thread is ready for scheduling.  */

    lw      $9, 152($8)                         # Pickup the thread's VPE control register
    ori     $9, $9, 0x8000                      # Set ready bit (bit 15)
    sync
    sw      $9, 152($8)                         # Make this thread ready for scheduling

    /* Return to the scheduler.  */
    /* _tx_thread_schedule();  */


_tx_thread_idle_system_restore:

    /* Just return back to the scheduler!  */

    mfc0    $15, $12                            # Pickup SR
    li      $8, 0xFFFFFFFD                      # Build mask for EXL bit
    and     $15, $15, $8                        # Clear EXL bit
    ori     $15, $15, 1                         # Set IE bit
    mtc0    $15, $12                            # Setup new SR with IE enabled
    ehb                                         #
    la      $8, _tx_thread_schedule             # Build address of scheduling loop
    jr      $8                                  # Return to scheduler
    nop                                         # Delay slot

/* }  */

