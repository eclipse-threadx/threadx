/***************************************************************************
 * Copyright (c) 2026 Eclipse ThreadX contributors
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
/*    tx_port.h                                             RH850/GNU     */
/*                                                           6.5.2        */
/*                                                                        */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Parthiban Nallathambi, Linumiz                                      */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This file contains data type definitions that make the ThreadX      */
/*    real-time kernel function identically on a variety of different     */
/*    processor architectures. For example, the size or number of bits    */
/*    in an "int" data type vary between microprocessor architectures     */
/*    and therefore the ThreadX C data types are mapped to the RH850      */
/*    G4KH core with the GNU tools here. Interrupt masking uses the PLMR  */
/*    core mask register.                                                 */
/*                                                                        */
/**************************************************************************/

#ifndef TX_PORT_H
#define TX_PORT_H

#include <string.h>

#ifdef TX_INCLUDE_USER_DEFINE_FILE
#include "tx_user.h"
#endif

#include "rh850_core.h"

#define VOID void
typedef char CHAR;
typedef unsigned char UCHAR;
typedef int INT;
typedef unsigned int UINT;
typedef long LONG;
typedef unsigned long ULONG;
typedef unsigned long long ULONG64;
typedef short SHORT;
typedef unsigned short USHORT;
#define ULONG64_DEFINED

/* Priorities below TX_PORT_MAX_IPL stay unmasked and must not call ThreadX */
#ifndef TX_PORT_MAX_IPL
#define TX_PORT_MAX_IPL 0
#endif
#define TX_INT_DISABLE ((UINT)RH850_CORE_MASK(TX_PORT_MAX_IPL))
#define TX_INT_ENABLE  ((UINT)RH850_CORE_MASK_NONE)

#ifndef TX_TIMER_TICKS_PER_SECOND
#define TX_TIMER_TICKS_PER_SECOND 1000UL
#endif

#ifndef TX_MAX_PRIORITIES
#define TX_MAX_PRIORITIES 32
#endif

#ifndef TX_MINIMUM_STACK
#define TX_MINIMUM_STACK 512
#endif

#ifndef TX_TIMER_THREAD_STACK_SIZE
#define TX_TIMER_THREAD_STACK_SIZE 1024
#endif

#ifndef TX_TIMER_THREAD_PRIORITY
#define TX_TIMER_THREAD_PRIORITY 0
#endif

#ifndef TX_TRACE_TIME_SOURCE
#define TX_TRACE_TIME_SOURCE ++_tx_trace_simulated_time
#endif

#ifndef TX_TRACE_TIME_MASK
#define TX_TRACE_TIME_MASK 0xFFFFFFFFUL
#endif

#define TX_PORT_SPECIFIC_BUILD_OPTIONS 0

#define TX_INLINE_INITIALIZATION

#ifdef TX_ENABLE_STACK_CHECKING
#undef TX_DISABLE_STACK_FILLING
#endif

#define TX_THREAD_EXTENSION_0
#define TX_THREAD_EXTENSION_1
#define TX_THREAD_EXTENSION_2
#define TX_THREAD_EXTENSION_3
#define TX_BLOCK_POOL_EXTENSION
#define TX_BYTE_POOL_EXTENSION
#define TX_EVENT_FLAGS_GROUP_EXTENSION
#define TX_MUTEX_EXTENSION
#define TX_QUEUE_EXTENSION
#define TX_SEMAPHORE_EXTENSION
#define TX_TIMER_EXTENSION

#ifndef TX_THREAD_USER_EXTENSION
#define TX_THREAD_USER_EXTENSION
#endif

