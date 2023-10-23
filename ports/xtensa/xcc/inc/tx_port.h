
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
/**                                                                       */
/** ThreadX Component                                                     */
/**                                                                       */
/**   Port Specific                                                       */
/**                                                                       */
/**************************************************************************/

/**************************************************************************/
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
/*  11-09-2020      Cadence Design Systems  Initial Version 6.1.2         */
/*  04-02-2021      Bhupendra Naphade       Modified comment(s), updated  */
/*                                            macro definition,           */
/*                                            resulting in version 6.1.6  */
/*  10-31-2022      Scott Larson            Modified comment(s), removed  */
/*                                            EPK extension,              */
/*                                            resulting in version 6.2.0  */
/*                                                                        */
/**************************************************************************/

#ifndef TX_PORT_H
#define TX_PORT_H

/* Determine if the optional ThreadX user define file should be used.  */

#ifdef TX_INCLUDE_USER_DEFINE_FILE
#include "tx_user.h"
#endif

/* Comment this out to disable thread-safe C library support. */
#define TX_THREAD_SAFE_CLIB    1

/* Include the glue to Xtensa-generic parts of this ThreadX port. */
#include "xtensa_rtos.h"

/* Parts of this file should not been seen by assembler sources. */
#ifndef __ASSEMBLER__

/* Some generic C sources call memset() and need this (else compiler warns).
   Until the generic sources take care of this, do it here. */

#include <string.h>


/* Define compiler library include files and library-specific macros. */

/*
    Thread-safety support for the supported C libraries. At this time
    only the newlib and Xtensa C libraries are supported.

    The C library reent structure can be quite large so it is placed
    at the end of TX_THREAD, and a pointer to it is defined near the
    beginning of TX_THREAD where assembly code can easily get to it
    at a fixed offset.
*/

#ifdef TX_THREAD_SAFE_CLIB
struct TX_THREAD_STRUCT;

extern char *_tx_clib_heap_start, *_tx_clib_heap_end;
extern void _tx_clib_reent_init (struct TX_THREAD_STRUCT *thread_ptr);
extern void _tx_clib_reent_cleanup (struct TX_THREAD_STRUCT *thread_ptr, int partial);
extern void _tx_clib_thread_setup(struct TX_THREAD_STRUCT *thread_ptr);

#include    <sys/reent.h>

#define TX_CLIB_THREAD_EXTENSION \
    VOID                (*tx_real_thread_entry)(ULONG id);  /* Actual entry point */ \
    struct _reent *     tx_thread_clib_ptr;                 /* C lib reentrancy ptr */
#define TX_CLIB_THREAD_EXTENSION_END \
    struct _reent       tx_thread_clib_reent;               /* C lib reentrancy struct */
#define TX_CLIB_THREAD_CREATE_EXTENSION(thread_ptr)
#define TX_CLIB_THREAD_DELETE_EXTENSION(thread_ptr)
#define TX_CLIB_THREAD_EXIT_EXTENSION(thread_ptr)
#define TX_THREAD_CLIB_EXTENSION(thread_ptr) \
    _tx_clib_thread_setup(thread_ptr);
#else
#define TX_CLIB_THREAD_EXTENSION
#define TX_CLIB_THREAD_EXTENSION_END
#define TX_CLIB_THREAD_CREATE_EXTENSION(thread_ptr)
#define TX_CLIB_THREAD_DELETE_EXTENSION(thread_ptr)
#define TX_CLIB_THREAD_EXIT_EXTENSION(thread_ptr)
#define TX_THREAD_CLIB_EXTENSION(thread_ptr)
#endif


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


#endif /* #ifndef __ASSEMBLER__ */


/* Define the priority levels for ThreadX.  Legal values range
   from 32 to 1024 and MUST be evenly divisible by 32.  */

#ifndef TX_MAX_PRIORITIES
#define TX_MAX_PRIORITIES                       32
#endif


