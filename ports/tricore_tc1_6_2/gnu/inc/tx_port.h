/***************************************************************************
 * Copyright (c) 2024 Microsoft Corporation
 * Copyright (c) 2026 safecubed GmbH
 * Copyright (c) 2026 Eclipse ThreadX contributors
 *
 * This program and the accompanying materials are made available under the
 * terms of the MIT License which is available at
 * https://opensource.org/licenses/MIT.
 *
 * SPDX-License-Identifier: MIT
 **************************************************************************/

/* Portions of this file were generated with AI assistance. */


/**************************************************************************/
/**************************************************************************/
/**                                                                       */
/** ThreadX Component                                                     */
/**                                                                       */
/**   Port Specific                                                       */
/**                                                                       */
/**************************************************************************/
/**************************************************************************/


/**************************************************************************/
/*                                                                        */
/*  PORT SPECIFIC C INFORMATION                            RELEASE        */
/*                                                                        */
/*    tx_port.h                                           TriCore/GNU     */
/*                                                           6.5.2        */
/*                                                                        */
/*  AUTHOR                                                                */
/*                                                                        */
/*    William E. Lamie, Microsoft Corporation                             */
/*    Andreas Neubauer, safecubed GmbH                                    */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This file contains data type definitions that make the ThreadX      */
/*    real-time kernel function identically on a variety of different     */
/*    processor architectures.  For example, the size or number of bits   */
/*    in an "int" data type vary between microprocessor architectures and */
/*    even C compilers for the same microprocessor.  ThreadX does not     */
/*    directly use native C data types.  Instead, ThreadX creates its     */
/*    own special types that can be mapped to actual data types by this   */
/*    file to guarantee consistency in the interface and functionality.   */
/*                                                                        */
/*  RELEASE HISTORY                                                       */
/*                                                                        */
/*    DATE              NAME                      DESCRIPTION             */
/*                                                                        */
/*  09-30-2020     William E. Lamie         Initial Version 6.1           */
/*  10-15-2021     William E. Lamie         Modified comment(s), added    */
/*                                            symbol ULONG64_DEFINED,     */
/*                                            resulting in version 6.1.9  */
/*  04-25-2022     William E. Lamie         Modified comment(s), removed  */
/*                                            useless definition,         */
/*                                            resulting in version 6.1.11 */
/*  10-31-2023     Yanwu Cai                Modified comment(s), fixed    */
/*                                            compile warnings,           */
/*                                            resulting in version 6.3.0  */
/*  11-04-2026     Andreas Neubauer         Adapted for TriCore,          */
/*                                            resulting in version 6.5.2  */
/*                                                                        */
/**************************************************************************/

#ifndef TX_PORT_H
#define TX_PORT_H



/* #define TX_MISRA_ENABLE  */


/* #define TX_INLINE_INITIALIZATION */

/* #define TX_NOT_INTERRUPTABLE  */
/* #define TX_TIMER_PROCESS_IN_ISR */
/* #define TX_REACTIVATE_INLINE */
/* #define TX_DISABLE_STACK_FILLING */
/* #define TX_ENABLE_STACK_CHECKING */
/* #define TX_DISABLE_PREEMPTION_THRESHOLD */
/* #define TX_DISABLE_REDUNDANT_CLEARING */
/* #define TX_DISABLE_NOTIFY_CALLBACKS */
/* #define TX_INLINE_THREAD_RESUME_SUSPEND */
/* #define TX_ENABLE_EVENT_TRACE */


/* For MISRA, define enable performance info. Also, for MISRA TX_DISABLE_NOTIFY_CALLBACKS should not be defined.  */


/* #define TX_BLOCK_POOL_ENABLE_PERFORMANCE_INFO
#define TX_BYTE_POOL_ENABLE_PERFORMANCE_INFO
#define TX_EVENT_FLAGS_ENABLE_PERFORMANCE_INFO
#define TX_MUTEX_ENABLE_PERFORMANCE_INFO
#define TX_QUEUE_ENABLE_PERFORMANCE_INFO
#define TX_SEMAPHORE_ENABLE_PERFORMANCE_INFO
#define TX_THREAD_ENABLE_PERFORMANCE_INFO
#define TX_TIMER_ENABLE_PERFORMANCE_INFO */



