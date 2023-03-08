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


    .text
    .set        noreorder
/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _tx_thread_context_save                       MIPS32_interAptiv/GNU */
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
    .globl _tx_thread_context_save
_tx_thread_context_save:

    /* Upon entry to this routine, it is assumed that interrupts are locked
       out and the stack is exactly were it was when the interrupt occurred.
       The return address is in $27 (k1).  */

    subu    $29, $29, 392                   # Allocate space for a full stack frame
                                        /*  #   even though the whole thing might
                                            #   not be needed for awhile  */
    sw      $25, 48($29)                    # Store t9
    sw      $24, 52($29)                    # Store t8
    sw      $8, 84($29)                     # Save t0
    mfc0    $25, UserLocal                  # Pickup VPE ID
    sll     $24, $25, 2                     # Build index based on VPE number
    la      $26, _tx_thread_system_state    # Pickup address of system state
    addu    $26, $26, $24                   # Index by VPE
    lw      $8, ($26)                       # Pickup system state

    /* Check for a nested interrupt condition.  */
    /* if (_tx_thread_system_state[VPE]++)
    {  */

    beqz    $8, _tx_thread_not_nested_save  # If 0, first interrupt condition
    addu    $8, $8, 1                       # Increment the nested interrupt counter


    /* Nested interrupt condition.  */

    sw      $8, ($26)                       # Store the interrupt counter


#ifdef TX_ENABLE_64BIT_FPU_SUPPORT

    /* Check if FPU is enabled for this thread. Note that threads with FPU enabled will only be
       scheduled in VPE 0.  */

    bne     $25, $0, _tx_skip_nest_int_save # If not VPE 0, skip FPU save
    nop                                     #

    /* Save scratch floating point registers.  */

    cfc1    $8, $31                         # Pickup floating point control reg
    sdc1    $f19, 224($29)                  # Save f19
    sdc1    $f18, 232($29)                  # Save f18
    sdc1    $f17, 240($29)                  # Save f17
    sdc1    $f16, 248($29)                  # Save f16
    sdc1    $f15, 256($29)                  # Save f15
    sdc1    $f14, 264($29)                  # Save f14
    sdc1    $f13, 272($29)                  # Save f13
    sdc1    $f12, 280($29)                  # Save f12
    sdc1    $f11, 288($29)                  # Save f11
    sdc1    $f10, 296($29)                  # Save f10
    sdc1    $f9,  304($29)                  # Save f9
    sdc1    $f8,  312($29)                  # Save f8
    sdc1    $f7,  320($29)                  # Save f7
    sdc1    $f6,  328($29)                  # Save f6
    sdc1    $f5,  336($29)                  # Save f5
    sdc1    $f4,  344($29)                  # Save f4
    sdc1    $f3,  352($29)                  # Save f3
    sdc1    $f2,  360($29)                  # Save f2
    sdc1    $f1,  368($29)                  # Save f1
    sdc1    $f0,  376($29)                  # Save f0
    sw      $8,   384($29)                  # Save fcr31
_tx_skip_nest_int_save:

#endif


    /* Save the rest of the scratch registers on the stack and return to the
       calling ISR.  */

    sw      $16, 36($29)                    # Store s0
    mfhi    $8                              # Pickup hi
    mflo    $26                             # Pickup lo
    sw      $8,  40($29)                    # Store hi
    sw      $26, 44($29)                    # Store lo
    sw      $15, 56($29)                    # Store t7
    sw      $14, 60($29)                    # Store t6
    sw      $13, 64($29)                    # Store t5
    sw      $12, 68($29)                    # Store t4
    sw      $11, 72($29)                    # Store t3
    sw      $10, 76($29)                    # Store t2
    sw      $9,  80($29)                    # Store t1
    sw      $7,  88($29)                    # Store a3
    sw      $6,  92($29)                    # Store a2
    sw      $5,  96($29)                    # Store a1
    sw      $4, 100($29)                    # Store a0
    sw      $3, 104($29)                    # Store v1
    sw      $2, 108($29)                    # Store v0
    .set    noat
    sw      $1, 112($29)                    # Store at
    .set    at
    sw      $31, 116($29)                   # Store ra
    mfc0    $8, $12                         # Pickup SR
    mfc0    $9, $14                         # Pickup EPC
    sw      $8, 120($29)                    # Store SR
    sw      $9, 124($29)                    # Store EPC

    /* Return to the ISR.  */

    j       $27                             # Return to ISR
    nop                                     #

