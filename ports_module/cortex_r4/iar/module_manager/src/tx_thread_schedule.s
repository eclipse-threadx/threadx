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
;
;
    EXTERN     _tx_thread_execute_ptr
    EXTERN     _tx_thread_current_ptr
    EXTERN     _tx_timer_time_slice
    EXTERN     _tx_execution_thread_enter
;
    IMPORT     _txm_system_mode_enter
    IMPORT     _txm_system_mode_exit
;
THUMB_MASK          EQU     0x20            ; Thumb bit (5) of CPSR/SPSR.
IRQ_MODE            EQU     0x12            ; IRQ mode
USR_MODE            EQU     0x10            ; USR mode
SVC_MODE            EQU     0x13            ; SVC mode
SYS_MODE            EQU     0x1F            ; SYS mode
MODE_MASK           EQU     0x1F            ; Mode mask
;
;
;/**************************************************************************/ 
;/*                                                                        */ 
;/*  FUNCTION                                               RELEASE        */ 
;/*                                                                        */ 
;/*    _tx_thread_schedule                             Cortex-R4/MPU/IAR   */ 
;/*                                                           6.1          */
;/*  AUTHOR                                                                */
;/*                                                                        */
;/*    Scott Larson, Microsoft Corporation                                 */
;/*                                                                        */
;/*  DESCRIPTION                                                           */ 
;/*                                                                        */ 
;/*    This function waits for a thread control block pointer to appear in */ 
;/*    the _tx_thread_execute_ptr variable.  Once a thread pointer appears */ 
;/*    in the variable, the corresponding thread is resumed.               */ 
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
;/*    None                                                                */ 
;/*                                                                        */ 
;/*  CALLED BY                                                             */ 
;/*                                                                        */ 
;/*    _tx_initialize_kernel_enter          ThreadX entry function         */ 
;/*    _tx_thread_system_return             Return to system from thread   */ 
;/*    _tx_thread_context_restore           Restore thread's context       */ 
;/*                                                                        */ 
;/*  RELEASE HISTORY                                                       */ 
;/*                                                                        */ 
;/*    DATE              NAME                      DESCRIPTION             */
;/*                                                                        */
;/*  09-30-2020     Scott Larson             Initial Version 6.1           */
;/*                                                                        */
;/**************************************************************************/
;VOID   _tx_thread_schedule(VOID)
;{
    RSEG    .text:CODE:NOROOT(2)
    PUBLIC  _tx_thread_schedule
    ARM
_tx_thread_schedule??rA
_tx_thread_schedule

    ; Enter the scheduler.
    SVC     0

    ; We should never get here - ever!
_tx_scheduler_fault__
    B       _tx_scheduler_fault__
;}
; ****************************************************************************


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; SWI_Handler
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

    RSEG    .text:CODE:NOROOT(2)
    PUBLIC  SWI_Handler
    ARM