/* Determine if the optional ThreadX user define file should be used.  */

#ifdef TX_INCLUDE_USER_DEFINE_FILE


/* Yes, include the user defines in tx_user.h. The defines in this file may
   alternately be defined on the command line.  */

#include "tx_user.h"
#endif


/* Define compiler library include files.  */

#include <stdlib.h>
#include <string.h>
#include <stdint.h>

/* TriCore intrinsics of the AURIX GCC. */
#include <machine/intrinsics.h>




/* Define ThreadX basic types for this port.  */

#define VOID                                    void
typedef char                                    CHAR;
typedef unsigned char                           UCHAR;
typedef int                                     INT;
typedef unsigned int                            UINT;
typedef long                                    LONG;
typedef unsigned long                           ULONG;
typedef unsigned long long                      ULONG64;
typedef short                                   SHORT;
typedef unsigned short                          USHORT;
#define ULONG64_DEFINED


/* Define automated coverage test extensions for the ThreadX regression test.  */

#ifndef __ASSEMBLER__

typedef unsigned int    TEST_FLAG;
extern TEST_FLAG        threadx_byte_allocate_loop_test;
extern TEST_FLAG        threadx_byte_release_loop_test;
extern TEST_FLAG        threadx_mutex_suspension_put_test;
extern TEST_FLAG        threadx_mutex_suspension_priority_test;
#ifndef TX_TIMER_PROCESS_IN_ISR
extern TEST_FLAG        threadx_delete_timer_thread;
#endif

extern void             abort_and_resume_byte_allocating_thread(void);
extern void             abort_all_threads_suspended_on_mutex(void);
extern void             suspend_lowest_priority(void);
#ifndef TX_TIMER_PROCESS_IN_ISR
extern void             delete_timer_thread(void);
#endif
extern TEST_FLAG        test_stack_analyze_flag;
extern TEST_FLAG        test_initialize_flag;
extern TEST_FLAG        test_forced_mutex_timeout;


#ifdef TX_REGRESSION_TEST

#define TX_BYTE_ALLOCATE_EXTENSION              if (threadx_byte_allocate_loop_test == ((TEST_FLAG) 1))         \
                                                {                                                               \
                                                    pool_ptr -> tx_byte_pool_owner =  TX_NULL;                  \
                                                    threadx_byte_allocate_loop_test = ((TEST_FLAG) 0);          \
                                                }

#define TX_BYTE_RELEASE_EXTENSION               if (threadx_byte_release_loop_test == ((TEST_FLAG) 1))          \
                                                {                                                               \
                                                    threadx_byte_release_loop_test = ((TEST_FLAG) 0);           \
                                                    abort_and_resume_byte_allocating_thread();                  \
                                                }

#define TX_MUTEX_PUT_EXTENSION_1                if (threadx_mutex_suspension_put_test == ((TEST_FLAG) 1))       \
                                                {                                                               \
                                                    threadx_mutex_suspension_put_test = ((TEST_FLAG) 0);        \
                                                    abort_all_threads_suspended_on_mutex();                     \
                                                }

#define TX_MUTEX_PUT_EXTENSION_2                if (test_forced_mutex_timeout == ((TEST_FLAG) 1))               \
                                                {                                                               \
                                                    test_forced_mutex_timeout = ((TEST_FLAG) 0);                \
                                                    _tx_thread_wait_abort(mutex_ptr -> tx_mutex_suspension_list); \
                                                }

#define TX_MUTEX_PRIORITY_CHANGE_EXTENSION      if (threadx_mutex_suspension_priority_test == ((TEST_FLAG) 1))  \
                                                {                                                               \
                                                    threadx_mutex_suspension_priority_test = ((TEST_FLAG) 0);   \
                                                    suspend_lowest_priority();                                  \
                                                }

#ifndef TX_TIMER_PROCESS_IN_ISR

#define TX_TIMER_INITIALIZE_EXTENSION(a)        if (threadx_delete_timer_thread == ((TEST_FLAG) 1))             \
                                                {                                                               \
                                                    threadx_delete_timer_thread = ((TEST_FLAG) 0);              \
                                                    delete_timer_thread();                                      \
                                                    (a) =  ((UINT) 1);                                          \
                                                }

#endif

