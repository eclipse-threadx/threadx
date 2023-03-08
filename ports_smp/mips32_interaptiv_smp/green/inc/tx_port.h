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


/**************************************************************************/
/*                                                                        */
/*  PORT SPECIFIC C INFORMATION                            RELEASE        */
/*                                                                        */
/*    tx_port.h                             MIPS32_interAptiv/Green Hills */
/*                                                           6.2.1        */
/*                                                                        */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Scott Larson, Microsoft Corporation                                 */
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
/*  03-08-2023      Scott Larson            Initial release version 6.2.1 */
/*                                                                        */
/**************************************************************************/

#ifndef TX_PORT_H
#define TX_PORT_H



/************* Define ThreadX SMP constants.  *************/

/* Define the ThreadX SMP maximum number of cores.  */

#ifndef TX_THREAD_SMP_MAX_CORES
#define TX_THREAD_SMP_MAX_CORES                 6               /* 3 cores, 2 VPEs per core   */
#endif


/* Define the ThreadX SMP core mask. */

#ifndef TX_THREAD_SMP_CORE_MASK
#define TX_THREAD_SMP_CORE_MASK                 0x3F            /* here bit 0 represents Core 0, bit 1 represents Core 1, etc.  */
#endif


/* Define dynamic number of cores option.  When commented out, the number of cores is static.  */

/* #define TX_THREAD_SMP_DYNAMIC_CORE_MAX  */


/* Define ThreadX SMP initialization macro.  */

#define TX_PORT_SPECIFIC_PRE_INITIALIZATION


/* Define ThreadX SMP pre-scheduler initialization.  */

#define TX_PORT_SPECIFIC_PRE_SCHEDULER_INITIALIZATION


/* Enable the inter-core interrupt logic.  When commented out, inter-core interrupts are not enabled.  */

/* #define TX_THREAD_SMP_INTER_CORE_INTERRUPT  */


/* For GHS debugger compatibility, map the structure SMP members to the previous names.  */

#define tx_thread_smp_core_mapped           tx_thread_mips32_mapped_vpe
#define tx_thread_smp_core_control          tx_thread_mips32_vpe_control


/* Determine if the optional wakeup macro has been defined. This definition can be
   made here or prior to this file being included. When TX_THREAD_SMP_WAKEUP(i) is
   defined (where "i" is the core), the wakeup logic is also brought in.  */

#ifdef TX_THREAD_SMP_WAKEUP
#define TX_THREAD_SMP_WAKEUP_LOGIC
#endif


/* Determine if there is customer-specific wakeup logic needed.  */

#ifdef TX_THREAD_SMP_WAKEUP_LOGIC

/* Include customer-specific wakeup code.  */

#include "tx_thread_smp_core_wakeup.h"
#endif


/* Ensure that the in-line resume/suspend define is not allowed.  */

#ifdef TX_INLINE_THREAD_RESUME_SUSPEND
#undef TX_INLINE_THREAD_RESUME_SUSPEND
#endif


/************* End ThreadX SMP constants.  *************/


/* Determine if the optional ThreadX user define file should be used.  */

#ifdef TX_INCLUDE_USER_DEFINE_FILE


/* Yes, include the user defines in tx_user.h. The defines in this file may
   alternately be defined on the command line.  */

#include "tx_user.h"
#endif

/* Define the TX_MEMSET macro to remove library reference.  */

#define TX_MEMSET(a,b,c)                                    {                                       \
                                                            UCHAR *ptr;                             \
                                                            UCHAR value;                            \
                                                            UINT  i, size;                          \
                                                                ptr =    (UCHAR *) ((VOID *) a);    \
                                                                value =  (UCHAR) b;                 \
                                                                size =   (UINT) c;                  \
                                                                for (i = 0; i < size; i++)          \
                                                                {                                   \
                                                                    *ptr++ =  value;                \
                                                                }                                   \
                                                            }


/* Ensure that the in-line resume/suspend define is not allowed.  */

#ifdef TX_INLINE_THREAD_RESUME_SUSPEND
#undef TX_INLINE_THREAD_RESUME_SUSPEND
#endif


/* Define compiler library include files.  */

#include <stdlib.h>
#include <string.h>
#include <mips_ghs.h>
#ifdef TX_SOURCE_CODE
#include "tx_ghs.h"
#endif


/* Define ThreadX basic types for this port.  */

