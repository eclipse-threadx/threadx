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
/*    tx_port.h                                       SMP/ARC_HS/MetaWare */ 
/*                                                            6.1.6       */
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


/* Remove volatile for ThreadX source on the ARC. This is because the ARC 
   compiler generates different non-cache r/w access when using volatile 
   that is different from the assembly language access of the same 
   global variables in ThreadX.  */

#ifdef TX_SOURCE_CODE
#define volatile
#else
#ifdef NX_SOURCE_CODE
#define volatile
#else
#ifdef FX_SOURCE_CODE
#define volatile
#else
#ifdef UX_SOURCE_CODE
#define volatile
#endif
#endif
#endif
#endif

/************* Define ThreadX SMP constants.  *************/

/* Define the ThreadX SMP maximum number of cores.  */

#ifndef TX_THREAD_SMP_MAX_CORES
#define TX_THREAD_SMP_MAX_CORES                 2
#endif


/* Define the ThreadX SMP core mask. */

#ifndef TX_THREAD_SMP_CORE_MASK
#define TX_THREAD_SMP_CORE_MASK                 0x3            /* Where bit 0 represents Core 0, bit 1 represents Core 1, etc.  */
#endif


/* Define INLINE_DECLARE to whitespace for MetaWare compiler.  */

#define INLINE_DECLARE


/* Define dynamic number of cores option.  When commented out, the number of cores is static.  */

/* #define TX_THREAD_SMP_DYNAMIC_CORE_MAX  */


/* Define ThreadX SMP initialization macro.  */

#define TX_PORT_SPECIFIC_PRE_INITIALIZATION                 


/* Define ThreadX SMP pre-scheduler initialization.  */

#define TX_PORT_SPECIFIC_PRE_SCHEDULER_INITIALIZATION       


/* Enable the inter-core interrupt logic.  */

#define TX_THREAD_SMP_INTER_CORE_INTERRUPT


/* Determine if there is customer-specific wakeup logic needed.  */

#ifdef TX_THREAD_SMP_WAKEUP_LOGIC

/* Include customer-specific wakeup code.  */

#include "tx_thread_smp_core_wakeup.h"
#else

#ifdef TX_THREAD_SMP_DEFAULT_WAKEUP_LOGIC

/* Default wakeup code.  */
#define TX_THREAD_SMP_WAKEUP_LOGIC
#define TX_THREAD_SMP_WAKEUP(i)                _tx_thread_smp_core_preempt(i)
#endif
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


/* Define compiler library include files.  */

#include <stdlib.h>
#include <string.h>


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
#define TX_MINIMUM_STACK                        800         /* Minimum stack size for this port  */
#endif


/* Define the system timer thread's default stack size and priority.  These are only applicable
   if TX_TIMER_PROCESS_IN_ISR is not defined.  */

#ifndef TX_TIMER_THREAD_STACK_SIZE
#define TX_TIMER_THREAD_STACK_SIZE              2048        /* Default timer thread stack size  */
#endif

#ifndef TX_TIMER_THREAD_PRIORITY    
#define TX_TIMER_THREAD_PRIORITY                0           /* Default timer thread priority    */ 
#endif


/* Define various constants for the ThreadX ARC HS port.  */ 

#define TX_INT_ENABLE                           0x0000001F  /* Enable all interrupts            */
#define TX_INT_DISABLE_MASK                     0x00000000  /* Disable all interrupts           */ 


/* Define the clock source for trace event entry time stamp. The following two item are port specific.  
   For example, if the time source is at the address 0x0a800024 and is 16-bits in size, the clock 
   source constants would be:

#define TX_TRACE_TIME_SOURCE                    *((ULONG *) 0x0a800024)
#define TX_TRACE_TIME_MASK                      0x0000FFFFUL

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

#define TX_THREAD_EXTENSION_0       VOID        *__mw_threadx_tls; \
                                    int         __mw_errnum; \
                                    VOID        (*__mw_thread_exit)(struct TX_THREAD_STRUCT *);
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

#if __HIGHC__

/* The MetaWare thread safe C/C++ runtime library needs space to 
   store thread specific information.  In addition, a function pointer
   is also supplied so that certain thread-specific resources may be 
   released upon thread termination and/or thread completion.  */

#define TX_THREAD_CREATE_EXTENSION(thread_ptr)          \
                                                        thread_ptr -> __mw_threadx_tls = 0; \
                                                        thread_ptr -> __mw_errnum = 0; \
                                                        thread_ptr -> __mw_thread_exit =  TX_NULL;                                
#define TX_THREAD_DELETE_EXTENSION(thread_ptr)                                  
#define TX_THREAD_COMPLETED_EXTENSION(thread_ptr)       \
                                                        if (thread_ptr -> __mw_thread_exit) \
                                                            (thread_ptr -> __mw_thread_exit) (thread_ptr);
#define TX_THREAD_TERMINATED_EXTENSION(thread_ptr)      \
                                                        if (thread_ptr -> __mw_thread_exit) \
                                                            (thread_ptr -> __mw_thread_exit) (thread_ptr);

#else

#define TX_THREAD_CREATE_EXTENSION(thread_ptr)          
#define TX_THREAD_DELETE_EXTENSION(thread_ptr)                                  
#define TX_THREAD_COMPLETED_EXTENSION(thread_ptr)       
#define TX_THREAD_TERMINATED_EXTENSION(thread_ptr)      

#endif


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
    ULONG           tx_thread_smp_protect_status32;
    ULONG           tx_thread_smp_protect_release_caller;
} TX_THREAD_SMP_PROTECT;


/* Define ThreadX interrupt lockout and restore macros for protection on 
   access of critical kernel information.  The restore interrupt macro must 
   restore the interrupt posture of the running thread prior to the value 
   present prior to the disable macro.  In most cases, the save area macro
   is used to define a local function save area for the disable and restore
   macros.  */

#define TX_INTERRUPT_SAVE_AREA                  UINT interrupt_save;

#define TX_DISABLE                              interrupt_save =  _tx_thread_smp_protect();
#define TX_RESTORE                              _tx_thread_smp_unprotect(interrupt_save);


/************* End ThreadX SMP data type and function prototype definitions.  *************/


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
                                    "Copyright (c) Microsoft Corporation. All rights reserved.  *  ThreadX SMP/ARC_HS/MetaWare Version 6.1.6 *";
#else
extern  CHAR                    _tx_version_id[];
#endif


#endif