#define TX_THREAD_STACK_ANALYZE_EXTENSION       if (test_stack_analyze_flag == ((TEST_FLAG) 1))                 \
                                                {                                                               \
                                                    thread_ptr -> tx_thread_id =  ((TEST_FLAG) 0);              \
                                                    test_stack_analyze_flag =     ((TEST_FLAG) 0);              \
                                                }                                                               \
                                                else if (test_stack_analyze_flag == ((TEST_FLAG) 2))            \
                                                {                                                               \
                                                    stack_ptr =  thread_ptr -> tx_thread_stack_start;           \
                                                    test_stack_analyze_flag =     ((TEST_FLAG) 0);              \
                                                }                                                               \
                                                else if (test_stack_analyze_flag == ((TEST_FLAG) 3))            \
                                                {                                                               \
                                                    *stack_ptr =  TX_STACK_FILL;                                \
                                                    test_stack_analyze_flag =     ((TEST_FLAG) 0);              \
                                                }                                                               \
                                                else                                                            \
                                                {                                                               \
                                                    test_stack_analyze_flag =     ((TEST_FLAG) 0);              \
                                                }

#define TX_INITIALIZE_KERNEL_ENTER_EXTENSION    if (test_initialize_flag == ((TEST_FLAG) 1))                    \
                                                {                                                               \
                                                    test_initialize_flag =  ((TEST_FLAG) 0);                    \
                                                    return;                                                     \
                                                }

#endif /* TX_REGRESSION_TEST */

#endif /* __ASSEMBLER__ */




/* TriCore core registers used by the inline functions of this file. The
   definitions shared only by the port sources are in src/tx_port_tricore.h.  */

#define TX_PORT_TRICORE_CSFR_PSW                         0xFE04
#define TX_PORT_TRICORE_CSFR_ICR                         0xFE2C

#define TX_PORT_TRICORE_PSW_IS_OFF                       9             /* 1: interrupt stack in use */
#define TX_PORT_TRICORE_ICR_CCPN_MSK                     0x000000FFUL  /* Current CPU priority number */
#define TX_PORT_TRICORE_ICR_IE_MSK                       0x00008000UL  /* Global interrupt enable */

/* System call that enters the scheduler.  */
#define TX_PORT_TRICORE_SYSCALL_ID_SCHEDULE              0

/* Receives every other system call number. The weak default stops the CPU.  */
VOID    tx_port_tricore_syscall_hook(UINT id);


/* Interrupt handlers of CPU0.

   TX_PORT_TRICORE_INTERRUPT(isr, priority) starts the definition of the
   interrupt handler isr, for example:

       TX_PORT_TRICORE_INTERRUPT(timer_isr, 2)
       {
           ...
       }

   It places the vector table entry in the section .intvec_tc0_<priority>.
   The linker script shall locate this section at BIV + 32 * priority. The
   entry saves the lower context and jumps to isr, which returns through
   RSLCX and RFE. priority shall expand to a decimal integer literal without
   parentheses or suffix, because it becomes part of the section name.  */

#define TX_PORT_TRICORE_STRINGIFY(x)                     #x
#define TX_PORT_TRICORE_EXPAND_STRINGIFY(x)              TX_PORT_TRICORE_STRINGIFY(x)

#define TX_PORT_TRICORE_INTERRUPT(isr, priority)                                                    \
    __asm__ (".pushsection .intvec_tc0_" TX_PORT_TRICORE_EXPAND_STRINGIFY(priority)                 \
             ", \"ax\", @progbits\n"                                                                \
             "    svlcx\n"                                                                          \
             "    movh.a  %a14, hi:" #isr "\n"                                                      \
             "    lea     %a14, [%a14]lo:" #isr "\n"                                                \
             "    ji      %a14\n"                                                                   \
             ".popsection\n");                                                                      \
    void isr(void) __attribute__ ((interrupt_handler));                                             \
    void isr(void)


/* The dispatch interrupt switches threads on behalf of interrupt handlers.
   Its priority shall be below every interrupt that calls ThreadX services,
   including the timer interrupt.  */
#ifndef TX_PORT_TRICORE_DISPATCH_PRIORITY
#define TX_PORT_TRICORE_DISPATCH_PRIORITY                1
#endif