#define VOID                                                void
typedef char                                                CHAR;
typedef unsigned char                                       UCHAR;
typedef int                                                 INT;
typedef unsigned int                                        UINT;
typedef long                                                LONG;
typedef unsigned long                                       ULONG;
typedef short                                               SHORT;
typedef unsigned short                                      USHORT;


/* Define the priority levels for ThreadX.  Legal values range
   from 32 to 1024 and MUST be evenly divisible by 32.  */

#ifndef TX_MAX_PRIORITIES
#define TX_MAX_PRIORITIES                                   32
#endif


/* Define the minimum stack for a ThreadX thread on this processor. If the size supplied during
   thread creation is less than this value, the thread create call will return an error.  */

#ifndef TX_MINIMUM_STACK
#define TX_MINIMUM_STACK                                    1024        /* Minimum stack size for this port  */
#endif


/* Define the system timer thread's default stack size and priority.  These are only applicable
   if TX_TIMER_PROCESS_IN_ISR is not defined.  */

#ifndef TX_TIMER_THREAD_STACK_SIZE
#define TX_TIMER_THREAD_STACK_SIZE                          2048        /* Default timer thread stack size  */
#endif

#ifndef TX_TIMER_THREAD_PRIORITY
#define TX_TIMER_THREAD_PRIORITY                            0           /* Default timer thread priority    */
#endif


/* Define various constants for the ThreadX MIPS port.  */

#define TX_INT_DISABLE                                      0x00000000  /* Disable interrupts value */
#define TX_INT_ENABLE                                       0x00000001  /* Enable interrupt value   */


/* Define constants for Green Hills EventAnalyzer.  */

/* Define the number of ticks per second. This informs the EventAnalyzer what the timestamps
   represent.  By default, this is set to 1,000,000 i.e., one tick every microsecond. */

#define TX_EL_TICKS_PER_SECOND                              1000000


/* Define prototype of internal time get function.  */

ULONG   _tx_thread_smp_time_get(void);
ULONG   _tx_thread_smp_time_get_upper(void);


/* Define the method of how to get the upper and lower 32-bits of the time stamp. By default, simply
   simulate the time-stamp source with a counter.  */

#define read_tbu()                                          _tx_thread_smp_time_get_upper()
#define read_tbl()                                          _tx_thread_smp_time_get()


/* Define the clock source for trace event entry time stamp. The following two item are port specific.
   For example, if the time source is at the address 0x0a800024 and is 16-bits in size, the clock
   source constants would be:

#define TX_TRACE_TIME_SOURCE                                *((ULONG *) 0x0a800024)
#define TX_TRACE_TIME_MASK                                  0x0000FFFFUL

*/

#ifndef TX_MISRA_ENABLE
#ifndef TX_TRACE_TIME_SOURCE
#define TX_TRACE_TIME_SOURCE                                _tx_thread_smp_time_get()
#endif
#else
#ifndef TX_TRACE_TIME_SOURCE
ULONG   _tx_misra_time_stamp_get(VOID);
#define TX_TRACE_TIME_SOURCE                                _tx_misra_time_stamp_get()
#endif
#endif
#ifndef TX_TRACE_TIME_MASK
#define TX_TRACE_TIME_MASK                                  0xFFFFFFFFUL
#endif


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

#define TX_THREAD_EXTENSION_0
#define TX_THREAD_EXTENSION_1                               VOID *  tx_thread_eh_globals;
#define TX_THREAD_EXTENSION_2
#define TX_THREAD_EXTENSION_3                               int     Errno;  \
                                                            char *  strtok_saved_pos;



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

/* The MIPS32 architecture has the CLZ instruction. Redefine the macro for calculating the
   lowest bit set to use this instruction.  */
#ifndef TX_DISABLE_INLINE

#define TX_LOWEST_SET_BIT_CALCULATE(m, b)       m = m & ((ULONG) (-((LONG) m))); \
                                                b = __CLZ32(m); \
                                                b = 31 - b;

#endif


/************* Define ThreadX SMP data types and function prototypes.  *************/


struct TX_THREAD_STRUCT;


/* Define the ThreadX SMP protection structure.   */

