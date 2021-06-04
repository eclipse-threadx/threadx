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
/**   Initialize                                                          */
/**                                                                       */
/**************************************************************************/
/**************************************************************************/


/* #define TX_SOURCE_CODE  */


/* Include necessary system files.  */

/*
#include "tx_api.h"
#include "tx_initialize.h"
#include "tx_thread.h"
#include "tx_timer.h"
*/

#define THUMB_MASK      0x20                    // Thumb bit (5) of CPSR/SPSR
#define ABT_MODE        0x17                    // ABT mode
#define SYS_MODE        0x1F                    // SYS mode

#define GICI_BASE       0xAE000000
#define ICCIAR_OFFSET   0x0000000C
#define ICCEOIR_OFFSET  0x00000010


    .global      _tx_thread_system_stack_ptr
    .global      _tx_initialize_unused_memory
    .global      _tx_thread_context_save
    .global      _tx_thread_context_restore
#ifdef TX_ENABLE_FIQ_SUPPORT
    .global      _tx_thread_fiq_context_save
    .global      _tx_thread_fiq_context_restore
#endif
#ifdef TX_ENABLE_IRQ_NESTING
    .global      _tx_thread_irq_nesting_start
    .global      _tx_thread_irq_nesting_end
#endif
#ifdef TX_ENABLE_FIQ_NESTING
    .global      _tx_thread_fiq_nesting_start
    .global      _tx_thread_fiq_nesting_end
#endif
    .global      _tx_timer_interrupt
    .global      __main
    .global      _tx_version_id
    .global      _tx_build_options

#ifdef TX_THUMB_MODE
    .thumb
#else
    .arm
#endif
    .text
    .eabi_attribute Tag_ABI_align_preserved, 1
/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _tx_initialize_low_level                          Cortex-R4/ARM     */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Scott Larson, Microsoft Corporation                                 */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function is responsible for any low-level processor            */
/*    initialization, including setting up interrupt vectors, setting     */
/*    up a periodic timer interrupt source, saving the system stack       */
/*    pointer for use in ISR processing later, and finding the first      */
/*    available RAM memory address for tx_application_define.             */
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
/*    _tx_initialize_kernel_enter           ThreadX entry function        */
/*                                                                        */
/*  RELEASE HISTORY                                                       */
/*                                                                        */
/*    DATE              NAME                      DESCRIPTION             */
/*                                                                        */
/*  09-30-2020     Scott Larson             Initial Version 6.1           */
/*                                                                        */
/**************************************************************************/
/* VOID   _tx_initialize_low_level(VOID)
{ */
    .global _tx_initialize_low_level
    .type   _tx_initialize_low_level, "function"
