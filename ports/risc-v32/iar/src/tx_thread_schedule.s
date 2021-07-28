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
#ifdef TX_ENABLE_EXECUTION_CHANGE_NOTIFY
    EXTERN      _tx_execution_thread_enter
#endif


    SECTION `.text`:CODE:REORDER:NOROOT(2)
    CODE
/**************************************************************************/ 
/*                                                                        */ 
/*  FUNCTION                                               RELEASE        */ 
/*                                                                        */ 
/*    _tx_thread_schedule                                RISC-V32/IAR     */
/*                                                           6.1          */
/*  AUTHOR                                                                */ 
/*                                                                        */ 
/*    William E. Lamie, Microsoft Corporation                             */ 
/*    Tom van Leeuwen, Technolution B.V.                                  */
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
/*  09-30-2020     William E. Lamie         Initial Version 6.1           */ 
/*                                                                        */ 
/**************************************************************************/ 
/* VOID   _tx_thread_schedule(VOID)
{  */
    PUBLIC  _tx_thread_schedule
_tx_thread_schedule:

    /* Enable interrupts.  */
    csrsi   mstatus, 0x08                               ; Enable interrupts

    /* Wait for a thread to execute.  */
    /* do
    {  */

    la      t0, _tx_thread_execute_ptr                  ; Pickup address of execute ptr
_tx_thread_schedule_loop:
    lw      t1, 0(t0)                                   ; Pickup next thread to execute
    beqz    t1, _tx_thread_schedule_loop                ; If NULL, wait for thread to execute

    /* }
    while(_tx_thread_execute_ptr == TX_NULL);  */
    
    /* Yes! We have a thread to execute.  Lockout interrupts and
       transfer control to it.  */
    csrci   mstatus, 0x08                               ; Lockout interrupts

    /* Setup the current thread pointer.  */
    /* _tx_thread_current_ptr =  _tx_thread_execute_ptr;  */

    la      t0, _tx_thread_current_ptr                  ; Pickup current thread pointer address
    sw      t1, 0(t0)                                   ; Set current thread pointer

    /* Increment the run count for this thread.  */
    /* _tx_thread_current_ptr -> tx_thread_run_count++;  */

    lw      t2, 4(t1)                                   ; Pickup run count
    lw      t3, 24(t1)                                  ; Pickup time slice value
    addi    t2, t2, 1                                   ; Increment run count
    sw      t2, 4(t1)                                   ; Store new run count

    /* Setup time-slice, if present.  */
    /* _tx_timer_time_slice =  _tx_thread_current_ptr -> tx_thread_time_slice;  */

    la      t2, _tx_timer_time_slice                    ; Pickup time-slice variable address

    /* Switch to the thread's stack.  */
    /* SP =  _tx_thread_execute_ptr -> tx_thread_stack_ptr;  */

    lw      sp, 8(t1)                                   ; Switch to thread's stack
    sw      t3, 0(t2)                                   ; Store new time-slice*/

#ifdef TX_ENABLE_EXECUTION_CHANGE_NOTIFY

    call    _tx_execution_thread_enter                  ; Call the thread execution enter function
#endif

    /* Determine if an interrupt frame or a synchronous task suspension frame
       is present.  */

    lw      t2, 0(sp)                                   ; Pickup stack type
    beqz    t2, _tx_thread_synch_return                 ; If 0, solicited thread return

    /* Determine if floating point registers need to be recovered.  */

#if __iar_riscv_base_isa == rv32e 
    flw     f0, 0x7C(sp)                                ; Recover ft0
    flw     f1, 0x80(sp)                                ; Recover ft1
    flw     f2, 0x84(sp)                                ; Recover ft2
    flw     f3, 0x88(sp)                                ; Recover ft3
    flw     f4, 0x8C(sp)                                ; Recover ft4
    flw     f5, 0x90(sp)                                ; Recover ft5
    flw     f6, 0x94(sp)                                ; Recover ft6
    flw     f7, 0x98(sp)                                ; Recover ft7
    flw     f8, 0x9C(sp)                                ; Recover fs0
    flw     f9, 0xA0(sp)                                ; Recover fs1
    flw     f10,0xA4(sp)                                ; Recover fa0
    flw     f11,0xA8(sp)                                ; Recover fa1
    flw     f12,0xAC(sp)                                ; Recover fa2
    flw     f13,0xB0(sp)                                ; Recover fa3
    flw     f14,0xB4(sp)                                ; Recover fa4
    flw     f15,0xB8(sp)                                ; Recover fa5
    flw     f16,0xBC(sp)                                ; Recover fa6
    flw     f17,0xC0(sp)                                ; Recover fa7
    flw     f18,0xC4(sp)                                ; Recover fs2
    flw     f19,0xC8(sp)                                ; Recover fs3
    flw     f20,0xCC(sp)                                ; Recover fs4
    flw     f21,0xD0(sp)                                ; Recover fs5
    flw     f22,0xD4(sp)                                ; Recover fs6
    flw     f23,0xD8(sp)                                ; Recover fs7
    flw     f24,0xDC(sp)                                ; Recover fs8
    flw     f25,0xE0(sp)                                ; Recover fs9
    flw     f26,0xE4(sp)                                ; Recover fs10
    flw     f27,0xE8(sp)                                ; Recover fs11
    flw     f28,0xEC(sp)                                ; Recover ft8
    flw     f29,0xF0(sp)                                ; Recover ft9
    flw     f30,0xF4(sp)                                ; Recover ft10
    flw     f31,0xF8(sp)                                ; Recover ft11
    lw      t0, 0xFC(sp)                                ; Recover fcsr
    csrw    fcsr, t0                                    ; 
