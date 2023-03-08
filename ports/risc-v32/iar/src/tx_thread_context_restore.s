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


/* #define TX_SOURCE_CODE  */


/* Include necessary system files.  */

/*  #include "tx_api.h"
    #include "tx_thread.h"
    #include "tx_timer.h"  */
    
    EXTERN      _tx_thread_execute_ptr
    EXTERN      _tx_thread_current_ptr
    EXTERN      _tx_timer_time_slice
    EXTERN      _tx_thread_preempt_disable
    EXTERN      _tx_thread_schedule
    EXTERN      _tx_thread_system_state
#ifdef TX_ENABLE_EXECUTION_CHANGE_NOTIFY
    EXTERN      _tx_execution_isr_exit
#endif


    SECTION `.text`:CODE:REORDER:NOROOT(2)
    CODE
/**************************************************************************/ 
/*                                                                        */ 
/*  FUNCTION                                               RELEASE        */ 
/*                                                                        */ 
/*    _tx_thread_context_restore                         RISC-V32/IAR     */
/*                                                           6.1          */
/*  AUTHOR                                                                */ 
/*                                                                        */ 
/*    William E. Lamie, Microsoft Corporation                             */ 
/*    Tom van Leeuwen, Technolution B.V.                                  */
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
/*  09-30-2020     William E. Lamie         Initial Version 6.1           */ 
/*                                                                        */ 
/**************************************************************************/ 
/* VOID   _tx_thread_context_restore(VOID)
{  */
    PUBLIC  _tx_thread_context_restore
_tx_thread_context_restore:

    /* Lockout interrupts.  */

    csrci   mstatus, 0x08                               ; Disable interrupts

#ifdef TX_ENABLE_EXECUTION_CHANGE_NOTIFY
    call    _tx_execution_isr_exit                      ; Call the ISR execution exit function
#endif

    /* Determine if interrupts are nested.  */
    /* if (--_tx_thread_system_state)
    {  */

    la      t0, _tx_thread_system_state                 ; Pickup addr of nested interrupt count
    lw      t1, 0(t0)                                   ; Pickup nested interrupt count
    addi    t1, t1, -1                                  ; Decrement the nested interrupt counter
    sw      t1, 0(t0)                                   ; Store new nested count
    beqz    t1, _tx_thread_not_nested_restore           ; If 0, not nested restore

    /* Interrupts are nested.  */

    /* Just recover the saved registers and return to the point of 
       interrupt.  */
       
#if __iar_riscv_base_isa == rv32e 

    /* Recover floating point registers.  */

    flw     f0, 0x7C(sp)                                ; Recover ft0
    flw     f1, 0x80(sp)                                ; Recover ft1
    flw     f2, 0x84(sp)                                ; Recover ft2
    flw     f3, 0x88(sp)                                ; Recover ft3
    flw     f4, 0x8C(sp)                                ; Recover ft4
    flw     f5, 0x90(sp)                                ; Recover ft5
    flw     f6, 0x94(sp)                                ; Recover ft6
    flw     f7, 0x98(sp)                                ; Recover ft7
    flw     f10,0xA4(sp)                                ; Recover fa0
    flw     f11,0xA8(sp)                                ; Recover fa1
    flw     f12,0xAC(sp)                                ; Recover fa2
    flw     f13,0xB0(sp)                                ; Recover fa3
    flw     f14,0xB4(sp)                                ; Recover fa4
    flw     f15,0xB8(sp)                                ; Recover fa5
    flw     f16,0xBC(sp)                                ; Recover fa6
    flw     f17,0xC0(sp)                                ; Recover fa7
    flw     f28,0xEC(sp)                                ; Recover ft8
    flw     f29,0xF0(sp)                                ; Recover ft9
    flw     f30,0xF4(sp)                                ; Recover ft10
    flw     f31,0xF8(sp)                                ; Recover ft11
    lw      t0, 0xFC(sp)                                ; Recover fcsr
    csrw    fcsr, t0                                    ; 
