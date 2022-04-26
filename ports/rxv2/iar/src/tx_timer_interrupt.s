;/**************************************************************************/
;/*                                                                        */
;/*       Copyright (c) Microsoft Corporation. All rights reserved.        */
;/*                                                                        */
;/*       This software is licensed under the Microsoft Software License   */
;/*       Terms for Microsoft Azure RTOS. Full text of the license can be  */
;/*       found in the LICENSE file at https://aka.ms/AzureRTOS_EULA       */
;/*       and in the root directory of this software.                      */
;/*                                                                        */
;/**************************************************************************/
;
;
;/**************************************************************************/
;/**************************************************************************/
;/**                                                                       */
;/** ThreadX Component                                                     */
;/**                                                                       */
;/**   Thread                                                              */
;/**                                                                       */
;/**************************************************************************/
;/**************************************************************************/

SWI0 EQU   0x872E0

    extern __tx_timer_expiration_process
    extern __tx_timer_system_clock
    extern __tx_timer_expired_time_slice
    extern __tx_timer_current_ptr
    extern __tx_timer_expired
    extern __tx_timer_list_start
    extern __tx_timer_time_slice
    extern __tx_timer_list_end
    extern __tx_thread_time_slice
    extern __tx_thread_preempt_disable
    extern __tx_thread_execute_ptr
    extern __tx_thread_current_ptr

    section .text:CODE:ROOT

;/**************************************************************************/
;/*                                                                        */
;/*  FUNCTION                                               RELEASE        */
;/*                                                                        */
;/*    _tx_timer_interrupt                                  RXv2/IAR       */
;/*                                                           6.1.11       */
;/*  AUTHOR                                                                */
;/*                                                                        */
;/*    William E. Lamie, Microsoft Corporation                             */
;/*                                                                        */
;/*  DESCRIPTION                                                           */
;/*                                                                        */
;/*    This function processes the hardware timer interrupt.  This         */
;/*    processing includes incrementing the system clock and checking for  */
;/*    time slice and/or timer expiration.  If either is found, the        */
;/*    interrupt context save/restore functions are called along with the  */
;/*    expiration functions.                                               */
;/*                                                                        */
;/*  INPUT                                                                 */
;/*                                                                        */
;/*    None                                                                */
;/*                                                                        */
;/*  OUTPUT                                                                */
;/*                                                                        */
;/*    None                                                                */
;/*                                                                        */
;/*  CALLS                                                                 */
;/*                                                                        */
;/*    _tx_thread_context_save               Save interrupted context      */
;/*    _tx_timer_expiration_process          Timer expiration processing   */
;/*    _tx_thread_time_slice                 Time slice interrupted thread */
;/*    _tx_thread_context_restore            Restore interrupted context   */
;/*                                                                        */
;/*  CALLED BY                                                             */
;/*                                                                        */
;/*    interrupt vector                                                    */
;/*                                                                        */
;/*  RELEASE HISTORY                                                       */
;/*                                                                        */
;/*    DATE              NAME                      DESCRIPTION             */
;/*                                                                        */
;/*  12-30-2020     William E. Lamie         Initial Version 6.1.3         */
;/*  10-15-2021     William E. Lamie         Modified comment(s),          */
;/*                                            resulting in version 6.1.9  */
;/*  01-31-2022     William E. Lamie         Modified comment(s), and      */
;/*                                            added missing thread        */
;/*                                            preemption logic,           */
;/*                                            resulting in version 6.1.10 */
;/*  04-25-2022     William E. Lamie         Modified comment(s),          */
;/*                                            resulting in version 6.1.11 */
;/*                                                                        */
;/**************************************************************************/

    public __tx_timer_interrupt
__tx_timer_interrupt:
;
;    /* Upon entry to this routine, it is assumed that all interrupts are locked
;       out and the stack looks like the following:
;                            SP+4 ->    Interrupted PC
;                            SP+8->    Interrupted SR
;   */
;
;    /* Increment the system clock.  */
;    _tx_timer_system_clock++;
;
    PUSHM    R14-R15
    PUSHM    R1-R5

    MOV.L    #__tx_timer_system_clock, R1        ; Pickup address of system clock
    MOV.L    [R1], R2                            ; Pickup system clock
    ADD      #1, R2                              ; Increment system clock
    MOV.L    R2,[R1]                             ; Store new system clock
