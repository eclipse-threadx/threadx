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

#define TX_SOURCE_CODE

/* Include necessary system files.  */

#include "tx_api.h"
#include "tx_timer.h"
#ifdef TX_PORT_TRUSTZONE_NSC_ENABLE
 #include "tx_secure_interface.h"
#endif

#ifdef __ARM_ARCH_7EM__                // CM4
 #define TX_PORT_ISA_CBZ_SUPPORTED
 #define TX_PORT_ISA_STMDB_LDMIA_SUPPORTED
 #define TX_PORT_ISA_THUMB2_SUB_ADD_SUPPORTED
 #define TX_PORT_ISA_IT_SUPPORTED
#endif

#ifdef __ARM_ARCH_8M_MAIN__            // CM33
 #define TX_PORT_ISA_CBZ_SUPPORTED
 #define TX_PORT_ISA_STMDB_LDMIA_SUPPORTED
 #define TX_PORT_ISA_THUMB2_SUB_ADD_SUPPORTED
 #define TX_PORT_ISA_IT_SUPPORTED
#endif

#ifdef __ARM_ARCH_8M_BASE__            // CM23
 #define TX_PORT_ISA_CBZ_SUPPORTED
#endif

/* This must follow the definition for each core instruction set. IAR does not support CBZ in inline assembly. */
#ifdef __IAR_SYSTEMS_ICC__
 #undef TX_PORT_ISA_CBZ_SUPPORTED
#endif

/* Used to generate a compiler error (negative array size error) if the assertion fails.  This is used in place of "#error"
 * for expressions that cannot be evaluated by the preprocessor like offsetof(). */
#ifndef TX_PORT_COMPILE_TIME_ASSERT
 #define TX_PORT_COMPILE_TIME_ASSERT(e)    ((void) sizeof(char[1 - 2 * !(e)]))
#endif

/* Used to convert an evaluated macro into a string. */
#define TX_PORT_STRINGIFY_EXPANDED(s)      TX_PORT_STRINGIFY(s)

/* Used to convert text into a string. */
#define TX_PORT_STRINGIFY(s)               #s

/* The scheduler stacks r4-r11 (and LR if the MCU has an FPU) in PendSV_Handler, but PSPLIM will not detect a stack
 * overflow because PSP is not used in handler mode. TX_PORT_SCHEDULER_STACK is used to move PSPLIM to account for
 * this. TX_PORT_SCHEDULER_STACK must be a multiple of 8. */
#if __FPU_USED
#define TX_PORT_SCHEDULER_STACK    40
#else
#define TX_PORT_SCHEDULER_STACK    32
#endif

/* The following macros are defined as hard coded numbers so they can be converted to strings in the
 * assembly language of PendSV_Handler. They are verified to be correct using compile time assertions
 * at the beginning of PendSV_Handler. */

/* The following offsets are relative to the base of the TX_THREAD structure.  For example:
 * offsetof(TX_THREAD, tx_thread_run_count) == 4 */
#define TX_PORT_OFFSET_RUN_COUNT      4
#define TX_PORT_OFFSET_STACK_PTR      8
#define TX_PORT_OFFSET_STACK_START    12
#define TX_PORT_OFFSET_STACK_END      16
#define TX_PORT_OFFSET_TIME_SLICE     24

#if defined(__GNUC__) && !defined(__ARMCC_VERSION) && !defined(__clang__)
 #define TX_PORT_NAKED_FUNCTION       __attribute__((naked, no_instrument_function, \
                                                     no_profile_instrument_function))
#else
 #define TX_PORT_NAKED_FUNCTION       __attribute__((naked))
#endif

TX_PORT_NAKED_FUNCTION VOID PendSV_Handler(VOID);
TX_PORT_NAKED_FUNCTION VOID SVC_Handler(VOID);
VOID                        SysTick_Handler(VOID);

VOID _tx_thread_schedule(VOID);

VOID _tx_port_svc_handler(UINT * caller_stack_ptr);

extern TX_THREAD * volatile _tx_thread_current_ptr;
extern TX_THREAD * volatile _tx_thread_execute_ptr;
extern volatile UINT        _tx_thread_preempt_disable;

#ifdef TX_PORT_TRUSTZONE_NSC_ENABLE