#endif

    /* Recover standard registers.  */

    /* Restore registers,
       Skip global pointer because that does not change
       Also skip the saved registers since they have been restored by any function we called.
       Except s0 since we use it ourselves. */

    lw      t0, 0x78(sp)                                ; Recover mepc
    csrw    mepc, t0                                    ; Setup mepc
    li      t0, 0x1880                                  ; Prepare MPIP
    csrw    mstatus, t0                                 ; Enable MPIP

    lw      x1, 0x70(sp)                                ; Recover RA
    lw      x5, 0x4C(sp)                                ; Recover t0
    lw      x6, 0x48(sp)                                ; Recover t1
    lw      x7, 0x44(sp)                                ; Recover t2
    lw      x8, 0x30(sp)                                ; Recover s0
    lw      x10, 0x6C(sp)                               ; Recover a0
    lw      x11, 0x68(sp)                               ; Recover a1
    lw      x12, 0x64(sp)                               ; Recover a2
    lw      x13, 0x60(sp)                               ; Recover a3
    lw      x14, 0x5C(sp)                               ; Recover a4
    lw      x15, 0x58(sp)                               ; Recover a5
    lw      x16, 0x54(sp)                               ; Recover a6
    lw      x17, 0x50(sp)                               ; Recover a7
    lw      x28, 0x40(sp)                               ; Recover t3
    lw      x29, 0x3C(sp)                               ; Recover t4
    lw      x30, 0x38(sp)                               ; Recover t5
    lw      x31, 0x34(sp)                               ; Recover t6

#if __iar_riscv_base_isa == rv32e 
    addi    sp, sp, 260                                 ; Recover stack frame - with floating point enabled
#else
    addi    sp, sp, 128                                 ; Recover stack frame - without floating point enabled
#endif
    mret                                                ; Return to point of interrupt

    /* }  */
_tx_thread_not_nested_restore:
    /* Determine if a thread was interrupted and no preemption is required.  */
    /* else if (((_tx_thread_current_ptr) && (_tx_thread_current_ptr == _tx_thread_execute_ptr))
               || (_tx_thread_preempt_disable))
    {  */

    lw      t1, _tx_thread_current_ptr                  ; Pickup current thread pointer
    beqz    t1, _tx_thread_idle_system_restore          ; If NULL, idle system restore

    lw      t2, _tx_thread_preempt_disable              ; Pickup preempt disable flag
    bgtz    t2, _tx_thread_no_preempt_restore           ; If set, restore interrupted thread

    lw      t2, _tx_thread_execute_ptr                  ; Pickup thread execute pointer
    bne     t1, t2, _tx_thread_preempt_restore          ; If higher-priority thread is ready, preempt


_tx_thread_no_preempt_restore:
    /* Restore interrupted thread or ISR.  */

    /* Pickup the saved stack pointer.  */
    /* SP =  _tx_thread_current_ptr -> tx_thread_stack_ptr;  */

    lw      sp, 8(t1)                                   ; Switch back to thread's stack

#if __iar_riscv_base_isa == rv32e 

    /* Recover floating point registers.  */

    flw     f0, 0x7C(sp)                                ; Recover ft0
    flw     f1, 0x80(sp)                                ; Recover ft1
    flw     f2, 0x84(sp)                                ; Recover ft2
    flw     f3, 0x88(sp)                                ; Recover ft3
    flw     f4, 0x8C(sp)                                ; Recover ft4
    flw     f5, 0x90(sp)                                ; Recover ft5
    flw     f6, 0x94(sp)                                ; Recover ft6
    flw     f7, 0x98(sp)                                ; Recover ft7
    flw     f10,0xA4(sp)                                ; Recover fa0
    flw     f11,0xA8(sp)                                ; Recover fa1
    flw     f12,0xAC(sp)                                ; Recover fa2
    flw     f13,0xB0(sp)                                ; Recover fa3
    flw     f14,0xB4(sp)                                ; Recover fa4
    flw     f15,0xB8(sp)                                ; Recover fa5
    flw     f16,0xBC(sp)                                ; Recover fa6
    flw     f17,0xC0(sp)                                ; Recover fa7
    flw     f28,0xEC(sp)                                ; Recover ft8
    flw     f29,0xF0(sp)                                ; Recover ft9
    flw     f30,0xF4(sp)                                ; Recover ft10
    flw     f31,0xF8(sp)                                ; Recover ft11
    lw      t0, 0xFC(sp)                                ; Recover fcsr
    csrw    fcsr, t0                                    ; 