typedef struct TX_THREAD_SMP_PROTECT_STRUCT
{
    ULONG           tx_thread_smp_protect_in_force;
    struct TX_THREAD_STRUCT *
                    tx_thread_smp_protect_thread;
    ULONG           tx_thread_smp_protect_core;
    ULONG           tx_thread_smp_protect_count;

    /* Implementation specific information follows.  */

    ULONG           tx_thread_smp_protect_get_caller;
    ULONG           tx_thread_smp_protect_sr;
    ULONG           tx_thread_smp_protect_release_caller;
} TX_THREAD_SMP_PROTECT;


#ifndef TX_DISABLE_INLINE

/* Define the get thread in-line assembly.   */

struct TX_THREAD_STRUCT;

asm struct TX_THREAD_STRUCT *_tx_thread_smp_current_thread_get(void)
{
%
    di      $9                                  # Disable interrupts
    mfc0    $25, $4,2                           # Pickup VPE ID (UserLocal)
    la      $12, _tx_thread_current_ptr         # Pickup the current thread pointer
    sll     $25, $25, 2                         # Build index based on VPE number
    addu    $12, $12, $25                       # Build address of current thread pointer for this VPE
    lw      $2, ($12)                           # Pickup current thread pointer
    mtc0    $9, $12                             # Restore interrupts
%error
}


/* Define the get thread macro. Without in-line assembly, this would map to _tx_thread_smp_current_thread_get(). */

/* Define the get core ID in-line assembly.  */

asm UINT _tx_thread_smp_core_get(void)
{
%
    mfc0    $2, $4,2                            # Pickup VPE ID (UserLocal)
%error
}

#endif


/* Define ThreadX interrupt lockout and restore macros for protection on
   access of critical kernel information.  The restore interrupt macro must
   restore the interrupt posture of the running thread prior to the value
   present prior to the disable macro.  In most cases, the save area macro
   is used to define a local function save area for the disable and restore
   macros.  */

#define TX_INTERRUPT_SAVE_AREA                  unsigned int interrupt_save;

#define TX_DISABLE                              interrupt_save =  _tx_thread_smp_protect();
#define TX_RESTORE                              _tx_thread_smp_unprotect(interrupt_save);



/* Define some extra variables used by the MIPS interAptiv SMP implementation.  */

#ifdef TX_THREAD_INIT
#define THREAD_SMP_DECLARE
#else
#define THREAD_SMP_DECLARE extern
#endif

THREAD_SMP_DECLARE ULONG    _tx_thread_smp_detected_cores;
THREAD_SMP_DECLARE ULONG    _tx_thread_smp_detected_vpes_per_core;
THREAD_SMP_DECLARE ULONG    _tx_thread_smp_system_error;
#ifdef TX_ENABLE_64BIT_FPU_SUPPORT
THREAD_SMP_DECLARE ULONG    _tx_thread_smp_initial_fpu_control_register;
#endif

/************* End ThreadX SMP data type and function prototype definitions.  *************/


/* Define the interrupt lockout macros for each ThreadX object.  */

#define TX_BLOCK_POOL_DISABLE                               TX_DISABLE
#define TX_BYTE_POOL_DISABLE                                TX_DISABLE
#define TX_EVENT_FLAGS_GROUP_DISABLE                        TX_DISABLE
#define TX_MUTEX_DISABLE                                    TX_DISABLE
#define TX_QUEUE_DISABLE                                    TX_DISABLE
#define TX_SEMAPHORE_DISABLE                                TX_DISABLE


/* Define the port specific options for the _tx_build_options variable. This variable indicates
   how the ThreadX library was built.  */

#ifdef TX_ENABLE_64BIT_FPU_SUPPORT
#define TX_FPU64_ENABLED                                    2
#else
#define TX_FPU64_ENABLED                                    0
#endif

#ifdef TX_ENABLE_DSP_SUPPORT
#define TX_DSP_ENABLED                                      4
#else
#define TX_DSP_ENABLED                                      0
#endif

#define TX_PORT_SPECIFIC_BUILD_OPTIONS                      (TX_FPU64_ENABLED | TX_DSP_ENABLED)



/* Define the version ID of ThreadX.  This may be utilized by the application.  */

#ifdef TX_THREAD_INIT
CHAR                            _tx_version_id[] =
                                    "Copyright (c) Microsoft Corporation. All rights reserved. * ThreadX SMP MIPS32_interAptiv/Green Hills Version 6.2.1 *";
#else
extern  CHAR                    _tx_version_id[];
#endif

#endif