/* Address of the service request control register (SRC) of the dispatch
   interrupt. The default is SRC_GPSR00, general purpose service request 0
   of group 0, of the TC3xx. The board shall configure this SRC for CPU0
   with TX_PORT_TRICORE_DISPATCH_PRIORITY and enable it.  */
#ifndef TX_PORT_TRICORE_DISPATCH_SRC_ADDRESS
#define TX_PORT_TRICORE_DISPATCH_SRC_ADDRESS             0xF0038990UL
#endif

#define TX_PORT_TRICORE_DISPATCH_SRC                     (*(volatile unsigned int *) TX_PORT_TRICORE_DISPATCH_SRC_ADDRESS)

/* Fields of an SRC.  */
#define TX_PORT_TRICORE_SRC_SRPN_MSK                     0x000000FFUL  /* Service request priority number */
#define TX_PORT_TRICORE_SRC_SRE_MSK                      0x00000400UL  /* Service request enable */
#define TX_PORT_TRICORE_SRC_TOS_OFF                      11            /* Type of service, 0: CPU0 */
#define TX_PORT_TRICORE_SRC_CLRR_MSK                     0x02000000UL  /* Request clear */
#define TX_PORT_TRICORE_SRC_SETR_MSK                     0x04000000UL  /* Request set */

#ifndef TX_PORT_TRICORE_TIMER_INTERRUPT_PRIORITY
#define TX_PORT_TRICORE_TIMER_INTERRUPT_PRIORITY         2
#endif

/* Lower 32 bits of the system timer STM0 of the TC3xx: the default time
   source of the trace and of the execution profile.  */
#define TX_PORT_TRICORE_STM0_TIM0                        (*(volatile unsigned int *) 0xF0001010UL)


/* Define the TX_MEMSET macro to remove library reference. The local names
   carry the prefix tx_memset_ so that they do not hide an argument of the
   same name.  */

#ifndef TX_MISRA_ENABLE
#define TX_MEMSET(a,b,c)                        do                                                      \
                                                {                                                       \
                                                    UCHAR *tx_memset_ptr;                               \
                                                    UCHAR  tx_memset_value;                             \
                                                    UINT   tx_memset_index;                             \
                                                    UINT   tx_memset_size;                              \
                                                    tx_memset_ptr =    (UCHAR *) ((VOID *) (a));        \
                                                    tx_memset_value =  (UCHAR) (b);                     \
                                                    tx_memset_size =   (UINT) (c);                      \
                                                    for (tx_memset_index = 0U; tx_memset_index < tx_memset_size; tx_memset_index++) \
                                                    {                                                   \
                                                        tx_memset_ptr[tx_memset_index] =  tx_memset_value; \
                                                    }                                                   \
                                                } while (0)
#endif


/* Define the priority levels for ThreadX.  Legal values range
   from 32 to 1024 and MUST be evenly divisible by 32.  */

#ifndef TX_MAX_PRIORITIES
#define TX_MAX_PRIORITIES                       32
#endif


/* Define the minimum stack for a ThreadX thread on this processor. If the size supplied during
   thread creation is less than this value, the thread create call will return an error.  */

#ifndef TX_MINIMUM_STACK
#define TX_MINIMUM_STACK                        200         /* Minimum stack size for this port */
#endif


/* Define the system timer thread's default stack size and priority.  These are only applicable
   if TX_TIMER_PROCESS_IN_ISR is not defined.  */

#ifndef TX_TIMER_THREAD_STACK_SIZE
#define TX_TIMER_THREAD_STACK_SIZE              1024        /* Default timer thread stack size  */
#endif

#ifndef TX_TIMER_THREAD_PRIORITY
#define TX_TIMER_THREAD_PRIORITY                0           /* Default timer thread priority    */
#endif


/* By default, TX_DISABLE masks all interrupts by clearing ICR.IE. If
   TX_PORT_USE_CCPN is defined, TX_DISABLE raises ICR.CCPN to TX_PORT_CCPN
   instead, and interrupts with a priority above TX_PORT_CCPN stay enabled.
   Such interrupts shall not call ThreadX services. TX_DISABLE never lowers
   ICR.CCPN. Both symbols can be defined in tx_user.h.  */

/* #define TX_PORT_USE_CCPN */