;
;    /* Test for time-slice expiration.  */
;    if (_tx_timer_time_slice)
;    {
;
    MOV.L   #__tx_timer_time_slice, R1           ; Pickup address of time slice
    MOV.L   [R1], R2                             ; Pickup the current time slice
    CMP     #0, R2                               ; Is a time slice active?
    BEQ     __tx_timer_no_time_slice             ; No, skip timer slice processing
;
;       /* Decrement the time_slice.  */
;       _tx_timer_time_slice--;
;
    SUB     #1, R2                               ; Decrement the time-slice
    MOV.L   R2, [R1]                             ; Store time-slice
;
;       /* Check for expiration.  */
;       if (__tx_timer_time_slice == 0)
;
    CMP     #0, R2                               ; Has it expired?
    BNE     __tx_timer_no_time_slice             ; No, time-slice has not expired
;
;       /* Set the time-slice expired flag.  */
;       _tx_timer_expired_time_slice =  TX_TRUE;
;
    MOV.L   #__tx_timer_expired_time_slice, R1   ; Pickup address of expired time-slice
    MOV.L   #1, R2                               ; Build expired value
    MOV.L   R2, [R1]                             ; Set expired time slice variable
;    }
;
__tx_timer_no_time_slice:
;
;    /* Test for timer expiration.  */
;    if (*_tx_timer_current_ptr)
;    {
;
    MOV.L   #__tx_timer_current_ptr, R1          ; Pickup address of current timer ptr
    MOV.L   [R1], R2                             ; Pickup current pointer
    MOV.L   [R2+], R1                            ; Pickup timer list entry, _tx_timer_current_ptr++
    CMP     #0, R1                               ; Is timer pointer NULL?
    BEQ     __tx_timer_no_timer                  ; Yes, no timer has expired
        
;
;        /* Set expiration flag.  */
;        _tx_timer_expired =  TX_TRUE;
;
    MOV.L   #__tx_timer_expired,R2               ; Build address of expired flag
    MOV.L   #1, R1                               ; Build expired value
    MOV.L   R1, [R2]    
    BRA     __tx_timer_done                      ; Finished with timer processing
;
;    }
;    else
;    {
__tx_timer_no_timer:
;
;        /* No timer expired, increment the timer pointer.  */
;        _tx_timer_current_ptr++;
;
;       /* R2 already contains __tx_timer_current_ptr++ */                         
;
;        /* Check for wrap-around.  */
;        if (_tx_timer_current_ptr == _tx_timer_list_end)
;
    MOV.L   #__tx_timer_list_end, R1             ; Pickup the timer list end ptr
    MOV.L   [R1], R1                             ; Pickup actual timer list end
    CMP     R1, R2                               ; Are we at list end?
    BNE     __tx_timer_skip_wrap                 ; No, don't move pointer to the
                                                 ;   top of the list
;
;            /* Wrap to beginning of list.  */
;            _tx_timer_current_ptr =  _tx_timer_list_start;
;
    MOV.L   #__tx_timer_list_start, R2           ; Pickup the timer list start ptr
    MOV.L   [R2], R2                             ; Pickup the start of the list
;    }
;
__tx_timer_skip_wrap:
    MOV.L   #__tx_timer_current_ptr,R1     
    MOV.L   R2, [R1]                             ; Store in updated pointer in  _tx_timer_current_ptr
    
__tx_timer_done:
;
;    /* See if anything has expired.  */
;    if ((_tx_timer_expired_time_slice) || (_tx_timer_expired))
;    {
;
    MOV.L  #__tx_timer_expired_time_slice, R1    ; Pickup expired time slice addr
    MOV.L  [R1], R1                              ; Pickup expired time slice
    MOV.L   #__tx_timer_expired, R2              ; Pickup expired timer flag address
    MOV.L   [R2], R2                             ; Pickup actual flag
    OR      R1, R2                               ; Or flags together
    BEQ     __tx_timer_nothing_expired           ; If Z set, nothing has expired

__tx_something_expired:
;    /* Did a timer expire?  */
;    if (_tx_timer_expired)
;    {
    MOV.L   #__tx_timer_expired,R1               ; Pickup expired flag address
    MOV.L   [R1], R1                             ; Pickup expired flag
    CMP     #0,R1                                ; Is the expired timer flag set?
    BEQ     __tx_timer_dont_activate             ; No, skip timer activation
;
;        /* Process timer expiration.  */
;        _tx_timer_expiration_process();
;
    BSR    __tx_timer_expiration_process         ; Call the timer expiration handling routine
;
;    }
__tx_timer_dont_activate:
;
;    /* Did time slice expire?  */
;    if (_tx_timer_expired_time_slice)
;    {
; 
    MOV.L   #__tx_timer_expired_time_slice, R1   ; Pickup time-slice expired flag addr
    MOV.L   [R1], R1                             ; Pickup actual flag
    CMP     #0,R1                                ; Has time-slice expired?
    BEQ      __tx_timer_not_ts_expiration        ; No, skip time-slice expiration
;
;        /* Time slice interrupted thread.  */
;        _tx_thread_time_slice(); 

    BSR     __tx_thread_time_slice               ; Call time-slice processing

;   /* Check if we must trigger a context switch. */
    MOV.L   #__tx_thread_preempt_disable, R1     ; Load prempt disable flag.
    MOV.L  [R1], R1
    CMP    #0, R1
    BNE    __tx_timer_not_ts_expiration          ; Skip if prempt disabled.

    MOV.L   #__tx_thread_execute_ptr, R1
    MOV.L  [R1], R1
    MOV.L   #__tx_thread_current_ptr, R2
    MOV.L  [R2], R2
    CMP    R1, R2
    BEQ    __tx_timer_not_ts_expiration

    MOV.L   #SWI0, R1
    MOV.L   #1, [R1]

;    }
;
__tx_timer_not_ts_expiration:

__tx_timer_nothing_expired:

    POPM R1-R5
    POPM R14-R15
;
    RTS                                          ; Return to point of interrupt
;
;}
    END
