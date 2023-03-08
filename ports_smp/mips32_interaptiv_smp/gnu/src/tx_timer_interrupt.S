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
/**   Timer                                                               */
/**                                                                       */
/**************************************************************************/
/**************************************************************************/


#define UserLocal       $4,2
#define C0_TCBind       $2,2

    .text
    .set        noreorder
/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _tx_timer_interrupt                           MIPS32_interAptiv/GNU */
/*                                                           6.2.1        */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Scott Larson, Microsoft Corporation                                 */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function processes the hardware timer interrupt.  This         */
/*    processing includes incrementing the system clock and checking for  */
/*    time slice and/or timer expiration.  If either is found, the        */
/*    interrupt context save/restore functions are called along with the  */
/*    expiration functions.                                               */
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
/*    _tx_thread_smp_protect                Get protection                */
/*    _tx_thread_smp_unprotect              Release protection            */
/*    _tx_timer_expiration_process          Timer expiration processing   */
/*    _tx_thread_time_slice                 Time slice interrupted thread */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    interrupt vector                                                    */
/*                                                                        */
/*  RELEASE HISTORY                                                       */
/*                                                                        */
/*    DATE              NAME                      DESCRIPTION             */
/*                                                                        */
/*  03-08-2023      Scott Larson            Initial Version 6.2.1         */
/*                                                                        */
/**************************************************************************/
/* VOID   _tx_timer_interrupt(VOID)
{  */
    .globl  _tx_timer_interrupt
_tx_timer_interrupt:

    /* Check VPE and throw away any timer interrupts for anything other than VPE 0.  */

    mfc0    $8, UserLocal                       # Pickup VPE ID
    beq     $8, $0, _handle_timer_interrupt     # If 0, VPE 0 should handle the interrupt
    nop
    jr      $31                                 # Other VPE simply returns
    nop
_handle_timer_interrupt:

    subu    $29, $29, 16                        # Allocate some storage on the stack
    sw      $31, 4($29)                         # Save ra
    sw      $16, 8($29)                         # Save preserved register s0

    /* Get protection before the timer variables are updated.  */
    /* _tx_thread_smp_protect();  */

    jal     _tx_thread_smp_protect              # Get VPE protection
    nop                                         #
    addu    $16, $2, 0                          # Save return value

    /* Increment timer interrupt active counter.  */
    /* _tx_timer_interrupt_active++;   */

    la      $9, _tx_timer_interrupt_active      # Build address of timer interrupt active count
    lw      $8, ($9)                            # Pickup timer interrupt active count
    addu    $8, $8, 1                           # Increment timer interrupt active count
    sw      $8, ($9)                            # Store new timer interrupt active count
    sync

    /* Increment the system clock.  */
    /* _tx_timer_system_clock++;  */

    la      $9, _tx_timer_system_clock          # Pickup address of system clock
    lw      $8, ($9)                            # Pickup system clock
    addu    $8, $8, 1                           # Increment system clock
    sw      $8, ($9)                            # Store new system clock

    /* Test for timer expiration.  */
    /* if (*_tx_timer_current_ptr)
    {  */

    la      $13, _tx_timer_expired              # Pickup address of timer expired flag
    lw      $10, ($13)                          # Pickup the timer expired flag
    bne     $10, $0, _tx_timer_done             # If already expired, skip expiration processing
    nop                                         #
    la      $9, _tx_timer_current_ptr           # Pickup address of current ptr
    lw      $8, ($9)                            # Pickup current pointer
    la      $13, _tx_timer_expired              # Pickup address of timer expired flag
    lw      $10, ($8)                           # Pickup the current timer entry
    ori     $12, $0, 1                          # Build TX_TRUE flag
    beqz    $10, _tx_timer_no_timer             # If NULL, no timer has expired
    nop                                         # Delay slot

        /* Set expiration flag.  */
        /* _tx_timer_expired =  TX_TRUE;  */

    ori     $15, $0, 2                          # Set local expired flag
    b       _tx_timer_done                      # Finished timer processing
    sw      $12, ($13)                          # Set expired flag in memory


    /* }
    else
    {  */
_tx_timer_no_timer:

    ori     $15, $0, 0                          # Set expired flag to false

        /* No timer expired, increment the timer pointer.  */
        /* _tx_timer_current_ptr++;  */

        /* Check for wrap-around.  */
        /* if (_tx_timer_current_ptr == _tx_timer_list_end)  */

    la      $12, _tx_timer_list_end             # Pickup address of list end pointer
    lw      $11, ($12)                          # Pickup actual list end
    addu    $8, $8, 4                           # Point to next timer entry
    bne     $8, $11, _tx_timer_skip_wrap        # If not same, good pointer
    sw      $8, ($9)                            # Store new timer pointer

            /* Wrap to beginning of list.  */
            /* _tx_timer_current_ptr =  _tx_timer_list_start;  */

    la      $12, _tx_timer_list_start           # Pickup address of list start pointer
    lw      $10, ($12)                          # Pickup start of the list
    sw      $10, ($9)                           # Store new timer pointer


_tx_timer_skip_wrap:
    /* }  */

_tx_timer_done:

    /* Did a timer expire?  */
    /* if (_tx_timer_expired)
    {  */

    beqz    $15, _tx_timer_dont_activate        # No, timer not expired
    nop                                         # Delay slot

        /* Call the timer expiration processing.  */
        /* _tx_timer_expiration_process(void);  */

    la      $9, _tx_timer_expiration_process    # Build address of _tx_timer_expiratoin_process routine
    jal     $9                                  # Call _tx_timer_expiration_process
    nop
    lw      $15, ($29)                          # Recover local expired flag

    /* }  */
_tx_timer_dont_activate:


        /* Call time-slice processing.   */
        /* _tx_thread_time_slice();  */

    la      $9, _tx_thread_time_slice           # Pickup address of time slice function
    jal     $9                                  # Call time slice
    nop                                         # Delay slot

    /* Decrement timer interrupt active counter.  */
    /* _tx_timer_interrupt_active--;   */

    la      $9, _tx_timer_interrupt_active      # Build address of timer interrupt active count
    lw      $8, ($9)                            # Pickup timer interrupt active count
    subu    $8, $8, 1                           # Decrement timer interrupt active count
    sw      $8, ($9)                            # Store new timer interrupt active count
    sync

    /* Release VPE protection.  */
    /* _tx_thread_smp_unprotect();  */

    addu    $4, $16, 0                          # Setup input parameter
    jal     _tx_thread_smp_unprotect            # Release protection
    nop                                         #

    lw      $31, 4($29)                         # Recover ra
    lw      $16, 8($29)                         # Recover s0
    addu    $29, $29, 16                        # Recover stack space
    j       $31                                 # Return to caller
    nop                                         # Delay slot

/* }  */