_tx_thread_not_nested_save:
    /* }  */

    /* Otherwise, not nested, check to see if a thread was running.  */
    /* else if (_tx_thread_current_ptr[VPE])
    {  */

    sw      $8, ($26)                       # Store the interrupt counter
    la      $26, _tx_thread_current_ptr     # Pickup address of current ptr
    addu    $26, $26, $24                   # Build address of current pointer for this VPE

    lw      $8, ($26)                       # Pickup current thread pointer
    beqz    $8, _tx_thread_idle_system_save # If NULL, idle system was interrupted
    sw      $16, 36($29)                    # Store s0

    /* Save minimal context of interrupted thread.  */

#ifdef TX_ENABLE_64BIT_FPU_SUPPORT

    /* Check if FPU is enabled for this thread. Note that threads with FPU enabled will only be
       scheduled in TC 0.  */

    bne     $25, $0, _tx_skip_int_save      # If not VPE 0, skip FPU save
    nop                                     # Delay
    lw      $26, 176($8)                    # Pickup FPU enable flag
    beq     $26, $0, _tx_skip_int_save      # If FPU not enabled, skip FPU save
    nop                                     # Delay

    /* Save scratch floating point registers.  */

    cfc1    $8, $31                         # Pickup floating point control reg
    sdc1    $f19, 224($29)                  # Save f19
    sdc1    $f18, 232($29)                  # Save f18
    sdc1    $f17, 240($29)                  # Save f17
    sdc1    $f16, 248($29)                  # Save f16
    sdc1    $f15, 256($29)                  # Save f15
    sdc1    $f14, 264($29)                  # Save f14
    sdc1    $f13, 272($29)                  # Save f13
    sdc1    $f12, 280($29)                  # Save f12
    sdc1    $f11, 288($29)                  # Save f11
    sdc1    $f10, 296($29)                  # Save f10
    sdc1    $f9,  304($29)                  # Save f9
    sdc1    $f8,  312($29)                  # Save f8
    sdc1    $f7,  320($29)                  # Save f7
    sdc1    $f6,  328($29)                  # Save f6
    sdc1    $f5,  336($29)                  # Save f5
    sdc1    $f4,  344($29)                  # Save f4
    sdc1    $f3,  352($29)                  # Save f3
    sdc1    $f2,  360($29)                  # Save f2
    sdc1    $f1,  368($29)                  # Save f1
    sdc1    $f0,  376($29)                  # Save f0
    sw      $8,   384($29)                  # Save fcr31
_tx_skip_int_save:

#endif

    /* Save the standard scratch registers.  */

    mfhi    $8                              # Pickup hi
    mflo    $26                             # Pickup lo
    sw      $8,  40($29)                    # Store hi
    sw      $26, 44($29)                    # Store lo
    sw      $15, 56($29)                    # Store t7
    sw      $14, 60($29)                    # Store t6
    sw      $13, 64($29)                    # Store t5
    sw      $12, 68($29)                    # Store t4
    sw      $11, 72($29)                    # Store t3
    sw      $10, 76($29)                    # Store t2
    sw      $9,  80($29)                    # Store t1
    sw      $7,  88($29)                    # Store a3
    sw      $6,  92($29)                    # Store a2
    sw      $5,  96($29)                    # Store a1
    sw      $4, 100($29)                    # Store a0
    sw      $3, 104($29)                    # Store v1
    sw      $2, 108($29)                    # Store v0
    .set    noat
    sw      $1, 112($29)                    # Store at
    .set    at
    sw      $31, 116($29)                   # Store ra
    mfc0    $8, $12                         # Pickup SR
    mfc0    $9, $14                         # Pickup EPC
    sw      $8, 120($29)                    # Store SR
    sw      $9, 124($29)                    # Store EPC

    li      $8, 1                           # Build stack type
    sw      $8, ($29)                       # Store stack type

    /* Save the current stack pointer in the thread's control block.  */
    /* _tx_thread_current_ptr[VPE] -> tx_thread_stack_ptr =  sp;  */

    /* Switch to the system stack.  */
    /* sp =  _tx_thread_system_stack_ptr[VPE];  */

    la      $9, _tx_thread_current_ptr      # Pickup address of current ptr
    addu    $9, $9, $24                     # Build address of current pointer for this VPE

    lw      $11, ($9)                       # Pickup current thread pointer
    la      $10,_tx_thread_system_stack_ptr # Pickup the stack pointer address
    sw      $29, 8($11)                     # Save stack pointer
    addu    $10, $10, $24                   # Build offset to system stack pointer
    lw      $29, ($10)                      # Switch to system stack

    j       $27                             # Return to ISR
    nop                                     #

    /* }
    else
    {  */

_tx_thread_idle_system_save:

    /* Interrupt occurred in the scheduling loop.  */

    addu    $29, $29, 392                   # Recover the reserved stack space
    j       $27                             # Return to ISR
    nop                                     #

    /* }
}  */