/*
    Define the minimum stack size for a thread on this processor.
    If the size supplied during thread creation is less than TX_MINIMUM_STACK,
    the thread create call will return an error. The minimum allows for a
    thread whose entry function makes no calls and needs no local frame.
    TX_MINIMUM_STACK_BASIC allows the entry function to at least call
    tx_thread_relinquish(). An extra 0x10 bytes is allowed in all cases to
    allow for stack pointer alignment to 16 bytes. There is an additional premium
    for the stack checking functionality of TX_ENABLE_STACK_CHECKING.
    In Xtensa, all these amounts depend on the function call ABI used by the
    configuration (in general, Call0 ABI needs about 0x20 bytes less stack space
    per function call frame). These amounts assume no compiler optimization (-O0).
    Optimization usually requires less stack.

    TX_MINIMUM_STACK_BASIC is a MINIMUM for threads that call tx_thread_relinquish()
    only. Threads that do more, and in particular call C library functions such as
    printf(), need much more stack space and it is up to the application developer
    to determine how much.
*/

#ifdef __XTENSA_CALL0_ABI__
        /* Call0 ABI */

#ifndef TX_MINIMUM_STACK
#define TX_MINIMUM_STACK                        (XT_STK_FRMSZ + 0x10)
#endif
#ifdef  TX_ENABLE_STACK_CHECKING
#define TX_STACK_CHECK_PREMIUM                  0x30
#else
#define TX_STACK_CHECK_PREMIUM                  0
#endif
#ifndef TX_MINIMUM_STACK_BASIC
#define TX_MINIMUM_STACK_BASIC                  (XT_STK_FRMSZ + 0x70 + TX_STACK_CHECK_PREMIUM)
#endif

#else   /* Windowed ABI */

#ifndef TX_MINIMUM_STACK
#define TX_MINIMUM_STACK                        (XT_STK_FRMSZ + 0x20)
#endif
#ifdef  TX_ENABLE_STACK_CHECKING
#define TX_STACK_CHECK_PREMIUM                  0x50
#else
#define TX_STACK_CHECK_PREMIUM                  0
#endif
#ifndef TX_MINIMUM_STACK_BASIC
#define TX_MINIMUM_STACK_BASIC                  (XT_STK_FRMSZ + 0x100 + TX_STACK_CHECK_PREMIUM)
#endif

#endif

/*
    Minimum stack size for the ThreadX system stack on this processor.
    This is just a useful starting point for an application, it is not
    checked by ThreadX. The minimum system stack size allows for the
    possible depth of interrupt nesting (XCHAL_EXCM_LEVEL-1 interrupt
    stack frames and XCHAL_EXCM_LEVEL interrupt handlers including timer),
    assuming very basic interrupt handlers (allows 1 call12). It needs to
    be increased to support the application's real interrupt handlers (and
    timer interrupt if TX_TIMER_PROCESS_IN_ISR). The system stack is located
    where the stack pointer is inside tx_kernel_enter() which is usually from
    main(), and so is determined by the development tools. It grows downward
    toward the first available memory pointer passed to tx_application_define().
    An application should allow sufficient space for the system stack.

    For XEA3, allow a minimum of XCHAL_NUM_INTLEVELS nested interrupts. The stack
    frames are minimal-sized and may need to be increased to support real applications.
*/

#if XCHAL_HAVE_XEA3
#define TX_SYSTEM_STACK_MINIMUM                 (XCHAL_NUM_INTLEVELS * 0x40)
#ifndef TX_SYSTEM_STACK_SIZE
#if TX_SYSTEM_STACK_MINIMUM > 2048
#define TX_SYSTEM_STACK_SIZE                    TX_SYSTEM_STACK_MINIMUM
#else
#define TX_SYSTEM_STACK_SIZE                    2048
#endif
#endif
#else
#define TX_SYSTEM_STACK_MINIMUM                 (((XCHAL_EXCM_LEVEL-1) * (0x40 + XT_STK_FRMSZ)) + 0x40)
#ifndef TX_SYSTEM_STACK_SIZE
#define TX_SYSTEM_STACK_SIZE                    4096
#endif
#endif

/*
    Define the system timer thread's default stack size and priority.
    These are only applicable if TX_TIMER_PROCESS_IN_ISR is not defined.
*/