#define TX_THREAD_CREATE_EXTENSION(thread_ptr)
#define TX_THREAD_DELETE_EXTENSION(thread_ptr)
#define TX_THREAD_COMPLETED_EXTENSION(thread_ptr)
#define TX_THREAD_TERMINATED_EXTENSION(thread_ptr)
#define TX_BLOCK_POOL_CREATE_EXTENSION(pool_ptr)
#define TX_BYTE_POOL_CREATE_EXTENSION(pool_ptr)
#define TX_EVENT_FLAGS_GROUP_CREATE_EXTENSION(group_ptr)
#define TX_MUTEX_CREATE_EXTENSION(mutex_ptr)
#define TX_QUEUE_CREATE_EXTENSION(queue_ptr)
#define TX_SEMAPHORE_CREATE_EXTENSION(semaphore_ptr)
#define TX_TIMER_CREATE_EXTENSION(timer_ptr)
#define TX_BLOCK_POOL_DELETE_EXTENSION(pool_ptr)
#define TX_BYTE_POOL_DELETE_EXTENSION(pool_ptr)
#define TX_EVENT_FLAGS_GROUP_DELETE_EXTENSION(group_ptr)
#define TX_MUTEX_DELETE_EXTENSION(mutex_ptr)
#define TX_QUEUE_DELETE_EXTENSION(queue_ptr)
#define TX_SEMAPHORE_DELETE_EXTENSION(semaphore_ptr)
#define TX_TIMER_DELETE_EXTENSION(timer_ptr)

#define TX_PORT_SR_(id, sel) "i"(id), "i"(sel)
#define TX_PORT_SR(sr)       TX_PORT_SR_(sr)

static inline UINT _tx_port_mask_get(VOID)
{
	UINT v;

	__asm__ volatile("stsr %1, %0, %2" : "=r"(v) : TX_PORT_SR(RH850_CORE_MASK_SR));
	return v;
}

static inline VOID _tx_port_mask_set(UINT v)
{
	__asm__ volatile("ldsr %0, %1, %2" ::"r"(v), TX_PORT_SR(RH850_CORE_MASK_SR) : "memory");
}

#define TX_INTERRUPT_SAVE_AREA UINT interrupt_save;
#define TX_DISABLE                                   \
	{                                            \
		interrupt_save = _tx_port_mask_get(); \
		_tx_port_mask_set(TX_INT_DISABLE);   \
	}
#define TX_RESTORE { _tx_port_mask_set(interrupt_save); }

UINT _tx_thread_interrupt_control(UINT new_posture);

VOID tx_port_setup_timer_interrupt(VOID);
extern UCHAR tx_port_free_memory[];
#ifndef TX_PORT_FREE_MEMORY_SIZE
#define TX_PORT_FREE_MEMORY_SIZE 8192U
#endif
VOID _tx_timer_interrupt(VOID);

#ifdef TX_LOW_POWER
VOID tx_port_low_power_timer_setup(ULONG ticks);
ULONG tx_port_low_power_adjust(VOID);
#ifndef TX_LOW_POWER_TIMER_SETUP
#define TX_LOW_POWER_TIMER_SETUP(ticks) tx_port_low_power_timer_setup(ticks)
#endif
#ifndef TX_LOW_POWER_USER_TIMER_ADJUST
#define TX_LOW_POWER_USER_TIMER_ADJUST tx_port_low_power_adjust()
#endif
#endif

#define TX_BLOCK_POOL_DISABLE        TX_DISABLE
#define TX_BYTE_POOL_DISABLE         TX_DISABLE
#define TX_EVENT_FLAGS_GROUP_DISABLE TX_DISABLE
#define TX_MUTEX_DISABLE             TX_DISABLE
#define TX_QUEUE_DISABLE             TX_DISABLE
#define TX_SEMAPHORE_DISABLE         TX_DISABLE