#endif

    /* Recover standard registers.  */

    lw      t0, 0x78(sp)                                ; Recover mepc
    csrw    mepc, t0                                    ; Store mepc
    li      t0, 0x1880                                  ; Prepare MPIP
    csrw    mstatus, t0                                 ; Enable MPIP

    lw      x1, 0x70(sp)                                ; Recover RA
    lw      x5, 0x4C(sp)                                ; Recover t0
    lw      x6, 0x48(sp)                                ; Recover t1
    lw      x7, 0x44(sp)                                ; Recover t2
    lw      x8, 0x30(sp)                                ; Recover s0
    lw      x9, 0x2C(sp)                                ; Recover s1
    lw      x10, 0x6C(sp)                               ; Recover a0
    lw      x11, 0x68(sp)                               ; Recover a1
    lw      x12, 0x64(sp)                               ; Recover a2
    lw      x13, 0x60(sp)                               ; Recover a3
    lw      x14, 0x5C(sp)                               ; Recover a4
    lw      x15, 0x58(sp)                               ; Recover a5
    lw      x16, 0x54(sp)                               ; Recover a6
    lw      x17, 0x50(sp)                               ; Recover a7
    lw      x18, 0x28(sp)                               ; Recover s2
    lw      x19, 0x24(sp)                               ; Recover s3
    lw      x20, 0x20(sp)                               ; Recover s4
    lw      x21, 0x1C(sp)                               ; Recover s5
    lw      x22, 0x18(sp)                               ; Recover s6
    lw      x23, 0x14(sp)                               ; Recover s7
    lw      x24, 0x10(sp)                               ; Recover s8
    lw      x25, 0x0C(sp)                               ; Recover s9
    lw      x26, 0x08(sp)                               ; Recover s10
    lw      x27, 0x04(sp)                               ; Recover s11
    lw      x28, 0x40(sp)                               ; Recover t3
    lw      x29, 0x3C(sp)                               ; Recover t4
    lw      x30, 0x38(sp)                               ; Recover t5
    lw      x31, 0x34(sp)                               ; Recover t6

#if __iar_riscv_base_isa == rv32e 
    addi    sp, sp, 260                                 ; Recover stack frame - with floating point registers  
#else
    addi    sp, sp, 128                                 ; Recover stack frame - without floating point registers
#endif
    mret                                                ; Return to point of interrupt

_tx_thread_synch_return:

#if __iar_riscv_base_isa == rv32e 
    flw     f8, 0x3C(sp)                                ; Recover fs0
    flw     f9, 0x40(sp)                                ; Recover fs1
    flw     f18,0x44(sp)                                ; Recover fs2
    flw     f19,0x48(sp)                                ; Recover fs3
    flw     f20,0x4C(sp)                                ; Recover fs4
    flw     f21,0x50(sp)                                ; Recover fs5
    flw     f22,0x54(sp)                                ; Recover fs6
    flw     f23,0x58(sp)                                ; Recover fs7
    flw     f24,0x5C(sp)                                ; Recover fs8
    flw     f25,0x60(sp)                                ; Recover fs9
    flw     f26,0x64(sp)                                ; Recover fs10
    flw     f27,0x68(sp)                                ; Recover fs11
    lw      t0, 0x6C(sp)                                ; Recover fcsr
    csrw    fcsr, t0                                    ; 
#endif

    /* Recover standard preserved registers.  */
    /* Recover standard registers.  */

    lw      x1, 0x34(sp)                                ; Recover RA
    lw      x8, 0x30(sp)                                ; Recover s0
    lw      x9, 0x2C(sp)                                ; Recover s1
    lw      x18, 0x28(sp)                               ; Recover s2
    lw      x19, 0x24(sp)                               ; Recover s3
    lw      x20, 0x20(sp)                               ; Recover s4
    lw      x21, 0x1C(sp)                               ; Recover s5
    lw      x22, 0x18(sp)                               ; Recover s6
    lw      x23, 0x14(sp)                               ; Recover s7
    lw      x24, 0x10(sp)                               ; Recover s8
    lw      x25, 0x0C(sp)                               ; Recover s9
    lw      x26, 0x08(sp)                               ; Recover s10
    lw      x27, 0x04(sp)                               ; Recover s11
    lw      t0, 0x38(sp)                                ; Recover mstatus
    csrw    mstatus, t0                                 ; Store mstatus, enables interrupt
#if __iar_riscv_base_isa == rv32e 
    addi    sp, sp, 116                                 ; Recover stack frame
#else
    addi    sp, sp, 64                                  ; Recover stack frame
#endif
    ret                                                 ; Return to thread

/* }  */
    END
    