SWI_Handler

    STMFD   sp!, {r0-r3, r12, lr}               ; Store the registers
    MOV     r1, sp                              ; Set pointer to parameters
    MRS     r0, spsr                            ; Get spsr
    STMFD   sp!, {r0, r3}                       ; Store spsr onto stack and another
                                                ; register to maintain 8-byte-aligned stack
    TST     r0, #THUMB_MASK                     ; Occurred in Thumb state?
    LDRNEH  r0, [lr,#-2]                        ; Yes: Load halfword and...
    BICNE   r0, r0, #0xFF00                     ; ...extract comment field
    LDREQ   r0, [lr,#-4]                        ; No: Load word and...
    BICEQ   r0, r0, #0xFF000000                 ; ...extract comment field

    ; r0 now contains SVC number
    ; r1 now contains pointer to stacked registers

    ;
    ; The service call is handled here
    ;
    
    CMP     r0, #0                              ; Is it a schedule request?
    BEQ     _tx_handler_svc_schedule            ; Yes, go there

    CMP     r0, #1                              ; Is it a system mode enter request?
    BEQ     _tx_handler_svc_super_enter         ; Yes, go there

    CMP     r0, #2                              ; Is it a system mode exit request?
    BEQ     _tx_handler_svc_super_exit          ; Yes, go there
    
    LDR     r2, =0x123456
    CMP     r0, r2                              ; Is it an ARM request?
    BEQ     _tx_handler_svc_arm                 ; Yes, go there

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; Unknown SVC argument
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
    ; Unrecognized service call
_tx_handler_svc_unrecognized

_tx_handler_svc_unrecognized_loop               ; We should never get here
    B       _tx_handler_svc_unrecognized_loop

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; SVC 1
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
    ; At this point we have an SVC 1, which means we are entering 
    ; supervisor mode to service a kernel call.
_tx_handler_svc_super_enter
    ; Make sure that we have been called from the system mode enter location (security)
    LDR     r2, =_txm_system_mode_enter         ; Load the address of the known call point 
    SUB     r1, lr, #4                          ; Calculate the address of the actual call
    CMP     r1, r2                              ; Did we come from txm_module_manager_user_mode_entry?
    BNE     _tx_handler_svc_unrecognized        ; Return to where we came

    ; Clear the user mode flag in the thread structure
    LDR     r1, =_tx_thread_current_ptr         ; Load the current thread pointer address
    LDR     r2, [r1]                            ; Load current thread location from the pointer (pointer indirection)
    MOV     r1, #0                              ; Load the new user mode flag value (user mode flag clear -> not user mode -> system)
    STR     r1, [r2, #0x9C]                     ; Clear the current user mode selection for thread
    
    ; Now we enter the system mode and return
    LDMFD   sp!, {r0, r3}                       ; Get spsr from the stack
    BIC     r0, r0, #MODE_MASK                  ; clear mode field
    ORR     r0, r0, #SYS_MODE                   ; system mode code
    MSR     SPSR_cxsf, r0                       ; Restore the spsr
    
    LDR     r1, [r2, #0xA8]                     ; Load the module kernel stack pointer
    CPS     #SYS_MODE						    ; Switch to SYS mode
    MOV		r3, sp							    ; Grab thread stack pointer
    MOV		sp, r1							    ; Set SP to kernel stack pointer
    CPS     #SVC_MODE						    ; Switch back to SVC mode
	STR     r3, [r2, #0xB0]                     ; Save thread stack pointer
#ifndef TXM_MODULE_KERNEL_STACK_MAINTENANCE_DISABLE
    LDR     r3, [r2, #0xAC]                     ; Load the module kernel stack size
    STR     r3, [r2, #20]                       ; Set stack size
    LDRD    r0, r1, [r2, #0xA4]                 ; Load the module kernel stack start and end
    STRD    r0, r1, [r2, #0x0C]                 ; Set stack start and end
#endif
    LDMFD   sp!, {r0-r3, r12, pc}^              ; Restore the registers and return
    
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; SVC 2
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
    ; At this point we have an SVC 2, which means we are exiting 
    ; supervisor mode after servicing a kernel call.
_tx_handler_svc_super_exit
    ; Make sure that we have been called from the system mode exit location (security)
    LDR     r2, =_txm_system_mode_exit          ; Load the address of the known call point 
    SUB     r1, lr, #4                          ; Calculate the address of the actual call
    CMP     r1, r2                              ; Did we come from txm_module_manager_user_mode_entry?
    BNE     _tx_handler_svc_unrecognized        ; Return to where we came

    ; Set the user mode flag into the thread structure
    LDR     r1, =_tx_thread_current_ptr         ; Load the current thread pointer address
    LDR     r2, [r1]                            ; Load the current thread location from the pointer (pointer indirection)
    MOV     r1, #1                              ; Load the new user mode flag value (user mode enabled -> not system anymore)
    STR     r1, [r2, #0x9C]                     ; Clear the current user mode selection for thread

    ; Now we enter user mode (exit the system mode) and return
    LDMFD   sp!, {r0, r3}                       ; Get spsr from the stack
    BIC     r0, r0, #MODE_MASK                  ; clear mode field
    ORR     r0, r0, #USR_MODE                   ; user mode code
    MSR     SPSR_cxsf, r0                       ; Restore the spsr

    LDR     r1, [r2, #0xB0]                     ; Load the module thread stack pointer
    CPS     #SYS_MODE						    ; Switch to SYS mode
    MOV		sp, r1							    ; Set SP back to thread stack pointer
    CPS     #SVC_MODE						    ; Switch back to SVC mode
#ifndef TXM_MODULE_KERNEL_STACK_MAINTENANCE_DISABLE
    LDR     r3, [r2, #0xBC]                     ; Load the module thread stack size
    STR     r3, [r2, #20]                       ; Set stack size
    LDRD    r0, r1, [r2, #0xB4]                 ; Load the module thread stack start and end
    STRD    r0, r1, [r2, #0x0C]                 ; Set stack start and end
#endif
    LDMFD   sp!, {r0-r3, r12, pc}^              ; Restore the registers and return
    
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; ARM Semihosting
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
_tx_handler_svc_arm
    
    ; *** TODO: handle semihosting requests or ARM angel requests ***
    
    ; just return
    LDMFD   sp!, {r0, r3}                       ; Get spsr from the stack
    MSR     SPSR_cxsf, r0                       ; Restore the spsr
    LDMFD   sp!, {r0-r3, r12, pc}^              ; Restore the registers and return
    
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; SVC 0
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
    ; At this point we have an SVC 0: enter the scheduler.
_tx_handler_svc_schedule

    LDMFD   sp!, {r0, r3}                       ; Get spsr from stack
    MSR     SPSR_cxsf, r0                       ; Restore spsr
    LDMFD   sp!, {r0-r3, r12, lr}               ; Restore the registers 

    ; This code waits for a thread control block pointer to appear in 
    ; the _tx_thread_execute_ptr variable.  Once a thread pointer appears 
    ; in the variable, the corresponding thread is resumed.
;
;    /* Enable interrupts.  */
;
    CPSIE   i                                   ; Enable IRQ interrupts

;
;    /* Wait for a thread to execute.  */
;    do
;    {
    LDR     r1, =_tx_thread_execute_ptr         ; Address of thread execute ptr
;
__tx_thread_schedule_loop
    LDR     r0, [r1, #0]                        ; Pickup next thread to execute
    CMP     r0, #0                              ; Is it NULL?
    BEQ     __tx_thread_schedule_loop           ; If so, keep looking for a thread
;
;    }
;    while(_tx_thread_execute_ptr == TX_NULL);
;    
;    /* Yes! We have a thread to execute.  Lockout interrupts and
;       transfer control to it.  */
;
    CPSID   i                                   ; Disable interrupts
;
;    /* Setup the current thread pointer.  */
;    _tx_thread_current_ptr =  _tx_thread_execute_ptr;
;
    LDR     r1, =_tx_thread_current_ptr         ; Pickup address of current thread
    STR     r0, [r1, #0]                        ; Setup current thread pointer
;
;    /* Increment the run count for this thread.  */
;    _tx_thread_current_ptr -> tx_thread_run_count++;
;
    LDR     r2, [r0, #4]                        ; Pickup run counter
    LDR     r3, [r0, #24]                       ; Pickup time-slice for this thread
    ADD     r2, r2, #1                          ; Increment thread run-counter
    STR     r2, [r0, #4]                        ; Store the new run counter
;
;    /* Setup time-slice, if present.  */
;    _tx_timer_time_slice =  _tx_thread_current_ptr -> tx_thread_time_slice;
;
    LDR     r2, =_tx_timer_time_slice           ; Pickup address of time slice variable
    STR     r3, [r2, #0]                        ; Setup time-slice
;
#ifdef TX_ENABLE_EXECUTION_CHANGE_NOTIFY
;
;    /* Call the thread entry function to indicate the thread is executing.  */
;
    MOV     r5, r0                              ; Save r0
    BL      _tx_execution_thread_enter          ; Call the thread execution enter function
    MOV     r0, r5                              ; Restore r0
#endif

    ; Determine if an interrupt frame or a synchronous task suspension frame is present.
    CPS     #SYS_MODE                           ; Enter SYS mode
    LDR     sp, [r0, #8]                        ; Switch to thread stack pointer
    LDMIA   sp!, {r4, r5}                       ; Pickup the stack type and saved CPSR
    CPS     #SVC_MODE                           ; Enter SVC mode

    ; **************************************************************************
    ; Set up MPU for module.
    LDR     r1, [r0, #0x94]                     ; Pickup module instance pointer
    CMP     r1, #0                              ; Valid module pointer?
    BEQ     _tx_end_mpu_update                  ; No - skip memory protection setup
    LDR     r2, [r0, #0xA0]                     ; Pickup tx_thread_module_user_mode
    CMP     r2, #1                              ; In user mode?
    BNE     _tx_end_mpu_update                  ; No - skip memory protection setup
    ; Is the MPU already set up for this module?
    ; Pickup the first data entry to check (txm_module_instance_mpu_registers[5])
    LDR     r2, [r1, #0xA0]                     ; Pickup txm_module_instance_mpu_registers[5]
    MOV     r3, #5                              ; Select region 5
    MCR     p15, 0, r3, c6, c2, 0               ; Select region 5
    MRC     p15, 0, r3, c6, c1, 0               ; Read DRBAR into r3
    CMP     r2, r3                              ; Is module already loaded?
    BEQ     _tx_end_mpu_update                  ; Yes - skip memory protection setup
    
    ; Disable MPU before applying new regions.
    MRC     p15, 0, r2, c1, c0, 0               ; Read SCTLR
    BIC     r2, r2, #1                          ; Disable MPU
    DSB
    MCR     p15, 0, r2, c1, c0, 0               ; Write to SCTLR
    ISB
    ; Loop to load MPU registers
    MOV     r3, #0                              ; Loop index
    ADD     r1, r1, #0x64                       ; Build address of MPU register table
_tx_mpu_loop
    LDR     r2, [r1]                            ; Pickup txm_module_mpu_region_address
    MCR     p15, 0, r3, c6, c2, 0               ; Select region
    MCR     p15, 0, r2, c6, c1, 0               ; Write to DRBAR
    ADD     r1, r1, #4                          ; Increment to next MPU parameter
    LDR     r2, [r1]                            ; Pickup txm_module_mpu_region_size
    MCR     p15, 0, r2, c6, c1, 2               ; Write to DRSR
    ADD     r1, r1, #4                          ; Increment to next MPU parameter
    LDR     r2, [r1]                            ; Pickup txm_module_mpu_region_attributes
    MCR     p15, 0, r2, c6, c1, 4               ; Write to DRACR
    ADD     r1, r1, #4                          ; Increment to next MPU parameter
    ADD     r3, r3, #1                          ; Increment loop index
    CMP     r3, #0xB                            ; Check the limit
    BLE     _tx_mpu_loop                        ; Loop if not finished
    
    ; Enable MPU with new regions.
    MRC     p15, 0, r2, c1, c0, 0               ; Read SCTLR
    ORR     r2, r2, #1                          ; Enable MPU
    ORR     r2, r2, #0x20000                    ; Enable Background Region
    DSB
    MCR     p15, 0, r2, c1, c0, 0               ; Write to SCTLR
    ISB
    ; 
_tx_end_mpu_update
    ; **************************************************************************
    
    CMP     r4, #0                              ; Check for synchronous context switch
    BEQ     _tx_solicited_return
    
    MSR     SPSR_cxsf, r5                       ; Setup SPSR for return
    LDR     r1, [r0, #8]                        ; Get thread SP
    LDR     lr, [r1, #0x40]                     ; Get thread PC
    CPS     #SYS_MODE                           ; Enter SYS mode
    
#ifdef __ARMVFP__
    LDR     r2, [r0, #144]                      ; Pickup the VFP enabled flag
    CMP     r2, #0                              ; Is the VFP enabled?
    BEQ     _tx_skip_interrupt_vfp_restore      ; No, skip VFP interrupt restore
    VLDMIA  sp!, {D0-D15}                       ; Recover D0-D15
    LDR     r4, [sp], #4                        ; Pickup FPSCR
    VMSR    FPSCR, r4                           ; Restore FPSCR
    CPS     #SVC_MODE                           ; Enter SVC mode
    LDR     lr, [r1, #0x144]                    ; Get thread PC
    CPS     #SYS_MODE                           ; Enter SYS mode
_tx_skip_interrupt_vfp_restore
#endif
    
    LDMIA   sp!, {r0-r12, lr}                   ; Restore registers
    ADD     sp, sp, #4                          ; Fix stack pointer
    CPS     #SVC_MODE                           ; Enter SVC mode
    SUBS    pc, lr, #0                          ; Return to point of thread interrupt

_tx_solicited_return
    MOV     r2, r5                              ; Move CPSR to scratch register
    CPS     #SYS_MODE                           ; Enter SYS mode
    
#ifdef __ARMVFP__
    LDR     r1, [r0, #144]                      ; Pickup the VFP enabled flag
    CMP     r1, #0                              ; Is the VFP enabled?
    BEQ     _tx_skip_solicited_vfp_restore      ; No, skip VFP solicited restore
    VLDMIA  sp!, {D8-D15}                       ; Recover D8-D15
    LDR     r4, [sp], #4                        ; Pickup FPSCR
    VMSR    FPSCR, r4                           ; Restore FPSCR
_tx_skip_solicited_vfp_restore
#endif
    
    LDMIA   sp!, {r4-r11, lr}                   ; Restore registers
    MOV     r1, lr                              ; Copy lr to r1 to preserve across mode change
    CPS     #SVC_MODE                           ; Enter SVC mode
    MSR     SPSR_cxsf, r2                       ; Recover CPSR
    SUBS    pc, r1, #0                          ; Return to thread synchronously

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; End SWI_Handler
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
    
#ifdef __ARMVFP__
    PUBLIC  tx_thread_vfp_enable
    CODE32
tx_thread_vfp_enable??rA    
tx_thread_vfp_enable
    MRS     r2, CPSR                            ; Pickup the CPSR
    CPSID   i                                   ; Disable IRQ interrupts
    LDR     r0, =_tx_thread_current_ptr         ; Build current thread pointer address
    LDR     r1, [r0]                            ; Pickup current thread pointer
    CMP     r1, #0                              ; Check for NULL thread pointer
    BEQ     __tx_no_thread_to_enable            ; If NULL, skip VFP enable
    MOV     r0, #1                              ; Build enable value
    STR     r0, [r1, #144]                      ; Set the VFP enable flag (tx_thread_vfp_enable field in TX_THREAD)
__tx_no_thread_to_enable:
    MSR     CPSR_cxsf, r2                       ; Recover CPSR
    BX      LR                                  ; Return to caller

    PUBLIC  tx_thread_vfp_disable
    CODE32
tx_thread_vfp_disable??rA    
tx_thread_vfp_disable
    MRS     r2, CPSR                            ; Pickup the CPSR
    CPSID   i                                   ; Disable IRQ interrupts
    LDR     r0, =_tx_thread_current_ptr         ; Build current thread pointer address
    LDR     r1, [r0]                            ; Pickup current thread pointer
    CMP     r1, #0                              ; Check for NULL thread pointer
    BEQ     __tx_no_thread_to_disable           ; If NULL, skip VFP disable
    MOV     r0, #0                              ; Build disable value
    STR     r0, [r1, #144]                      ; Clear the VFP enable flag (tx_thread_vfp_enable field in TX_THREAD)
__tx_no_thread_to_disable:
    MSR     CPSR_cxsf, r2                       ; Recover CPSR
    BX      LR                                  ; Return to caller
#endif

    END