/* offsetof() cannot be used directly in inline assembly.  offsetof() is required for the offset to
 * tx_thread_secure_stack_context because this offset may change if the thread extensions in the TX_THREAD
 * structure (for example, TX_THREAD_EXTENSION_0) are used. */

/* This cannot be static because the compiler is not aware it is reference in the assembly code. */
const uint32_t g_secure_stack_offset = offsetof(TX_THREAD, tx_thread_secure_stack_context);
#endif


/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _tx_thread_schedule                               Cortex-M/CMSIS    */
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
/*                                                                        */
/**************************************************************************/
VOID _tx_thread_schedule (VOID)
{
    /* The following compile time assertions validate offsets used in the assembly code
     * of PendSV_Handler.  These assertions cannot be in PendSV_Handler because only
     * assembly code is allowed in naked/stackless functions.  These statements generate
     * build errors if the offset of any of the structure members used in this code changes.
     * These structure offsets are hardcoded because offsetof() is not supported in inline
     * assembly. */
    TX_PORT_COMPILE_TIME_ASSERT(TX_PORT_OFFSET_RUN_COUNT == offsetof(TX_THREAD, tx_thread_run_count));
    TX_PORT_COMPILE_TIME_ASSERT(TX_PORT_OFFSET_STACK_PTR == offsetof(TX_THREAD, tx_thread_stack_ptr));
    TX_PORT_COMPILE_TIME_ASSERT(TX_PORT_OFFSET_STACK_START == offsetof(TX_THREAD, tx_thread_stack_start));
    TX_PORT_COMPILE_TIME_ASSERT(TX_PORT_OFFSET_TIME_SLICE == offsetof(TX_THREAD, tx_thread_time_slice));

    /* This function should only ever be called on Cortex-M from the first
     * schedule request. Subsequent scheduling occurs from the PendSV handling
     * routine below. */

    /* Clear the preempt-disable flag to enable rescheduling after initialization on Cortex-M targets.  */
    _tx_thread_preempt_disable = 0;

#if __FPU_USED

    /* Reset lazy stacking if the MCU has an FPU. */
    __asm volatile (
        "MRS     r12, CONTROL     \n"
        "BIC     r12, r12, #4     \n"  // Clear FPCA.
        "MSR     CONTROL, r12     \n"
        :                              // No outputs
        :                              // No inputs
        : "r12"                        // Clobbers r12
        );
#endif

    /* Enable interrupts */
#ifdef TX_PORT_USE_BASEPRI
    __set_BASEPRI(0U);
#else
    __enable_irq();
#endif

    /* Enter the scheduler for the first time.  */
    SCB->ICSR = SCB_ICSR_PENDSVSET_Msk;

    /* We should never get here - ever!  */
    while (1)
    {
#ifdef TX_ENABLE_BKPT
        __BKPT(0xEF);
#endif
#if defined(__ICCARM__)
 #ifdef TX_PORT_TRUSTZONE_NSC_ENABLE

        /* These functions are only called from inline assembly. They are
         * referenced here to avoid an IAR linker error in non-secure
         * TrustZone projects. */
        TZ_StoreContext_S(0);
        TZ_LoadContext_S(0);
 #endif
#endif
    }
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _tx_port_wait_thread_ready                        Cortex-M/CMSIS    */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    Waits for a new thread to become ready.                             */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    None                                                                */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    Pointer to thread ready to run                                      */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    None                                                                */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    PendSV_Handler                                                      */
/*                                                                        */
/**************************************************************************/

/* These variables are global because this function is called from PendSV_Handler, which is a stackless
 * function. */
__attribute((weak)) void * _tx_port_wait_thread_ready (VOID)
{
    /* The following is the idle wait processing. In this case, no threads are ready for execution and the
     * system will simply be idle until an interrupt occurs that makes a thread ready. Note that interrupts
     * are disabled to allow use of WFI for waiting for a thread to arrive.  */

    TX_THREAD * new_thread_ptr;

    while (1)
    {
        /* Disable interrupts - The next block is critical. Interrupts are disabled with PRIMASK event if
         * BASEPRI is used elsewhere because the WFI wake condition includes interrupts masked by PRIMASK,
         * but does not include interrupts masked by BASEPRI.
         * https://developer.arm.com/documentation/100235/0002/the-cortex-m33-instruction-set/miscellaneous-instructions/wfi?lang=en
         */
        __disable_irq();

        /* Make the new thread the current thread. */
        new_thread_ptr         = _tx_thread_execute_ptr;
        _tx_thread_current_ptr = new_thread_ptr;

        /* If non-NULL, a new thread is ready! */
        if (new_thread_ptr != 0)
        {
            /* At this point, we have a new thread ready to go.  */

            /* Clear any newly pended PendSV - since we are already in the handler!  */
            SCB->ICSR = SCB_ICSR_PENDSVCLR_Msk;

            /* Re-enable interrupts. */
            __enable_irq();
            break;
        }

        /**
         * DSB should be last instruction executed before WFI
         * infocenter.arm.com/help/index.jsp?topic=/com.arm.doc.dai0321a/BIHICBGB.html
         */
        __DSB();

        /* If there is a pending interrupt (wake up condition for WFI is true), the MCU does not enter low power mode:
         * http://infocenter.arm.com/help/index.jsp?topic=/com.arm.doc.dui0552a/BABHHGEB.html
         * After exiting from low power mode, interrupt must be re-enabled. */
        __WFI();

        /* Instruction Synchronization Barrier. */
        __ISB();

        /* Re-enable interrupts. */
        __enable_irq();
        __ISB();
    }

    return new_thread_ptr;
}

/* Generic context switching PendSV handler.  */
TX_PORT_NAKED_FUNCTION VOID PendSV_Handler (VOID)
{
    /* This function is assembly only to meet the requirements for naked/stackless functions. For GCC, only basic
     * assembly is allowed. The main stack is used when C functions are called. */

    /* Before entry into this exception handler, the hardware stacks XPSR, PC, LR, r12, r3, r2, r1, and r0 onto the
     * stack (typically the process stack of the executing thread). When the FPU is in use, the FPSCR and S0-S15
     * registers are also stored on the stack. All other registers are stored by software in this function. */

    /* Only r0-r3 and r12 can be used before stack preservation is complete. */

    /* r0-r3 are not guaranteed to retain their values after C functions are called. */

    /* An exception is granted in this case to allow commented out code.  Commented out equivalent C code is
     * included before each section of assembly code that has a C equivalent.  This is done to improve readability
     * and maintainability.  Commented out equivalent C code is in single line comments with no code on the
     * same line. */
    __asm volatile (
#if defined(__GNUC__)
        ".syntax unified                     \n"
#endif

#ifdef TX_ENABLE_EXECUTION_CHANGE_NOTIFY

        /* Call the thread exit function to indicate the thread is no longer executing.  */

        // _tx_execution_thread_exit();
        "CPSID   i                           \n" // Disable interrupts
        "PUSH    {r0, LR}                    \n" // Save LR since it has not been stored yet, and r0 to maintain double word stack aligment
        "BL      _tx_execution_thread_exit   \n" // Call the thread exit function

        /* r0-r3 are undefined after branches. */
        "POP     {r0, LR}                    \n" // Restore r0 and LR
        "CPSIE   i                           \n" // Enable interrupts
#endif

        /* Determine if there is a current thread to finish preserving.  */

        // if (_tx_thread_current_ptr != 0)
        // {
        "LDR     r0, =_tx_thread_current_ptr \n" // Build current thread pointer address
        "LDR     r1, [r0]                    \n" // Pickup current thread pointer
        "CMP     r1, #0                      \n"

        /* This branch is only taken the first time this function is called.  After that, there is always
         * a current thread to save. */
        "BEQ     __tx_no_current             \n"

        /* Recover PSP and preserve current thread context.  */
        "MRS     r3,  PSP                    \n" // Pickup PSP pointer (thread's stack pointer)

#if defined(TX_PORT_VENDOR_STACK_MONITOR_ENABLE)
 #ifdef TX_PORT_ISA_THUMB2_SUB_ADD_SUPPORTED
        "SUB     r3,  r3, #32                \n" // Update PSP to utilize the HW stack monitor with the amount of room required to save r4-r11
        "MSR     PSP, r3                     \n" // Update PSP to utilize the HW stack monitor
        "ADD     r3,  r3, #32                \n" // Restore original PSP to r3 so we can continue stacking
 #else
        "SUBS    r3,  r3, #32                \n" // Update PSP to utilize the HW stack monitor with the amount of room required to save r4-r11
        "MSR     PSP, r3                     \n" // Update PSP to utilize the HW stack monitor
        "ADDS    r3,  r3, #32                \n" // Restore original PSP to r3 so we can continue stacking
 #endif
#endif
#ifdef TX_PORT_ISA_STMDB_LDMIA_SUPPORTED
        "STMDB   r3!, {r4-r11}               \n" // Save its remaining registers
#else
        "SUBS    r3,  r3, #16                \n" // Allocate stack space
        "STM     r3!, {r4-r7}                \n" // Save its remaining registers (M33 Instruction: STMDB r3!, {r4-r11})
        "MOV     r4,  r8                     \n"
        "MOV     r5,  r9                     \n"
        "MOV     r6,  r10                    \n"
        "MOV     r7,  r11                    \n"
        "SUBS    r3,  r3, #32                \n" // Allocate stack space
        "STM     r3!, {r4-r7}                \n"
        "SUBS    r3,  r3, #16                \n" // Allocate stack space
#endif
#if __FPU_USED
        "TST     LR,  #0x10                  \n" // Determine if the VFP extended frame is present
        "BNE     _skip_vfp_save              \n" // No, skip additional VFP save
 #if defined(TX_PORT_VENDOR_STACK_MONITOR_ENABLE)
        "SUB     r3,  r3, #64                \n" // Calculate the amount of room required to save s16-s31
        "MSR     PSP, r3                     \n" // Update PSP to utilize the HW stack monitor
        "ADD     r3,  r3, #64                \n" // Restore original PSP to r3 so we can continue stacking
 #endif
        "VSTMDB  r3!, {s16-s31}              \n" // Yes, save additional VFP registers
        "_skip_vfp_save:                     \n"
        "STMDB   r3!, {LR}                   \n" // Save LR on the stack
#endif

        // [r1, #TX_PORT_OFFSET_STACK_PTR] == _tx_thread_current_ptr->tx_thread_stack_ptr
        "STR     r3, [r1, #"TX_PORT_STRINGIFY_EXPANDED (TX_PORT_OFFSET_STACK_PTR) /* Save its stack pointer */
        "]                                   \n"

#ifdef TX_PORT_TRUSTZONE_NSC_ENABLE

        /* Load the offset of tx_thread_secure_stack_context in r12. */
        "LDR     r12, =g_secure_stack_offset \n" // r12 = &g_secure_stack_offset
        "LDR     r12, [r12]                  \n" // r12 = g_secure_stack_offset

        // Save secure context
        "PUSH    {r0, r1}                    \n" // Save r1 (_tx_thread_current_ptr) and r0 to maintain double word stack alignment
        "LDR     r0, [r1, r12]               \n" // Load secure stack index
 #ifdef TX_PORT_ISA_CBZ_SUPPORTED
        "CBZ     r0, _skip_secure_save       \n" // Skip save if there is no secure context
 #else
        "CMP     r0, #0                      \n"
        "BEQ     _skip_secure_save           \n"
 #endif
        "BL      TZ_StoreContext_S           \n" // r0 must = secure stack index.
        "_skip_secure_save:                  \n"
        "POP     {r0, r1}                    \n" // Restore r0 and r1
#endif

        /* Stack preservation is complete. */

        /* Determine if time-slice is active. If it isn't, skip time handling processing.  */

        // if (_tx_timer_time_slice != 0)
        // {
        "LDR     r4, =_tx_timer_time_slice   \n" // Build address of time-slice variable

        "LDR     r3, [r4]                    \n" // Pickup current time-slice
#ifdef TX_PORT_ISA_CBZ_SUPPORTED
        "CBZ     r3, __tx_ts_new             \n" // If not active, skip processing
#else
        "CMP     r3, #0                      \n"
        "BEQ     __tx_ts_new                 \n"
#endif

        /* Time-slice is active, save the current thread's time-slice and clear the global time-slice variable.  */

        // _tx_thread_current_ptr->tx_thread_time_slice = _tx_timer_time_slice;
        "STR     r3, [r1, #"TX_PORT_STRINGIFY_EXPANDED (TX_PORT_OFFSET_TIME_SLICE) /* Save current time-slice */
        "]                                   \n"

        /* Clear the global time-slice.  */

        // _tx_timer_time_slice = 0;
        "MOVS    r3, #0                      \n" // Build clear value
        "STR     r3, [r4]                    \n" // Clear time-slice

        // }

        /* The executing thread is now completely preserved!!!  */

        // }

        /* Now we are looking for a new thread to execute!  */
        "__tx_ts_new:                        \n"

        /* Get the address of the new thread. */
        "LDR     r6, =_tx_thread_execute_ptr \n" // Build execute thread pointer address

        /* Is there a thread ready to execute?  */

        // if (_tx_thread_execute_ptr == 0)
        // {
        // _tx_port_wait_thread_ready();
        "LDR     r6, [r6]                    \n" // Is there another thread ready to execute?
#ifdef TX_PORT_ISA_CBZ_SUPPORTED
        "CBZ     r6, __tx_ts_wait            \n" // No, skip to the wait processing
#else
        "CMP     r6, #0                      \n"
        "BEQ     __tx_ts_wait                \n"
#endif

        // }
        // else
        // {

        /* Yes, another thread is ready for execution, make the current thread the new thread.  */

        // _tx_thread_current_ptr = _tx_thread_execute_ptr;

        "STR r6, [r0]                        \n" // Setup the current thread pointer to the new thread

        // }

        /* At this point, r6 must contain a pointer to the new thread. */

        "__tx_ts_restore:                    \n"

        /* Restore the thread.  */

        /* Increment the thread run count.  */

        // _tx_thread_current_ptr->tx_thread_run_count++;
        "LDR  r0, [r6, #"TX_PORT_STRINGIFY_EXPANDED (TX_PORT_OFFSET_RUN_COUNT)
        "]                                   \n" // Pickup the current thread run count
        "ADDS r0, r0, #1                     \n" // Increment the thread run count
        "STR  r0, [r6, #"TX_PORT_STRINGIFY_EXPANDED (TX_PORT_OFFSET_RUN_COUNT)
        "]                                   \n" // Store the new run count

        /* Setup global time-slice with thread's current time-slice.  */

        // _tx_timer_time_slice = _tx_thread_current_ptr->tx_thread_time_slice;
        "LDR r0, [r6, #"TX_PORT_STRINGIFY_EXPANDED (TX_PORT_OFFSET_TIME_SLICE)
        "]                                   \n" // Pickup thread's current time-slice
        "STR r0, [r4]                        \n" // Setup global time-slice

#ifdef TX_ENABLE_EXECUTION_CHANGE_NOTIFY

        /* Call the thread entry function to indicate the thread is executing.  */

        // _tx_execution_thread_enter();
        "CPSID   i                           \n" // Disable interrupts
        "BL  _tx_execution_thread_enter      \n" // Call the thread enter function
        "CPSIE   i                           \n" // Enable interrupts

        /* r0-r3 are undefined after branches. */
#endif

#ifdef TX_PORT_TRUSTZONE_NSC_ENABLE

        /* Load the offset of tx_thread_secure_stack_context in r12. */
        "LDR     r12, =g_secure_stack_offset \n" // r12 = &g_secure_stack_offset
        "LDR     r12, [r12]                  \n" // r12 = g_secure_stack_offset

        // Restore secure context
        // r0 = _tx_thread_current_ptr->tx_thread_secure_stack_context
        "LDR     r0, [r6, r12]               \n" // Load secure stack index
 #ifdef TX_PORT_ISA_CBZ_SUPPORTED
        "CBZ     r0, _skip_secure_restore    \n" // Skip restore if there is no secure context
 #else
        "CMP     r0, #0                      \n"
        "BEQ     _skip_secure_restore        \n"
 #endif
        "BL      TZ_LoadContext_S            \n" // Restore secure stack, secure context must be in r0
        "_skip_secure_restore:               \n"
#endif

#ifdef TX_PORT_VENDOR_STACK_MONITOR_ENABLE

        /* Allow vendors to disable custom stack monitor hardware and reconfigure for the new thread.
         * Vendors can find the new thread pointer in r6. */
        TX_PORT_VENDOR_ASM_STACK_MONITOR_CONFIGURE
#endif

#ifdef TX_PORT_PSPLIM_PRESENT

        /* Set PSPLIM to the beginning of the thread stack. */
        "LDR     r1, [r6, #"TX_PORT_STRINGIFY_EXPANDED (TX_PORT_OFFSET_STACK_START)
        "]                                   \n"
        "ADD     r1,  r1, #"TX_PORT_STRINGIFY_EXPANDED (TX_PORT_SCHEDULER_STACK)
        "                                    \n" // Update PSP to utilize the HW stack monitor with the amount of room required to save r4-r11
        "MSR     PSPLIM, r1                  \n"
#endif

        /* Restore the thread context and PSP.  */

        /* Get stack pointer for this thread. */

        // [r6, #TX_PORT_OFFSET_STACK_PTR] == _tx_thread_current_ptr->tx_thread_stack_ptr
        "LDR     r3, [r6, #"TX_PORT_STRINGIFY_EXPANDED (TX_PORT_OFFSET_STACK_PTR)
        "]                                   \n"

#if __FPU_USED

        /* Set LR (EXC_RETURN) to the value saved on the stack. LR is used to determine if FPU registers need
         * to be restored. */
        "LDMIA   r3!, {LR}                   \n"
#else

        /* Return to thread on process stack */
        "LDR r1, =0xFFFFFFFD                 \n"

        /* Set LR (EXC_RETURN) to 0xFFFFFFFD. Return to Thread mode, exception return uses non-floating-point state
         * from the PSP and execution uses PSP after return. */
        "MOV LR, r1                          \n"
#endif

#if __FPU_USED
        "TST     LR, #0x10                   \n" // Determine if the VFP extended frame is present
        "BNE     _skip_vfp_restore           \n" // If so, restore additional VFP registers and setup the proper exception return
        "VLDMIA  r3!, {s16-s31}              \n" // Yes, restore additional VFP registers
        "_skip_vfp_restore:                  \n"
#endif
#ifdef TX_PORT_ISA_STMDB_LDMIA_SUPPORTED
        "LDMIA   r3!, {r4-r11}               \n" // Recover thread's registers
#else
        "LDM     r3!, {r4-r7}                \n" // Recover thread's registers (M4 Instruction: LDMIA r3!, {r4-r11} )
        "MOV     r11, r7                     \n"
        "MOV     r10, r6                     \n"
        "MOV     r9,  r5                     \n"
        "MOV     r8,  r4                     \n"
        "LDM     r3!, {r4-r7}                \n"
#endif
        "MSR     PSP, r3                     \n"

#ifdef TX_PORT_VENDOR_STACK_MONITOR_ENABLE

        /* Allow vendors to enable custom stack monitor hardware. */
        TX_PORT_VENDOR_ASM_STACK_MONITOR_ENABLE
#endif

        /* Return to the thread.  */
        "BX      LR                          \n"

        /* We should never get here!  */
#ifdef TX_ENABLE_BKPT
        "BKPT    0                           \n"
#endif

        // while(1);
        "__tx_error:                         \n"
        "B       __tx_error                  \n"

        "__tx_no_current:                    \n"

        /* There is no current thread.  Load r4 with the value determined during thread preservation. */
        "LDR     r4, =_tx_timer_time_slice   \n" // Build address of time-slice variable

        "B __tx_ts_new                       \n"

        /* If no thread is ready to execute, wait until one becomes ready.  */
        "__tx_ts_wait:                       \n"
        "BL      _tx_port_wait_thread_ready  \n"

        /* r0 contains the base address of the thread ready to execute. Move it to r6 so the value
         * is persistent through function calls. */
        "MOV     r6, r0                      \n"

        "B       __tx_ts_restore             \n" // Restore the thread

        );
}

/* Moved from tx_timer_interrupt.c to ensure this code is not removed by the linker. */
#ifndef TX_NO_TIMER
extern VOID _tx_timer_interrupt(VOID);

/* System tick timer. */
VOID SysTick_Handler (VOID)
{
    _tx_timer_interrupt();
}

#endif

#ifdef TX_PORT_TRUSTZONE_NSC_ENABLE

/* SVC_Handler is not needed when ThreadX is running in single mode. In TrustZone projects, this
 * is used to allocate or free secure stack space. */
void SVC_Handler (void)
{
    /* This is a naked/stackless function. Do not pass arguments to the inline assembly when the GCC compiler is
     * used. */
    __asm volatile (
 #if defined(__GNUC__)
        ".syntax unified                         \n"
 #endif

        /* Save stack pointer in r0, then call _tx_port_svc_handler.
         *
         * Bit 2 of EXC_RETURN is SPSEL: Indicates which stack contains the exception stack frame.
         *   0 = MSP
         *   1 = PSP
         */

 #ifdef TX_PORT_ISA_IT_SUPPORTED
        "TST     LR, #4                          \n" // Determine return stack from EXC_RETURN bit 2
        "ITE     EQ                              \n"
        "MRSEQ   r0, MSP                         \n" // Get MSP if return stack is MSP
        "MRSNE   r0, PSP                         \n" // Get PSP if return stack is PSP
        "LDR     r1, =_tx_port_svc_handler       \n"
        "BX      r1                              \n" // Branch to _tx_port_svc_handler, with exception stack pointer in r0.
 #else
        "LDR     r2, =_tx_port_svc_handler       \n"
        "MOVS    r0, #4                          \n"
        "MOV     r1, LR                          \n"
        "TST     r0, r1                          \n" // Determine return stack from EXC_RETURN bit 2
        "BEQ _stacking_used_msp                  \n" // Get MSP if return stack is MSP
        "MRS     r0, PSP                         \n" // Get PSP if return stack is PSP
        "BX r2                                   \n" // Branch to _tx_port_svc_handler, with exception stack pointer in r0.
        "_stacking_used_msp:                     \n"
        "MRS     r0, MSP                         \n"
        "BX r2                                   \n" // Branch to _tx_port_svc_handler, with exception stack pointer in r0.
 #endif
        );
}

/* SVC processing is done in C here. */
void _tx_port_svc_handler (UINT * caller_stack_ptr)
{
    UINT  stacked_pc;
    UINT  stacked_r0;
    UCHAR svc_number;

    /* Load saved PC from stack. */
    stacked_pc = caller_stack_ptr[6];

    /* Load SVC number. */
    svc_number = ((UCHAR *) stacked_pc)[-2];

    switch (svc_number)
    {
        case TX_SVC_NUM_SECURE_INIT:
        {
            /* Initialize secure stacks for threads calling secure functions. */
            TZ_InitContextSystem_S();

            break;
        }

        /* Is it a secure stack allocate request? */
        case TX_SVC_NUM_SECURE_ALLOC:
        {
            /* r0 contains a pointer to the thread control block. */
            stacked_r0 = caller_stack_ptr[0];
            TX_THREAD * p_thread = (TX_THREAD *) stacked_r0;

            /* Allocate a secure. The input variable is unused. */
            p_thread->tx_thread_secure_stack_context = TZ_AllocModuleContext_S(0U);

            /* Convert CMSIS error code to ThreadX error code. */
            if (0U == p_thread->tx_thread_secure_stack_context)
            {
                caller_stack_ptr[0] = TX_NO_MEMORY;
            }
            else
            {
                caller_stack_ptr[0] = TX_SUCCESS;
            }

            /* If the context is being allocated for the current thread, load the context. */
            if (p_thread == _tx_thread_current_ptr)
            {
                TZ_LoadContext_S(p_thread->tx_thread_secure_stack_context);
            }

            break;
        }

        case TX_SVC_NUM_SECURE_FREE:
        {
            /* r0 contains a pointer to the thread control block. */
            stacked_r0 = caller_stack_ptr[0];
            TX_THREAD * p_thread = (TX_THREAD *) stacked_r0;

            /* Free the secure context. */
            uint32_t tz_return_code = TZ_FreeModuleContext_S(p_thread->tx_thread_secure_stack_context);

            /* Convert CMSIS error code to ThreadX error code. */
            if (0U == tz_return_code)
            {
                caller_stack_ptr[0] = TX_THREAD_ERROR;
            }
            else
            {
                caller_stack_ptr[0] = TX_SUCCESS;
            }

            break;
        }

        default:
        {
            /* Unknown SVC argument - just return. */
        }
    }
}


/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _tx_thread_secure_stack_initialize                Cortex-M/CMSIS    */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function initializes the TrustZone secure module.              */
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
/*    SVC 3                                                               */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    _tx_initialize_kernel_enter                                         */
/*                                                                        */
/**************************************************************************/
void _tx_thread_secure_stack_initialize (void)
{
    __asm volatile (
        TX_ENABLE_ASM                  // Enable interrupts for SVC call
        "SVC     %[svc_num]                  \n"
        TX_RESTORE_ASM                 // Restore interrupt state
        ::[svc_num] "i" (TX_SVC_NUM_SECURE_INIT) : "memory");
}

#endif