#ifndef TX_PORT_CCPN
#define TX_PORT_CCPN                            31
#endif

/* Define various constants for the ThreadX  port.  */

/* The interrupt posture of tx_interrupt_control is ICR.IE.  */
#define TX_INT_DISABLE                          0
#define TX_INT_ENABLE                           TX_PORT_TRICORE_ICR_IE_MSK


#ifndef TX_TIMER_TICKS_PER_SECOND
#define TX_TIMER_TICKS_PER_SECOND               (1000UL)
#endif


/* Define the clock source for trace event entry time stamp. The following two item are port specific.
   For example, if the time source is at the address 0x0a800024 and is 16-bits in size, the clock
   source constants would be:

#define TX_TRACE_TIME_SOURCE                    *((ULONG *) 0x0a800024)
#define TX_TRACE_TIME_MASK                      0x0000FFFFUL

*/

#ifndef TX_MISRA_ENABLE
#ifndef TX_TRACE_TIME_SOURCE
#define TX_TRACE_TIME_SOURCE                    TX_PORT_TRICORE_STM0_TIM0
#endif
#else
#ifndef TX_TRACE_TIME_SOURCE
ULONG   _tx_misra_time_stamp_get(VOID);
#define TX_TRACE_TIME_SOURCE                    _tx_misra_time_stamp_get()
#endif
#endif

/* Time source of the execution profile kit.  */
#ifndef TX_EXECUTION_TIME_SOURCE
#define TX_EXECUTION_TIME_SOURCE                (EXECUTION_TIME_SOURCE_TYPE) TX_PORT_TRICORE_STM0_TIM0
#endif

#ifndef TX_TRACE_TIME_MASK
#define TX_TRACE_TIME_MASK                      0xFFFFFFFFUL
#endif



/* Define the port specific options for the _tx_build_options variable. This variable indicates
   how the ThreadX library was built.  */

#define TX_PORT_SPECIFIC_BUILD_OPTIONS          (0)


/* Define the in-line initialization constant so that modules with in-line
   initialization capabilities can prevent their initialization from being
   a function call.  */

#ifdef TX_MISRA_ENABLE
#define TX_DISABLE_INLINE
#else
#define TX_INLINE_INITIALIZATION
#endif


/* Determine whether or not stack checking is enabled. By default, ThreadX stack checking is
   disabled. When the following is defined, ThreadX thread stack checking is enabled.  If stack
   checking is enabled (TX_ENABLE_STACK_CHECKING is defined), the TX_DISABLE_STACK_FILLING
   define is negated, thereby forcing the stack fill which is necessary for the stack checking
   logic.  */

#ifndef TX_MISRA_ENABLE
#ifdef TX_ENABLE_STACK_CHECKING
#undef TX_DISABLE_STACK_FILLING
#endif
#endif


/* Define the TX_THREAD control block extensions for this port. The main reason
   for the multiple macros is so that backward compatibility can be maintained with
   existing ThreadX kernel awareness modules.  */

#define TX_THREAD_EXTENSION_0                  UINT tx_thread_csa;
#define TX_THREAD_EXTENSION_1
#define TX_THREAD_EXTENSION_2
#define TX_THREAD_EXTENSION_3



/* Define the port extensions of the remaining ThreadX objects.  */

#define TX_BLOCK_POOL_EXTENSION
#define TX_BYTE_POOL_EXTENSION
#define TX_EVENT_FLAGS_GROUP_EXTENSION
#define TX_MUTEX_EXTENSION
#define TX_QUEUE_EXTENSION
#define TX_SEMAPHORE_EXTENSION
#define TX_TIMER_EXTENSION


/* Define the user extension field of the thread control block.  Nothing
   additional is needed for this port so it is defined as white space.  */

#ifndef TX_THREAD_USER_EXTENSION
#define TX_THREAD_USER_EXTENSION
#endif


/* Define the macros for processing extensions in tx_thread_create, tx_thread_delete,
   tx_thread_shell_entry, and tx_thread_terminate.  */


