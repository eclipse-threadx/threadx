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

/* #define TX_SOURCE_CODE  */


/* Include necessary system files.  */

/*  #include "tx_api.h"
    #include "tx_timer.h"
    #include "tx_thread.h"  */

    EXTERN      _tx_timer_system_clock
    EXTERN      _tx_timer_time_slice
    EXTERN      _tx_timer_expired_time_slice
    EXTERN      _tx_timer_current_ptr
    EXTERN      _tx_timer_expired
    EXTERN      _tx_timer_list_end
    EXTERN      _tx_timer_list_start
    EXTERN      _tx_timer_expiration_process
    EXTERN      _tx_thread_time_slice


    SECTION `.mtext`:CODE:REORDER:NOROOT(2)
    CODE
/**************************************************************************/ 
/*                                                                        */ 
/*  FUNCTION                                               RELEASE        */ 
/*                                                                        */ 
/*    _tx_timer_interrupt                                RISC-V32/IAR     */
/*                                                           6.1          */
/*  AUTHOR                                                                */ 
/*                                                                        */ 
/*    William E. Lamie, Microsoft Corporation                             */ 
/*    Tom van Leeuwen, Technolution B.V.                                  */
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
/*  09-30-2020     William E. Lamie         Initial Version 6.1           */ 
/*                                                                        */ 
/**************************************************************************/ 
/* VOID   _tx_timer_interrupt(VOID)
{  */
    PUBLIC      _tx_timer_interrupt
_tx_timer_interrupt:

    /* Increment the system clock.  */
    /* _tx_timer_system_clock++;  */

    la      t0, _tx_timer_system_clock                  ; Pickup address of system clock
    lw      t1, 0(t0)                                   ; Pickup system clock
    la      t2, _tx_timer_time_slice                    ; Pickup address of time slice
    lw      t3, 0(t2)                                   ; Pickup time slice
    addi    t1, t1, 1                                   ; Increment system clock
    sw      t1, 0(t0)                                   ; Store new system clock
    li      t6, 0                                       ; Clear local expired flag

    /* Test for time-slice expiration.  */
    /* if (_tx_timer_time_slice)
    {  */

    beqz    t3, _tx_timer_no_time_slice                 ; If 0, skip time slice processing
    addi    t3, t3, -1                                  ; Decrement the time slice

       /* Decrement the time_slice.  */
       /* _tx_timer_time_slice--;  */

    sw      t3, 0(t2)                                   ; Store new time slice

       /* Check for expiration.  */
       /* if (__tx_timer_time_slice == 0)  */

    bgtz    t3, _tx_timer_no_time_slice                 ; If not 0, has not expired yet
    li      t1, 1                                       ; Build expired flag

       /* Set the time-slice expired flag.  */
       /* _tx_timer_expired_time_slice =  TX_TRUE;  */

    la      t4, _tx_timer_expired_time_slice            ; Get address of expired flag
    sw      t1, 0(t4)                                   ; Set expired flag
    ori     t6, t6, 1                                   ; Set local expired flag

    /* }  */

_tx_timer_no_time_slice:

    /* Test for timer expiration.  */
    /* if (*_tx_timer_current_ptr)
    {  */

    la      t0, _tx_timer_current_ptr                   ; Pickup address of current ptr
    lw      t1, 0(t0)                                   ; Pickup current pointer
    lw      t3, 0(t1)                                   ; Pickup the current timer entry
    la      t2, _tx_timer_expired                       ; Pickup address of timer expired flag
    li      t4, 1                                       ; Build TX_TRUE flag
    beqz    t3, _tx_timer_no_timer                      ; If NULL, no timer has expired

        /* Set expiration flag.  */
        /* _tx_timer_expired =  TX_TRUE;  */

    ori     t6, t6, 2                                   ; Set local expired flag
    sw      t4, 0(t2)                                   ; Set expired flag in memory
    j       _tx_timer_done                              ; Finished timer processing


    /* }
    else
    {  */
_tx_timer_no_timer:

        /* No timer expired, increment the timer pointer.  */
        /* _tx_timer_current_ptr++;  */

        /* Check for wrap-around.  */
        /* if (_tx_timer_current_ptr == _tx_timer_list_end)  */

    la      t2, _tx_timer_list_end                      ; Pickup address of list end pointer
    lw      t3, 0(t2)                                   ; Pickup actual list end
    addi    t1, t1, 4                                   ; Point to next timer entry
    sw      t1, 0(t0)                                   ; Store new timer pointer
    bne     t1, t3, _tx_timer_skip_wrap                 ; If not same, good pointer

            /* Wrap to beginning of list.  */
            /* _tx_timer_current_ptr =  _tx_timer_list_start;  */

    la      t2, _tx_timer_list_start                    ; Pickup address of list start pointer
    lw      t4, 0(t2)                                   ; Pickup start of the list
    sw      t4, 0(t0)                                   ; Store new timer pointer*/


_tx_timer_skip_wrap:
    /* }  */

_tx_timer_done:


    /* See if anything has expired.  */
    /* if ((_tx_timer_expired_time_slice) || (_tx_timer_expired))
    {  */

    beqz    t6, _tx_timer_nothing_expired               ; If nothing expired skip the rest
    and     t2, t6, 2                                   ; Isolate the timer expired bit
    addi    sp, sp, -16                                 ; Allocate some storage on the stack
    sw      t6, 0(sp)                                   ; Save local expired flag
    sw      ra, 4(sp)                                   ; Save ra

    /* Did a timer expire?  */
    /* if (_tx_timer_expired)
    {  */

    beqz    t2, _tx_timer_dont_activate                 ; No, timer not expired

        /* Call the timer expiration processing.  */
        /* _tx_timer_expiration_process(void);  */

    call    _tx_timer_expiration_process                ; Call _tx_timer_expiration_process
    lw      t6, 0(sp)                                   ; Recover local expired flag

    /* }  */
_tx_timer_dont_activate:

    /* Did time slice expire?  */
    /* if (_tx_timer_expired_time_slice)
    {  */

    and     t2, t6, 1                                   ; Is the timer expired bit set?
    beqz    t2, _tx_timer_not_ts_expiration             ; If not, skip time slice processing

        /* Time slice interrupted thread.  */
        /* _tx_thread_time_slice();  */

    call    _tx_thread_time_slice                       ; Call time slice

     /* }  */

_tx_timer_not_ts_expiration:

    lw      ra, 4(sp)                                   ; Recover ra
    addi    sp, sp, 16                                  ; Recover stack space
    /* }  */

_tx_timer_nothing_expired:

    ret

/* }  */
    END
    