#ifndef TX_TIMER_THREAD_STACK_SIZE
#ifdef __XTENSA_CALL0_ABI__
#define TX_TIMER_THREAD_STACK_SIZE              (TX_MINIMUM_STACK_BASIC + 0x100)
#else
#define TX_TIMER_THREAD_STACK_SIZE              (TX_MINIMUM_STACK_BASIC + 0x200)
#endif
#endif


/* Parts of this file should not been seen by assembler sources. */
#ifndef __ASSEMBLER__


#ifndef TX_TIMER_THREAD_PRIORITY
#define TX_TIMER_THREAD_PRIORITY                0           /* Default timer thread priority    */
#endif


/* Define various constants for the ThreadX Xtensa port.  */

#if XCHAL_HAVE_XEA3
#define TX_INT_DISABLE                          0x8               /* Disable interrupts value */
#define TX_INT_ENABLE                           0x0               /* Enable interrupt value   */
#else
#define TX_INT_DISABLE                          XCHAL_EXCM_LEVEL  /* Disable interrupts value */
#define TX_INT_ENABLE                           0x0               /* Enable interrupt value   */
#endif


/*
    Define the clock source for trace event entry time stamp. The following
    two item are port specific. For example, if the time source is at the
    address 0x0a800024 and is 16-bits in size, the clock source constants
    would be:

    #define TX_TRACE_TIME_SOURCE                *((ULONG *) 0x0a800024)
    #define TX_TRACE_TIME_MASK                  0x0000FFFFUL
*/

#ifndef TX_TRACE_TIME_SOURCE
#define TX_TRACE_TIME_SOURCE                    ++_tx_trace_simulated_time
#endif
#ifndef TX_TRACE_TIME_MASK
#define TX_TRACE_TIME_MASK                      0xFFFFFFFFUL
#endif


/*
    Define the port specific options for the _tx_build_options variable.
    This variable indicates how the ThreadX library was built.
*/

#if defined(XT_SIMULATOR) || !defined(XT_BOARD)
#define TX_XT_OPT_SIMULATOR                     (1UL << 0)
#else
#define TX_XT_OPT_SIMULATOR                     0
#endif

#if defined(XT_BOARD)
#define TX_XT_OPT_BOARD                         (1UL << 1)
#else
#define TX_XT_OPT_BOARD                         0
#endif

#if defined(XT_INTEXC_HOOKS)
#define TX_XT_OPT_INTEXC_HOOKS                  (1UL << 2)
#else
#define TX_XT_OPT_INTEXC_HOOKS                  0
#endif

#if defined(TX_THREAD_SAFE_CLIB)
#define TX_XT_OPT_CLIB                          (1UL << 3)
#else
#define TX_XT_OPT_CLIB                          0
#endif

#define TX_PORT_SPECIFIC_BUILD_OPTIONS          (TX_XT_OPT_SIMULATOR | TX_XT_OPT_BOARD \
                                                | TX_XT_OPT_INTEXC_HOOKS | TX_XT_OPT_CLIB)


/*
    Define the in-line initialization constant so that modules with in-line
    initialization capabilities can prevent their initialization from being
    a function call.
*/

#define TX_INLINE_INITIALIZATION


/*
    Determine whether or not stack checking is enabled. By default, ThreadX
    stack checking is disabled. When the following is defined, ThreadX thread
    stack checking is enabled. If enabled (TX_ENABLE_STACK_CHECKING is defined),
    the TX_DISABLE_STACK_FILLING define is canceled, thereby forcing the stack
    fill which is necessary for the stack checking logic.
*/

#ifdef TX_ENABLE_STACK_CHECKING
#undef TX_DISABLE_STACK_FILLING
#endif


/*
    Define the TX_THREAD control block extensions for this port. The main
    reason for the multiple macros is so that backward compatibility can
    be maintained with existing ThreadX kernel awareness modules.
*/

#if XCHAL_CP_NUM > 0
#define TX_CP_THREAD_EXTENSION  \
    /* Co-proc state save areas, with padding for alignment: */ \
    UINT                tx_thread_cp_state[(XT_CP_SIZE+3)/4];
#else
#define TX_CP_THREAD_EXTENSION
#endif

#define TX_THREAD_EXTENSION_0 \
    /* These extensions needs to be accessed from assembly code at context switches */ \
    UINT                tx_thread_solicited;    /* Non-zero indicates solicited entry */ \
    TX_CLIB_THREAD_EXTENSION                    /* Ptr to C library re-ent struct */ \
    TX_CP_THREAD_EXTENSION          /* Coprocessor state save areas */