#ifdef TX_REGRESSION_TEST
typedef unsigned int TEST_FLAG;
extern TEST_FLAG threadx_byte_allocate_loop_test;
extern TEST_FLAG threadx_byte_release_loop_test;
extern TEST_FLAG threadx_mutex_suspension_put_test;
extern TEST_FLAG threadx_mutex_suspension_priority_test;
#ifndef TX_TIMER_PROCESS_IN_ISR
extern TEST_FLAG threadx_delete_timer_thread;
#endif
extern void abort_and_resume_byte_allocating_thread(void);
extern void abort_all_threads_suspended_on_mutex(void);
extern void suspend_lowest_priority(void);
#ifndef TX_TIMER_PROCESS_IN_ISR
extern void delete_timer_thread(void);
#endif
extern TEST_FLAG test_stack_analyze_flag;
extern TEST_FLAG test_initialize_flag;
extern TEST_FLAG test_forced_mutex_timeout;

#define TX_BYTE_ALLOCATE_EXTENSION                                   \
	if (threadx_byte_allocate_loop_test == ((TEST_FLAG)1)) {     \
		pool_ptr->tx_byte_pool_owner = TX_NULL;              \
		threadx_byte_allocate_loop_test = ((TEST_FLAG)0);    \
	}

#define TX_BYTE_RELEASE_EXTENSION                                    \
	if (threadx_byte_release_loop_test == ((TEST_FLAG)1)) {      \
		threadx_byte_release_loop_test = ((TEST_FLAG)0);     \
		abort_and_resume_byte_allocating_thread();           \
	}

#define TX_MUTEX_PUT_EXTENSION_1                                     \
	if (threadx_mutex_suspension_put_test == ((TEST_FLAG)1)) {   \
		threadx_mutex_suspension_put_test = ((TEST_FLAG)0);  \
		abort_all_threads_suspended_on_mutex();              \
	}

#define TX_MUTEX_PUT_EXTENSION_2                                     \
	if (test_forced_mutex_timeout == ((TEST_FLAG)1)) {           \
		test_forced_mutex_timeout = ((TEST_FLAG)0);          \
		_tx_thread_wait_abort(mutex_ptr->tx_mutex_suspension_list); \
	}

#define TX_MUTEX_PRIORITY_CHANGE_EXTENSION                           \
	if (threadx_mutex_suspension_priority_test == ((TEST_FLAG)1)) { \
		threadx_mutex_suspension_priority_test = ((TEST_FLAG)0); \
		suspend_lowest_priority();                           \
	}

#ifndef TX_TIMER_PROCESS_IN_ISR
#define TX_TIMER_INITIALIZE_EXTENSION(a)                             \
	if (threadx_delete_timer_thread == ((TEST_FLAG)1)) {         \
		threadx_delete_timer_thread = ((TEST_FLAG)0);        \
		delete_timer_thread();                               \
		(a) = ((UINT)1);                                     \
	}
#endif

#define TX_THREAD_STACK_ANALYZE_EXTENSION                            \
	if (test_stack_analyze_flag == ((TEST_FLAG)1)) {             \
		thread_ptr->tx_thread_id = ((TEST_FLAG)0);           \
		test_stack_analyze_flag = ((TEST_FLAG)0);            \
	} else if (test_stack_analyze_flag == ((TEST_FLAG)2)) {      \
		stack_ptr = thread_ptr->tx_thread_stack_start;       \
		test_stack_analyze_flag = ((TEST_FLAG)0);            \
	} else if (test_stack_analyze_flag == ((TEST_FLAG)3)) {      \
		*stack_ptr = TX_STACK_FILL;                          \
		test_stack_analyze_flag = ((TEST_FLAG)0);            \
	} else {                                                     \
		test_stack_analyze_flag = ((TEST_FLAG)0);            \
	}

#define TX_INITIALIZE_KERNEL_ENTER_EXTENSION                         \
	if (test_initialize_flag == ((TEST_FLAG)1)) {                \
		test_initialize_flag = ((TEST_FLAG)0);               \
		return;                                              \
	}
#endif

#ifdef TX_THREAD_INIT
CHAR _tx_version_id[] = "(c) 2026-present Eclipse ThreadX contributors.  *  ThreadX RH850/GNU Version 6.5.2.202603b *";
#else
extern CHAR _tx_version_id[];
#endif

#endif