_tx_initialize_low_level:

     /* Save the system stack pointer.  */
     /* _tx_thread_system_stack_ptr = (VOID_PTR) (sp); */

    LDR     r0, =Image$$SVC_STACK$$ZI$$Limit
    LDR     r1, =_tx_thread_system_stack_ptr    // Pickup address of system stack ptr
    STR     r0, [r1]                            // Pickup system stack

     /* Save the first available memory address.  */
     /* _tx_initialize_unused_memory =  (VOID_PTR) Image$$ZI$$Limit   + HEAP + [SYS_STACK] + FIQ_STACK + IRQ_STACK; */

    LDR     r0, =Image$$DATA$$ZI$$Limit
    LDR     r2, =_tx_initialize_unused_memory   // Pickup unused memory ptr address
    STR     r0, [r2, #0]                        // Save first free memory address

    /* Return to caller.  */
    BX      lr                                  // Return to caller
/* } */

    /* Define shells for each of the interrupt vectors.  */

    .global __tx_undefined
    .type   __tx_undefined, "function"
__tx_undefined:
    B       __tx_undefined                      // Undefined handler

/*** Prefetch and abort handlers are used below for MPU fault handling
    .global __tx_prefetch_handler
    .type   __tx_prefetch_handler, "function"
__tx_prefetch_handler:
    B       __tx_prefetch_handler               // Prefetch exception handler

    .global __tx_abort_handler
    .type   __tx_abort_handler, "function"
__tx_abort_handler:
    B       __tx_abort_handler                  // Abort exception handler
*/

    .global __tx_reserved_handler
    .type   __tx_reserved_handler, "function"
__tx_reserved_handler:
    B       __tx_reserved_handler               // Reserved exception handler


    .global __tx_irq_handler
    .type   __tx_irq_handler, "function"
    .global __tx_irq_processing_return
    .type   __tx_irq_processing_return, "function"
__tx_irq_handler:

    /* Jump to context save to save system context.  */
    B       _tx_thread_context_save
__tx_irq_processing_return:

    /* Acknowledge the interrupt.  */
    LDR     r1, =GICI_BASE                      // Load the base of the GIC
    LDR     r0, [r1, #ICCIAR_OFFSET]            // Read ICCIAR (GIC CPU Interface register)
    DSB                                         // Ensure that interrupt acknowledge completes before re-enabling interrupts
    PUSH    {r0, r1}                            // Save the IRQ ID and the GIC base address on the stack

    /* Clear the timer interrupt.  */
    LDR     r0, =0xB0110000                     // Load the base address of the timer
    MOV     r1, #1                              // Setup value to write to the interrupt clear register - can be anything.
    STR     r1, [r0, #0x0C]                     // Clear the interrupt. 0x0C is the offset to the interrupt clear register.

    /* At this point execution is still in the IRQ mode.  The CPSR, point of
       interrupt, and all C scratch registers are available for use.  In
       addition, IRQ interrupts may be re-enabled - with certain restrictions -
       if nested IRQ interrupts are desired.  Interrupts may be re-enabled over
       small code sequences where lr is saved before enabling interrupts and
       restored after interrupts are again disabled.  */


    BL      _tx_timer_interrupt                 // Timer interrupt handler
_tx_not_timer_interrupt:

    /* Interrupt nesting is allowed after calling _tx_thread_irq_nesting_start
       from IRQ mode with interrupts disabled.  This routine switches to the
       system mode and returns with IRQ interrupts enabled. */

       /* NOTE:  It is very important to ensure all IRQ interrupts are cleared
          prior to enabling nested IRQ interrupts.  */
#ifdef TX_ENABLE_IRQ_NESTING
    BL      _tx_thread_irq_nesting_start
#endif


    /* Application IRQ handlers can be called here!  */

    /* If interrupt nesting was started earlier, the end of interrupt nesting
       service must be called before returning to _tx_thread_context_restore.
       This routine returns in processing in IRQ mode with interrupts disabled.  */
#ifdef TX_ENABLE_IRQ_NESTING
    BL      _tx_thread_irq_nesting_end
#endif

    POP     {r0, r1}                            // Restore the IRQ ID and GIC base address
    STR     r0, [r1, #ICCEOIR_OFFSET]           // Write the IRQ ID to the End Of Interrupt register to clear the active bit

    /* Jump to context restore to restore system context.  */
    B       _tx_thread_context_restore


    /* This is an example of a vectored IRQ handler.  */

    .global __tx_example_vectored_irq_handler
    .type   __tx_example_vectored_irq_handler, "function"
__tx_example_vectored_irq_handler:


    /* Save initial context and call context save to prepare for
       vectored ISR execution. */

/*
    STMDB   sp!, {r0-r3}                        // Save some scratch registers
    MRS     r0, SPSR                            // Pickup saved SPSR
    SUB     lr, lr, #4                          // Adjust point of interrupt
    STMDB   sp!, {r0, r10, r12, lr}             // Store other scratch registers
    BL      _tx_thread_vectored_context_save    // Vectored context save
*/

    /* At this point execution is still in the IRQ mode.  The CPSR, point of
       interrupt, and all C scratch registers are available for use.  In
       addition, IRQ interrupts may be re-enabled - with certain restrictions -
       if nested IRQ interrupts are desired.  Interrupts may be re-enabled over
       small code sequences where lr is saved before enabling interrupts and
       restored after interrupts are again disabled.  */


    /* Interrupt nesting is allowed after calling _tx_thread_irq_nesting_start
       from IRQ mode with interrupts disabled.  This routine switches to the
       system mode and returns with IRQ interrupts enabled. */

       /* NOTE:  It is very important to ensure all IRQ interrupts are cleared
          prior to enabling nested IRQ interrupts.  */
/*
#ifdef TX_ENABLE_IRQ_NESTING
    BL      _tx_thread_irq_nesting_start
#endif
*/

    /* Application IRQ handlers can be called here!  */

    /* If interrupt nesting was started earlier, the end of interrupt nesting
       service must be called before returning to _tx_thread_context_restore.
       This routine returns in processing in IRQ mode with interrupts disabled.  */
/*
#ifdef TX_ENABLE_IRQ_NESTING
    BL      _tx_thread_irq_nesting_end
#endif
*/

    /* Jump to context restore to restore system context.  */
/*
    B       _tx_thread_context_restore
*/


#ifdef TX_ENABLE_FIQ_SUPPORT
    .global __tx_fiq_handler
    .type   __tx_fiq_handler, "function"
__tx_fiq_handler:

    /* Jump to fiq context save to save system context.  */
    B       _tx_thread_fiq_context_save

    .global __tx_fiq_processing_return
    .type   __tx_fiq_processing_return, "function"
__tx_fiq_processing_return:

    /* At this point execution is still in the FIQ mode.  The CPSR, point of
       interrupt, and all C scratch registers are available for use.  */

    /* Interrupt nesting is allowed after calling _tx_thread_fiq_nesting_start
       from FIQ mode with interrupts disabled.  This routine switches to the
       system mode and returns with FIQ interrupts enabled. */

       /* NOTE:  It is very important to ensure all FIQ interrupts are cleared
          prior to enabling nested FIQ interrupts.  */
#ifdef TX_ENABLE_FIQ_NESTING
    BL      _tx_thread_fiq_nesting_start
#endif

    /* Application FIQ handlers can be called here!  */

    /* If interrupt nesting was started earlier, the end of interrupt nesting
       service must be called before returning to _tx_thread_fiq_context_restore.  */
#ifdef TX_ENABLE_FIQ_NESTING
    BL      _tx_thread_fiq_nesting_end
#endif

    /* Jump to fiq context restore to restore system context.  */
    B       _tx_thread_fiq_context_restore


#else
    .global __tx_fiq_handler
    .type   __tx_fiq_handler, "function"
__tx_fiq_handler:
    B       __tx_fiq_handler                    // FIQ interrupt handler
#endif

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    __tx_prefetch_handler & __tx_abort_handler      Cortex-R4/MPU/ARM   */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Scott Larson, Microsoft Corporation                                 */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function handles MPU exceptions and fills the                  */
/*    _txm_module_manager_memory_fault_info struct.                       */
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
/*    _txm_module_manager_memory_fault_handler                            */
/*    _tx_execution_thread_exit                                           */
/*    _tx_thread_schedule                                                 */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    MMU exceptions                                                      */
/*                                                                        */
/*  RELEASE HISTORY                                                       */
/*                                                                        */
/*    DATE              NAME                      DESCRIPTION             */
/*                                                                        */
/*  09-30-2020     Scott Larson             Initial Version 6.1           */
/*                                                                        */
/**************************************************************************/

/********************************************************************
  MMU Exception Handling
********************************************************************/
    .global  _tx_thread_system_state
    .global  _txm_module_manager_memory_fault_info
    .global  _tx_thread_current_ptr
    .global  _txm_module_manager_memory_fault_handler
    .global  _tx_thread_schedule

    #ifdef TX_ENABLE_EXECUTION_CHANGE_NOTIFY
    .global  _tx_execution_thread_exit
    #endif


    .arm	/* Exception handler in ARM mode.  */
    .align 	3
    .global	__tx_prefetch_handler
    .global __tx_abort_handler
	.type   __tx_prefetch_handler, "function"
    .type   __tx_abort_handler, "function"

__tx_prefetch_handler:
__tx_abort_handler:
    STMDB   sp!, {r0-r3}                    // Save some working registers
    LDR     r3, =_tx_thread_system_state    // Pickup address of system state var
    LDR     r2, [r3, #0]                    // Pickup system state
    ADD     r2, r2, #1                      // Increment the interrupt counter
    STR     r2, [r3, #0]                    // Store it back in the variable

    // Now pickup and store all the fault related information

    // Pickup the memory fault info struct
    LDR     r3, =_txm_module_manager_memory_fault_info
    LDR     r0, =_tx_thread_current_ptr     // Build current thread pointer address
    LDR     r1, [r0]                        // Pickup the current thread pointer
    STR     r1, [r3, #0]                    // Save current thread pointer

    MRC     p15, 0, r0, c6, c0, 0           // Read DFAR
    STR     r0, [r3, #8]                    // Save DFAR

    CMP     r0, #0                          // Was it a data or instruction fault?
    SUBEQ   lr, lr, #4                      // Adjust point of exception for instruction
    SUBNE   lr, lr, #8                      // Adjust point of exception for data
    STR     lr, [r3, #4]                    // Save point of fault

    MRC     p15, 0, r0, c5, c0, 0           // Read DFSR
    STR     r0, [r3, #12]                   // Save DFSR
    MRC     p15, 0, r0, c6, c0, 2           // Read IFAR
    STR     r0, [r3, #16]                   // Save IFAR
    MRC     p15, 0, r0, c5, c0, 1           // Read IFSR
    STR     r0, [r3, #20]                   // Save IFSR
    MOV     r0, #0                          // Build zero register
    MCR     p15, 0, r0, c6, c0, 0           // Clear DFAR
    MCR     p15, 0, r0, c5, c0, 0           // Clear DFSR
    MCR     p15, 0, r0, c6, c0, 2           // Clear IFAR
    MCR     p15, 0, r0, c5, c0, 1           // Clear IFSR

    // Save registers r0-r12
    POP     {r0-r2}
    STR     r0, [r3, #28]                   // Save r0
    STR     r1, [r3, #32]                   // Save r1
    STR     r2, [r3, #36]                   // Save r2
    POP     {r0}
    STR     r0, [r3, #40]                   // Save r3
    STR     r4, [r3, #44]                   // Save r4
    STR     r5, [r3, #48]                   // Save r5
    STR     r6, [r3, #52]                   // Save r6
    STR     r7, [r3, #56]                   // Save r7
    STR     r8, [r3, #60]                   // Save r8
    STR     r9, [r3, #64]                   // Save r9
    STR     r10,[r3, #68]                   // Save r10
    STR     r11,[r3, #72]                   // Save r11
    STR     r12,[r3, #76]                   // Save r12

    CPS     #SYS_MODE                       // Enter SYS mode
    MOV     r0, lr                          // Pickup lr
    MOV     r1, sp                          // Pickup sp
    CPS     #ABT_MODE                       // Back to ABT mode
    STR     r0, [r3, #80]                   // Save lr
    STR     r1, [r3, #24]                   // Save sp
    MRS     r0, SPSR                        // Pickup SPSR
    STR     r0, [r3, #84]                   // Save SPSR
    ORR     r0, r0, #SYS_MODE               // Return into SYS mode
    BIC     r0, r0, #THUMB_MASK             // Clear THUMB mode
    MSR     SPSR_c, r0                      // Save SPSR

    // Call memory manager fault handler
    BL      _txm_module_manager_memory_fault_handler

#ifdef TX_ENABLE_EXECUTION_CHANGE_NOTIFY
    /* Call the thread exit function to indicate the thread is no longer executing.  */
    BL      _tx_execution_thread_exit       // Call the thread exit function
#endif

    LDR     r0, =_tx_thread_system_state    // Pickup address of system state
    LDR     r1, [r0]                        // Pickup system state
    SUB     r1, r1, #1                      // Decrement
    STR     r1, [r0]                        // Store new system state

    MOV     r1, #0                          // Build NULL value
    LDR     r0, =_tx_thread_current_ptr     // Pickup address of current thread pointer
    STR     r1, [r0]                        // Clear current thread pointer

    // Return from exception
    LDR     lr, =_tx_thread_schedule        // Load scheduler address
    SUBS    pc, lr, #0                      // Return to scheduler
/********************************************************************
  End of MMU exception handling.
********************************************************************/

    /* Reference build options and version ID to ensure they come in.  */

    LDR     r2, =_tx_build_options              // Pickup build options variable address
    LDR     r0, [r2, #0]                        // Pickup build options content
    LDR     r2, =_tx_version_id                 // Pickup version ID variable address
    LDR     r0, [r2, #0]                        // Pickup version ID content
