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
/**   Port Specific                                                       */
/**                                                                       */
/**************************************************************************/
/**************************************************************************/


/**************************************************************************/ 
/*                                                                        */ 
/*  PORT SPECIFIC C INFORMATION                            RELEASE        */ 
/*                                                                        */ 
/*    tx_port.h                                     Cortex-A8/Green Hills */ 
/*                                                           6.1.6        */
/*                                                                        */
/*  AUTHOR                                                                */
/*                                                                        */
/*    William E. Lamie, Microsoft Corporation                             */
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
/*  04-02-2021     Bhupendra Naphade        Modified comment(s),updated   */
/*                                            macro definition,           */
/*                                            resulting in version 6.1.6  */
/*                                                                        */
/**************************************************************************/

#ifndef TX_PORT_H
#define TX_PORT_H


/* Determine if the optional ThreadX user define file should be used.  */

#ifdef TX_INCLUDE_USER_DEFINE_FILE


/* Yes, include the user defines in tx_user.h. The defines in this file may 
   alternately be defined on the command line.  */

#include "tx_user.h"
#endif


/* Define compiler library include files.  */

#include <stdlib.h>
#include <string.h>
#include <arm_ghs.h>
#include "tx_ghs.h"


/* Define ThreadX basic types for this port.  */ 

#define VOID                                    void
typedef char                                    CHAR;
typedef unsigned char                           UCHAR;
typedef int                                     INT;
typedef unsigned int                            UINT;
typedef long                                    LONG;
typedef unsigned long                           ULONG;
typedef short                                   SHORT;
typedef unsigned short                          USHORT;


/* Define the priority levels for ThreadX.  Legal values range
   from 32 to 1024 and MUST be evenly divisible by 32.  */

#ifndef TX_MAX_PRIORITIES
#define TX_MAX_PRIORITIES                       32
#endif


/* Define the minimum stack for a ThreadX thread on this processor. If the size supplied during
   thread creation is less than this value, the thread create call will return an error.  */

#ifndef TX_MINIMUM_STACK
#define TX_MINIMUM_STACK                        200         /* Minimum stack size for this port  */
#endif


/* Define the system timer thread's default stack size and priority.  These are only applicable
   if TX_TIMER_PROCESS_IN_ISR is not defined.  */

#ifndef TX_TIMER_THREAD_STACK_SIZE
#define TX_TIMER_THREAD_STACK_SIZE              1024        /* Default timer thread stack size  */
#endif

#ifndef TX_TIMER_THREAD_PRIORITY    
#define TX_TIMER_THREAD_PRIORITY                0           /* Default timer thread priority    */ 
#endif


/* Define various constants for the ThreadX ARM port.  */ 

#ifdef TX_ENABLE_FIQ_SUPPORT
#define TX_INT_DISABLE                          0xC0        /* Disable IRQ & FIQ interrupts     */
#else
#define TX_INT_DISABLE                          0x80        /* Disable IRQ interrupts           */
#endif
#define TX_INT_ENABLE                           0x00        /* Enable IRQ interrupts            */


/* Define constants for Green Hills EventAnalyzer.  */

/* Define the number of ticks per second. This informs the EventAnalyzer what the timestamps
   represent.  By default, this is set to 1,000,000 i.e., one tick every microsecond. */

#define TX_EL_TICKS_PER_SECOND                  1000000     

/* Define the method of how to get the upper and lower 32-bits of the time stamp. By default, simply
   simulate the time-stamp source with a counter.  */                                                            

#define read_tbu()                              _tx_el_time_base_upper    
#define read_tbl()                              ++_tx_el_time_base_lower   


/* Define the port specific options for the _tx_build_options variable. This variable indicates
   how the ThreadX library was built.  */

#ifdef TX_ENABLE_FIQ_SUPPORT
#define TX_FIQ_ENABLED                          1
#else
#define TX_FIQ_ENABLED                          0
#endif

#ifdef TX_ENABLE_IRQ_NESTING
#define TX_IRQ_NESTING_ENABLED                  2
#else
#define TX_IRQ_NESTING_ENABLED                  0
#endif

#ifdef TX_ENABLE_FIQ_NESTING
#define TX_FIQ_NESTING_ENABLED                  4
#else
#define TX_FIQ_NESTING_ENABLED                  0
#endif

#define TX_PORT_SPECIFIC_BUILD_OPTIONS          (TX_FIQ_ENABLED | TX_IRQ_NESTING_ENABLED | TX_FIQ_NESTING_ENABLED)


/* Define the in-line initialization constant so that modules with in-line
   initialization capabilities can prevent their initialization from being
   a function call.  */

#define TX_INLINE_INITIALIZATION


/* Determine whether or not stack checking is enabled. By default, ThreadX stack checking is 
   disabled. When the following is defined, ThreadX thread stack checking is enabled.  If stack
   checking is enabled (TX_ENABLE_STACK_CHECKING is defined), the TX_DISABLE_STACK_FILLING
   define is negated, thereby forcing the stack fill which is necessary for the stack checking
   logic.  */

#ifdef TX_ENABLE_STACK_CHECKING
#undef TX_DISABLE_STACK_FILLING
#endif


/* Define the TX_THREAD control block extensions for this port. The main reason
   for the multiple macros is so that backward compatibility can be maintained with 
   existing ThreadX kernel awareness modules.  */

#define TX_THREAD_EXTENSION_0          
#define TX_THREAD_EXTENSION_1                   
#define TX_THREAD_EXTENSION_2                   ULONG       tx_thread_vfp_enable;                       \
                                                VOID *  tx_thread_eh_globals;                           \
                                                int     Errno;             /* errno.  */                \
                                                char *  strtok_saved_pos;  /* strtok() position.  */
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

