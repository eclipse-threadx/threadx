/***************************************************************************
 * Copyright (c) 2025 10xEngineers
 *
 * This program and the accompanying materials are made available under the
 * terms of the MIT License which is available at
 * https://opensource.org/licenses/MIT.
 *
 * SPDX-License-Identifier: MIT
 **************************************************************************/


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
/*    tx_port.h                                          RISC-V32/GNU     */
/*                                                           6.4.x        */
/*                                                                        */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Akif Ejaz, 10xEngineers                                             */
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
/**************************************************************************/

#ifndef TX_PORT_H
#define TX_PORT_H

/* Include shared RISC-V32 port definitions common to all toolchain ports.  */
#include "../../common/tx_port_riscv32_common.h"





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


/* Define the version ID of ThreadX.  This may be utilized by the application.  */

#ifndef __ASSEMBLER__
#ifdef TX_THREAD_INIT
CHAR                            _tx_version_id[] =
                                    "(c) 2024 Microsoft Corp. (c) 2026-present Eclipse ThreadX contributors. * ThreadX RISC-V32/GNU Version 6.5.1.202602a *";
#else
extern  CHAR                    _tx_version_id[];
#endif /* TX_THREAD_INIT */
#endif /* __ASSEMBLER__ */

#endif /* TX_PORT_H */