#define TX_THREAD_EXTENSION_1 \
    TX_CLIB_THREAD_EXTENSION_END

#define TX_THREAD_EXTENSION_2

/* Execution profile related */
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

#if XCHAL_CP_NUM > 0
extern void _xt_coproc_release(void * coproc_sa_base);

#define TX_THREAD_DELETE_EXTENSION(thread_ptr) \
        _xt_coproc_release(&thread_ptr->tx_thread_cp_state);
#define TX_THREAD_COMPLETED_EXTENSION(thread_ptr) \
        _xt_coproc_release(&thread_ptr->tx_thread_cp_state);
#define TX_THREAD_TERMINATED_EXTENSION(thread_ptr) \
        _xt_coproc_release(&thread_ptr->tx_thread_cp_state);
#define TX_THREAD_CP_EXTENSION(thread_ptr) \
    /* Initialize XT_CP_ASA ptr to aligned save area: */ \
    /* NOTE: keep this matched with xtensa_context.h. */ \
    thread_ptr->tx_thread_cp_state[0] = 0; \
    thread_ptr->tx_thread_cp_state[1] = 0; \
    thread_ptr->tx_thread_cp_state[2] = \
        ((((UINT)thread_ptr->tx_thread_cp_state)+12+XCHAL_TOTAL_SA_ALIGN-1) \
            & -XCHAL_TOTAL_SA_ALIGN);
#else
#define TX_THREAD_DELETE_EXTENSION(thread_ptr)
#define TX_THREAD_TERMINATED_EXTENSION(thread_ptr)
#define TX_THREAD_COMPLETED_EXTENSION(thread_ptr)
#define TX_THREAD_CP_EXTENSION(thread_ptr)
#endif

#define TX_THREAD_CREATE_EXTENSION(thread_ptr) \
    TX_THREAD_CLIB_EXTENSION(thread_ptr) \
    TX_THREAD_CP_EXTENSION(thread_ptr)


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


/*
    Define ThreadX interrupt lockout and restore macros for protection
    on access of critical kernel information. The restore interrupt macro
    must restore the interrupt posture of the running thread prior to the
    value present prior to the disable macro. In most cases, the save area
    macro is used to define a local function save area for the disable and
    restore macros.
*/

extern unsigned int                 _tx_thread_interrupt_control(unsigned int new_posture);

#define TX_INTERRUPT_SAVE_AREA      register UINT interrupt_save;

#ifdef TX_DISABLE_INLINE_MACROS
#define TX_DISABLE                  interrupt_save = _tx_thread_interrupt_control(TX_INT_DISABLE);
#define TX_RESTORE                  _tx_thread_interrupt_control(interrupt_save);
#else
#define TX_DISABLE                  interrupt_save = xthal_disable_interrupts();
#define TX_RESTORE                  xthal_restore_interrupts(interrupt_save);
#endif


/* Define the interrupt lockout macros for each ThreadX object.  */

#define TX_BLOCK_POOL_DISABLE                   TX_DISABLE
#define TX_BYTE_POOL_DISABLE                    TX_DISABLE
#define TX_EVENT_FLAGS_GROUP_DISABLE            TX_DISABLE
#define TX_MUTEX_DISABLE                        TX_DISABLE
#define TX_QUEUE_DISABLE                        TX_DISABLE
#define TX_SEMAPHORE_DISABLE                    TX_DISABLE


#if XCHAL_HAVE_XEA3
/* Variables that keep track of the timer and software interrupt numbers in use. */
extern int xt_sw_intnum;
extern int xt_timer_intnum;
#endif


/* Define the version ID of ThreadX.  This may be utilized by the application.  */

#ifdef TX_THREAD_INIT
CHAR                            _tx_version_id[] =
                                    "Copyright (c) Microsoft Corporation. All rights reserved. * Azure RTOS Xtensa Version 6.3.0 *";
#else
extern  CHAR                    _tx_version_id[];
#endif

#endif /* #ifndef __ASSEMBLER__ */

#endif