#define TX_THREAD_CREATE_EXTENSION(thread_ptr)
struct  TX_THREAD_STRUCT;
VOID    _tx_thread_csa_release(struct TX_THREAD_STRUCT *thread_ptr);
#define TX_THREAD_DELETE_EXTENSION(thread_ptr)      _tx_thread_csa_release(thread_ptr);
#define TX_THREAD_COMPLETED_EXTENSION(thread_ptr)
#define TX_THREAD_TERMINATED_EXTENSION(thread_ptr)


/* Define the ThreadX object creation extensions for the remaining objects.  */

#define TX_BLOCK_POOL_CREATE_EXTENSION(pool_ptr)
#define TX_BYTE_POOL_CREATE_EXTENSION(pool_ptr)
#define TX_EVENT_FLAGS_GROUP_CREATE_EXTENSION(group_ptr)
#define TX_MUTEX_CREATE_EXTENSION(mutex_ptr)
#define TX_QUEUE_CREATE_EXTENSION(queue_ptr)
#define TX_SEMAPHORE_CREATE_EXTENSION(semaphore_ptr)
#define TX_TIMER_CREATE_EXTENSION(timer_ptr)


/* Define the ThreadX object deletion extensions for the remaining objects.  */

#define TX_BLOCK_POOL_DELETE_EXTENSION(pool_ptr)
#define TX_BYTE_POOL_DELETE_EXTENSION(pool_ptr)
#define TX_EVENT_FLAGS_GROUP_DELETE_EXTENSION(group_ptr)
#define TX_MUTEX_DELETE_EXTENSION(mutex_ptr)
#define TX_QUEUE_DELETE_EXTENSION(queue_ptr)
#define TX_SEMAPHORE_DELETE_EXTENSION(semaphore_ptr)
#define TX_TIMER_DELETE_EXTENSION(timer_ptr)


/* Returns PSW.IS: 1 in interrupt and trap handlers, 0 in threads.  */

__attribute__( ( always_inline ) ) static inline unsigned int __get_interrupt_state(void)
{
    UINT psw;
    psw = __mfcr(TX_PORT_TRICORE_CSFR_PSW);
    _isync();
    return (psw >> TX_PORT_TRICORE_PSW_IS_OFF) & 1U;
}

#ifndef TX_THREAD_GET_SYSTEM_STATE
#ifndef TX_MISRA_ENABLE
#define TX_THREAD_GET_SYSTEM_STATE()        (_tx_thread_system_state | __get_interrupt_state())


#else   /* TX_MISRA_ENABLE is defined, use MISRA function. */
ULONG   _tx_misra_interrupt_state_get(VOID);
#define TX_THREAD_GET_SYSTEM_STATE()        (_tx_thread_system_state | _tx_misra_interrupt_state_get())
#endif  /* TX_MISRA_ENABLE */
#endif  /* TX_THREAD_GET_SYSTEM_STATE */


/* Define the check for whether or not to call the _tx_thread_system_return function.  A non-zero value
   indicates that _tx_thread_system_return should not be called. This overrides the definition in tx_thread.h:
   _tx_thread_system_return checks the interrupt state itself (PSW.IS), so only the preemption state is
   checked here. */

#ifndef TX_THREAD_SYSTEM_RETURN_CHECK
#define TX_THREAD_SYSTEM_RETURN_CHECK(c)    (c) = ((ULONG) _tx_thread_preempt_disable);
#endif


/* The switch takes place once no higher-priority interrupt is active.  */

__attribute__( ( always_inline ) ) static inline void _tx_thread_dispatch_request(void)
{
    TX_PORT_TRICORE_DISPATCH_SRC |= (unsigned int) TX_PORT_TRICORE_SRC_SETR_MSK;
    _dsync();
}

/* PSW.IS is 1 after reset, and start-up code may keep it. The initialisation runs on the user stack, so
   _tx_initialize_kernel_enter clears PSW.IS: traps and interrupts then switch to the interrupt stack, and
   TX_THREAD_GET_SYSTEM_STATE does not take the initialisation for an interrupt handler. The clear shall be
   inline code of _tx_initialize_kernel_enter, which does not return: the return from a called function
   would restore the PSW of its caller.  */
#define TX_PORT_SPECIFIC_PRE_INITIALIZATION     __mtcr(TX_PORT_TRICORE_CSFR_PSW, __mfcr(TX_PORT_TRICORE_CSFR_PSW) & ~(1UL << TX_PORT_TRICORE_PSW_IS_OFF)); \
                                                _isync();