#endif

    /* Recover the saved context and return to the point of interrupt.  */

    /* Recover standard registers.  */
    /* Restore registers,
       Skip global pointer because that does not change */

    lw      t0, 0x78(sp)                                ; Recover mepc
    csrw    mepc, t0                                    ; Setup mepc
    li      t0, 0x1880                                  ; Prepare MPIP
    csrw    mstatus, t0                                 ; Enable MPIP

    lw      x1, 0x70(sp)                                ; Recover RA
    lw      x5, 0x4C(sp)                                ; Recover t0
    lw      x6, 0x48(sp)                                ; Recover t1
    lw      x7, 0x44(sp)                                ; Recover t2
    lw      x8, 0x30(sp)                                ; Recover s0
    lw      x10, 0x6C(sp)                               ; Recover a0
    lw      x11, 0x68(sp)                               ; Recover a1
    lw      x12, 0x64(sp)                               ; Recover a2
    lw      x13, 0x60(sp)                               ; Recover a3
    lw      x14, 0x5C(sp)                               ; Recover a4
    lw      x15, 0x58(sp)                               ; Recover a5
    lw      x16, 0x54(sp)                               ; Recover a6
    lw      x17, 0x50(sp)                               ; Recover a7
    lw      x28, 0x40(sp)                               ; Recover t3
    lw      x29, 0x3C(sp)                               ; Recover t4
    lw      x30, 0x38(sp)                               ; Recover t5
    lw      x31, 0x34(sp)                               ; Recover t6

#if __iar_riscv_base_isa == rv32e 
    addi    sp, sp, 260                                 ; Recover stack frame - with floating point enabled
#else
    addi    sp, sp, 128                                 ; Recover stack frame - without floating point enabled
#endif
    mret                                                ; Return to point of interrupt

    /* }
    else
    {  */
_tx_thread_preempt_restore:
    /* Instead of directly activating the thread again, ensure we save the
       entire stack frame by saving the remaining registers. */

    lw      t0, 8(t1)                                   ; Pickup thread's stack pointer
    ori     t3, x0, 1                                   ; Build interrupt stack type
    sw      t3, 0(t0)                                   ; Store stack type

#if __iar_riscv_base_isa == rv32e 

    /* Store floating point preserved registers.  */
 
    fsw     f8,  0x9C(t0)                               ; Store fs0
    fsw     f9,  0xA0(t0)                               ; Store fs1
    fsw     f18, 0xC4(t0)                               ; Store fs2
    fsw     f19, 0xC8(t0)                               ; Store fs3
    fsw     f20, 0xCC(t0)                               ; Store fs4
    fsw     f21, 0xD0(t0)                               ; Store fs5
    fsw     f22, 0xD4(t0)                               ; Store fs6
    fsw     f23, 0xD8(t0)                               ; Store fs7
    fsw     f24, 0xDC(t0)                               ; Store fs8
    fsw     f25, 0xE0(t0)                               ; Store fs9
    fsw     f26, 0xE4(t0)                               ; Store fs10
    fsw     f27, 0xE8(t0)                               ; Store fs11
#endif

    /* Store standard preserved registers.  */

    sw      x9, 0x2C(t0)                                ; Store s1
    sw      x18, 0x28(t0)                               ; Store s2
    sw      x19, 0x24(t0)                               ; Store s3
    sw      x20, 0x20(t0)                               ; Store s4
    sw      x21, 0x1C(t0)                               ; Store s5
    sw      x22, 0x18(t0)                               ; Store s6
    sw      x23, 0x14(t0)                               ; Store s7
    sw      x24, 0x10(t0)                               ; Store s8
    sw      x25, 0x0C(t0)                               ; Store s9
    sw      x26, 0x08(t0)                               ; Store s10
    sw      x27, 0x04(t0)                               ; Store s11
                                                        ; Note: s0 is already stored!

    /* Save the remaining time-slice and disable it.  */
    /* if (_tx_timer_time_slice)
    {  */

    la      t0, _tx_timer_time_slice                    ; Pickup time slice variable address
    lw      t2, 0(t0)                                   ; Pickup time slice
    beqz    t2, _tx_thread_dont_save_ts                 ; If 0, skip time slice processing

        /* _tx_thread_current_ptr -> tx_thread_time_slice =  _tx_timer_time_slice
        _tx_timer_time_slice =  0;  */

    sw      t2, 24(t1)                                  ; Save current time slice
    sw      x0, 0(t0)                                   ; Clear global time slice


    /* }  */
_tx_thread_dont_save_ts:
    /* Clear the current task pointer.  */
    /* _tx_thread_current_ptr =  TX_NULL;  */

    /* Return to the scheduler.  */
    /* _tx_thread_schedule();  */

    sw      x0, _tx_thread_current_ptr, t0              ; Clear current thread pointer*/
    /* }  */

_tx_thread_idle_system_restore:
    /* Just return back to the scheduler!  */
    j       _tx_thread_schedule                         ; Return to scheduler

/* }  */
    END