#if (__GHS_VERSION_NUMBER >= 500)
#define TX_THREAD_CREATE_EXTENSION(thread_ptr)                                  \
    {                                                                           \
        extern void __tx_cpp_exception_init(TX_THREAD *thread_ptr);             \
        __tx_cpp_exception_init(thread_ptr);                                    \
    }
#else
#define TX_THREAD_CREATE_EXTENSION(thread_ptr)                                  \
    {                                                                           \
        #pragma weak __cpp_exception_init                                       \
        extern void __cpp_exception_init(void **);                              \
        static void (*const cpp_init_funcp)(void **) = __cpp_exception_init;    \
        if (cpp_init_funcp)                                                     \
            __cpp_exception_init(&(thread_ptr -> tx_thread_eh_globals));        \
    }
#endif

#if (__GHS_VERSION_NUMBER >= 500)
#define TX_THREAD_DELETE_EXTENSION(thread_ptr)                                  \
    {                                                                           \
        extern void __tx_cpp_exception_cleanup(TX_THREAD *thread_ptr);          \
        __tx_cpp_exception_cleanup(thread_ptr);                                 \
    }
#else 
#define TX_THREAD_DELETE_EXTENSION(thread_ptr)                                  \
    {                                                                           \
        #pragma weak __cpp_exception_cleanup                                    \
        extern void __cpp_exception_cleanup(void **);                           \
        static void (*const cpp_cleanup_funcp)(void **) =                       \
            __cpp_exception_cleanup;                                            \
        if (cpp_cleanup_funcp)                                                  \
            __cpp_exception_cleanup(&(thread_ptr -> tx_thread_eh_globals));     \
    }
#endif

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


/* Determine if the ARM architecture has the CLZ instruction. This is available on 
   architectures v5 and above. If available, redefine the macro for calculating the 
   lowest bit set.  */

#define TX_LOWEST_SET_BIT_CALCULATE(m, b)       m = m & ((ULONG) (-((LONG) m))); \
                                                b = __CLZ32(m); \
                                                b = 31 - b; 


/* Define ThreadX interrupt lockout and restore macros for protection on 
   access of critical kernel information.  The restore interrupt macro must 
   restore the interrupt posture of the running thread prior to the value 
   present prior to the disable macro.  In most cases, the save area macro
   is used to define a local function save area for the disable and restore
   macros.  */

#if defined(__THUMB)

unsigned int                    _tx_thread_interrupt_disable(void);
void                            _tx_thread_interrupt_restore(unsigned int new_posture);

#define TX_INTERRUPT_SAVE_AREA                  register INT interrupt_save;

#define TX_DISABLE                              interrupt_save = _tx_thread_interrupt_disable();

#define TX_RESTORE                              _tx_thread_interrupt_restore(interrupt_save);

#else

#define TX_INTERRUPT_SAVE_AREA                  register INT interrupt_save;

#if defined(__GHS_VERSION_NUMBER) && (__GHS_VERSION_NUMBER >= 350)

/* Define ThreadX interrupt lockout and restore macros using
   compiler built-in functions if using Green Hills ARM compiler
   version 3.5 or later.  */

#ifdef TX_ENABLE_FIQ_SUPPORT
#define TX_DISABLE interrupt_save               = __GETSR(); \
                                                __SETSR(interrupt_save | 0xC0);

#define TX_RESTORE                              __SETSR(interrupt_save);
#else
#define TX_DISABLE                              interrupt_save = __GETSR(); \
                                                __SETSR(interrupt_save | 0x80);

#define TX_RESTORE                              __SETSR(interrupt_save);
#endif

#else

/* Define ThreadX interrupt lockout and restore macros using
   asm macros if using Green Hills ARM compiler earlier than
   version 3.5.  */

asm int disable_ints(void)
{
%
    MRS r0,CPSR
#ifdef TX_BEFORE_ARMV6
#ifdef TX_ENABLE_FIQ_SUPPORT
    ORR r1,r0,0xC0
#else
    ORR r1,r0,0x80
#endif
    MSR CPSR_c,r1
#else
#ifdef TX_ENABLE_FIQ_SUPPORT
    CPSID if   
#else
    CPSID i
#endif
#endif
%error
}

asm void restore_ints(int a)
{
%reg a
    MSR CPSR_c,a
%mem a
    LDR r0,a
    MSR CPSR_c,r0
%error
}

#define TX_DISABLE                              interrupt_save = disable_ints();

#define TX_RESTORE                              restore_ints(interrupt_save);
#endif
#endif


/* Define VFP extension for the Cortex-A8.  Each is assumed to be called in the context of the executing
   thread.  */

void    tx_thread_vfp_enable(void);
void    tx_thread_vfp_disable(void);


/* Define the interrupt lockout macros for each ThreadX object.  */

#define TX_BLOCK_POOL_DISABLE                   TX_DISABLE
#define TX_BYTE_POOL_DISABLE                    TX_DISABLE
#define TX_EVENT_FLAGS_GROUP_DISABLE            TX_DISABLE
#define TX_MUTEX_DISABLE                        TX_DISABLE
#define TX_QUEUE_DISABLE                        TX_DISABLE
#define TX_SEMAPHORE_DISABLE                    TX_DISABLE


/* Define the version ID of ThreadX.  This may be utilized by the application.  */

#ifdef TX_THREAD_INIT
CHAR                            _tx_version_id[] =
                                    "Copyright (c) Microsoft Corporation. All rights reserved.  *  ThreadX Cortex-A8/Green Hills Version 6.1.6 *";
#else
extern  CHAR                    _tx_version_id[];
#endif


#endif