/* Define the macro to ensure _tx_thread_preempt_disable is set early in initialization in order to
   prevent early scheduling. _tx_thread_schedule clears it before the first thread runs.  */
#define TX_PORT_SPECIFIC_POST_INITIALIZATION    _tx_thread_preempt_disable++;




/* Define the interrupt disable/restore functions. */

__attribute__( ( always_inline ) ) static inline unsigned int __get_interrupt_posture(void)
{
    UINT posture;
    posture = __mfcr(TX_PORT_TRICORE_CSFR_ICR);
    _isync();
    return posture;
}


__attribute__( ( always_inline ) ) static inline void __set_ccpn_value(unsigned int ccpn_value)
{
    UINT icr;
    icr = __mfcr(TX_PORT_TRICORE_CSFR_ICR);
    icr = (icr & ~((UINT) TX_PORT_TRICORE_ICR_CCPN_MSK)) | (ccpn_value & ((UINT) TX_PORT_TRICORE_ICR_CCPN_MSK));
    __mtcr(TX_PORT_TRICORE_CSFR_ICR, icr);
    _isync();
}

__attribute__( ( always_inline ) ) static inline void __enable_interrupts(void)
{
    _enable();
}

__attribute__( ( always_inline ) ) static inline void __restore_interrupt(unsigned int posture)
{
    __mtcr(TX_PORT_TRICORE_CSFR_ICR, posture);
    _isync();
}

__attribute__( ( always_inline ) ) static inline unsigned int __disable_interrupts(void)
{
    UINT posture;

    posture = __get_interrupt_posture();

#ifdef TX_PORT_USE_CCPN
    /* TX_DISABLE shall not lower CCPN.  */
    if ((posture & TX_PORT_TRICORE_ICR_CCPN_MSK) < TX_PORT_CCPN)
    {
        __set_ccpn_value(TX_PORT_CCPN);
    }
#else
    _disable();
#endif

    return posture;
}


/* tx_thread_system_return.c calls this function when TX_DISABLE_INLINE is
   defined.  */
__attribute__( ( always_inline ) ) static inline void _tx_thread_system_return_inline(void)
{
    unsigned int interrupt_save;

    /* Thread code switches through the system call, interrupt and trap
       handlers (PSW.IS = 1) through the dispatch interrupt.  */
    if (__get_interrupt_state() == 0U)
    {
        interrupt_save = __get_interrupt_posture();
#ifdef TX_PORT_USE_CCPN
        __set_ccpn_value(0);
#else
        __enable_interrupts();
#endif
        __syscall(TX_PORT_TRICORE_SYSCALL_ID_SCHEDULE);
        __restore_interrupt(interrupt_save);
    }
    else
    {
        _tx_thread_dispatch_request();
    }
}


#ifndef TX_DISABLE_INLINE

#define TX_INTERRUPT_SAVE_AREA                  UINT interrupt_save;
#define TX_DISABLE                              interrupt_save =  __disable_interrupts();
#define TX_RESTORE                              __restore_interrupt(interrupt_save);

/* Redefine _tx_thread_system_return for improved performance.  */

#define _tx_thread_system_return                _tx_thread_system_return_inline

#else   /* TX_DISABLE_INLINE is defined */

UINT   _tx_thread_interrupt_disable(void);
VOID   _tx_thread_interrupt_restore(UINT previous_posture);

#define TX_INTERRUPT_SAVE_AREA                  UINT interrupt_save;

#define TX_DISABLE                              interrupt_save = _tx_thread_interrupt_disable();
#define TX_RESTORE                              _tx_thread_interrupt_restore(interrupt_save);
#endif  /* TX_DISABLE_INLINE */



/* Define the version ID of ThreadX.  This may be utilized by the application.  */

#ifdef TX_THREAD_INIT
CHAR                            _tx_version_id[] =
                                    "(c) 2024 Microsoft Corp. (c) 2026-present Eclipse ThreadX contributors.  *  ThreadX TriCore TC1.6.2/GNU Version 6.5.2.202603b *";
#else
#ifdef TX_MISRA_ENABLE
extern  CHAR                    _tx_version_id[100];
#else
extern  CHAR                    _tx_version_id[];
#endif
#endif


#endif
