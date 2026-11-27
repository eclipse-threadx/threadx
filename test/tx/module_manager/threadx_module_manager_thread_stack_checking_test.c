/***************************************************************************/
/* Copyright (c) 2026 Eclipse ThreadX contributors                         */
/*                                                                         */
/* This program and the accompanying materials are made available under    */
/* the terms of the MIT License which is available at                      */
/* https://opensource.org/licenses/MIT.                                    */
/*                                                                         */
/* SPDX-License-Identifier: MIT                                            */
/***************************************************************************/
// Portions of this file were generated with AI assistance.

/**************************************************************************/
/**************************************************************************/
/**                                                                       */
/** ThreadX Test                                                          */
/**                                                                       */
/**   Module Manager thread stack checking test                           */
/**                                                                       */
/**************************************************************************/
/**************************************************************************/

#include <stdio.h>

#include "threadx_module_manager_host_test_port.h"

#include "tx_thread.h"
#include "tx_timer.h"
#include "tx_queue.h"
#include "tx_semaphore.h"
#include "tx_mutex.h"
#include "tx_event_flags.h"
#include "tx_block_pool.h"
#include "tx_byte_pool.h"
#include "txm_module.h"
#include "txm_module_manager_util.h"

/* Stand-in interrupt locks for the test port.  */
unsigned int test_interrupt_disable_depth;
unsigned int test_interrupt_disable_max_depth;
unsigned int test_interrupt_restore_underflows;

/* Manager globals.  */
TX_BYTE_POOL    _txm_module_manager_object_pool;
UINT            _txm_module_manager_object_pool_created;
TX_MUTEX        _txm_module_manager_mutex;
TX_THREAD       *_tx_thread_current_ptr;

/* Kernel created object lists.  */
TX_BLOCK_POOL           *_tx_block_pool_created_ptr;
ULONG                   _tx_block_pool_created_count;
TX_BYTE_POOL            *_tx_byte_pool_created_ptr;
ULONG                   _tx_byte_pool_created_count;
TX_EVENT_FLAGS_GROUP    *_tx_event_flags_created_ptr;
ULONG                   _tx_event_flags_created_count;
TX_MUTEX                *_tx_mutex_created_ptr;
ULONG                   _tx_mutex_created_count;
TX_QUEUE                *_tx_queue_created_ptr;
ULONG                   _tx_queue_created_count;
TX_SEMAPHORE            *_tx_semaphore_created_ptr;
ULONG                   _tx_semaphore_created_count;
TX_THREAD               *_tx_thread_created_ptr;
ULONG                   _tx_thread_created_count;
TX_TIMER                *_tx_timer_created_ptr;
ULONG                   _tx_timer_created_count;

TX_THREAD               *_tx_thread_execute_ptr;
volatile UINT           _tx_thread_preempt_disable;
volatile ULONG          _tx_thread_system_state;
TX_THREAD               _tx_timer_thread;

VOID _tx_thread_system_preempt_check(VOID)
{
}

VOID _tx_thread_system_resume(TX_THREAD *thread_ptr)
{
    (VOID) thread_ptr;
}

VOID _tx_thread_timeout(ULONG timeout_input)
{
    (VOID) timeout_input;
}

ALIGN_TYPE _txm_module_manager_kernel_dispatch(ULONG kernel_request, ALIGN_TYPE param_1, ALIGN_TYPE param_2, ALIGN_TYPE param_3)
{
    (VOID) kernel_request;
    (VOID) param_1;
    (VOID) param_2;
    (VOID) param_3;
    return(0);
}

ALIGN_TYPE _txm_module_manager_user_mode_entry(ULONG kernel_request, ALIGN_TYPE param_1, ALIGN_TYPE param_2, ALIGN_TYPE param_3)
{
    (VOID) kernel_request;
    (VOID) param_1;
    (VOID) param_2;
    (VOID) param_3;
    return(0);
}

#define TEST_POOL_BYTES 16384

static union
{
    ALIGN_TYPE  test_pool_alignment;
    UCHAR       test_pool_bytes[TEST_POOL_BYTES];
} test_pool_arena;

static ULONG test_pool_offset;

static ULONG test_checks;
static UINT  test_failures;
static UINT  test_stack_error_detected;

VOID _tx_thread_stack_error_handler(TX_THREAD *thread_ptr)
{
    (VOID) thread_ptr;
    test_stack_error_detected++;
}

static VOID test_expect(const char *description, ULONG actual, ULONG expected)
{
    test_checks++;
    if (actual != expected)
    {
        printf("FAIL: %s (expected %lu, got %lu)\n", description,
               (unsigned long) expected, (unsigned long) actual);
        test_failures++;
    }
}

/* Check that application threads in the same kernel still use their one stack.  */
static VOID check_single_stack(VOID)
{
    TX_THREAD thread;
    TX_THREAD *thread_ptr = &thread;
    ULONG stack[32];
    UINT index;

    TX_MEMSET(thread_ptr, 0, sizeof(TX_THREAD));
    for (index = 0U; index < 32U; index++)
    {
        stack[index] = TX_STACK_FILL;
    }
    thread_ptr -> tx_thread_stack_start = stack;
    thread_ptr -> tx_thread_stack_end = ((UCHAR *) &stack[31]) - 1;
    thread_ptr -> tx_thread_stack_highest_ptr = &stack[30];
    thread_ptr -> tx_thread_stack_ptr = &stack[28];

    test_stack_error_detected = 0U;
    TX_THREAD_STACK_CHECK(TX_NULL);
    TX_THREAD_STACK_CHECK(thread_ptr);
    test_expect("null and uncreated threads ignored", test_stack_error_detected, 0U);

    thread_ptr -> tx_thread_id = TX_THREAD_ID;
    TX_THREAD_STACK_CHECK(thread_ptr);
    test_expect("application stack checked", test_stack_error_detected, 0U);
    test_expect("application watermark updated",
                (ULONG) (thread_ptr -> tx_thread_stack_highest_ptr == &stack[28]), TX_TRUE);

    /* Search with a fill-only range, then report invalid saved and highest pointers.  */
    _tx_thread_stack_analyze(thread_ptr);
    thread_ptr -> tx_thread_stack_ptr = &stack[32];
    TX_THREAD_STACK_CHECK(thread_ptr);
    test_expect("application saved pointer outside its stack reported", test_stack_error_detected, 1U);

    thread_ptr -> tx_thread_stack_ptr = &stack[28];
    thread_ptr -> tx_thread_stack_highest_ptr = TX_NULL;
    TX_THREAD_STACK_CHECK(thread_ptr);
    test_expect("application invalid watermark reported", test_stack_error_detected, 2U);
}

UINT _txe_byte_allocate(TX_BYTE_POOL *pool_ptr, VOID **memory_ptr, ULONG memory_size, ULONG wait_option)
{
    ULONG rounded_size;
    UCHAR *block_start;

    (VOID) pool_ptr;
    (VOID) wait_option;

    rounded_size = (memory_size + (sizeof(ALIGN_TYPE) - 1U)) & (~(sizeof(ALIGN_TYPE) - 1U));
    if ((test_pool_offset + rounded_size) > ((ULONG) TEST_POOL_BYTES))
    {
        return(TX_NO_MEMORY);
    }

    block_start = &test_pool_arena.test_pool_bytes[test_pool_offset];
    test_pool_offset += rounded_size;
    *memory_ptr = (VOID *) block_start;

    return(TX_SUCCESS);
}

UINT _txe_byte_release(VOID *memory_ptr)
{
    (VOID) memory_ptr;
    return(TX_SUCCESS);
}

UINT _txe_mutex_get(TX_MUTEX *mutex_ptr, ULONG wait_option)
{
    (VOID) mutex_ptr;
    (VOID) wait_option;
    return(TX_SUCCESS);
}

UINT _txe_mutex_put(TX_MUTEX *mutex_ptr)
{
    (VOID) mutex_ptr;
    return(TX_SUCCESS);
}

UINT _txm_module_manager_inside_data_check(TXM_MODULE_INSTANCE *module_instance, ALIGN_TYPE obj_ptr,
                                            ULONG obj_size, UINT write_request)
{
    (VOID) module_instance;
    (VOID) obj_ptr;
    (VOID) obj_size;
    (VOID) write_request;
    return(TX_FALSE);
}

UINT _txm_module_manager_outside_data_check(TXM_MODULE_INSTANCE *module_instance, ALIGN_TYPE obj_ptr,
                                             ULONG obj_size)
{
    (VOID) module_instance;
    (VOID) obj_ptr;
    (VOID) obj_size;
    return(TX_TRUE);
}

VOID _txm_module_manager_alignment_adjust(TXM_MODULE_PREAMBLE *module_preamble, ULONG *code_size,
                                           ULONG *code_alignment, ULONG *data_size, ULONG *data_alignment)
{
    (VOID) module_preamble;
    (VOID) code_size;
    (VOID) code_alignment;
    (VOID) data_size;
    (VOID) data_alignment;
}

VOID _txm_module_manager_thread_stack_build(TX_THREAD *thread_ptr,
                                            VOID (*shell_function)(TX_THREAD *, TXM_MODULE_INSTANCE *))
{
    (VOID) shell_function;
    thread_ptr -> tx_thread_stack_ptr = (VOID *) (((ALIGN_TYPE) thread_ptr -> tx_thread_stack_end) + 1U - 64U);
}

static VOID dummy_shell(TX_THREAD *thread_ptr, TXM_MODULE_INSTANCE *module_instance)
{
    (VOID) thread_ptr;
    (VOID) module_instance;
}

static VOID dummy_entry(ULONG id)
{
    (VOID) id;
}

UINT _txe_thread_create(TX_THREAD *thread_ptr, TX_NAME_CONST CHAR *name_ptr,
                        VOID (*entry_function)(ULONG id), ULONG entry_input,
                        VOID *stack_start, ULONG stack_size,
                        UINT priority, UINT preempt_threshold,
                        ULONG time_slice, UINT auto_start, UINT thread_control_block_size)
{
    (VOID) entry_function;
    (VOID) entry_input;
    (VOID) priority;
    (VOID) preempt_threshold;
    (VOID) time_slice;
    (VOID) auto_start;
    (VOID) thread_control_block_size;

    thread_ptr -> tx_thread_id = TX_THREAD_ID;
    thread_ptr -> tx_thread_name = (CHAR *) name_ptr;
    thread_ptr -> tx_thread_stack_start = stack_start;
    thread_ptr -> tx_thread_stack_end = (VOID *) (((ALIGN_TYPE) stack_start) + stack_size - 1);
    thread_ptr -> tx_thread_stack_size = stack_size;
    thread_ptr -> tx_thread_stack_ptr = (VOID *) (((ALIGN_TYPE) stack_start) + stack_size - 64);
    thread_ptr -> tx_thread_stack_highest_ptr = thread_ptr -> tx_thread_stack_ptr;

    return(TX_SUCCESS);
}

int main(void)
{
    TXM_MODULE_INSTANCE module_instance;
    TX_THREAD thread;
    TX_THREAD *thread_ptr = &thread;
    UINT status;
    ULONG user_stack[512];
    ULONG kernel_stack_size;
    ULONG *guard_word_ptr;
    VOID *saved_user_stack_start;
    VOID *saved_user_stack_end;
    VOID *saved_user_stack_highest_ptr;
    ULONG *user_guard_ptr;
    VOID *saved_kernel_pointer;

    setvbuf(stdout, TX_NULL, _IONBF, 0);

    test_failures = 0U;
    test_checks = 0U;
    test_pool_offset = 0U;
    test_stack_error_detected = 0U;

    _txm_module_manager_object_pool.tx_byte_pool_id = TX_BYTE_POOL_ID;
    _txm_module_manager_object_pool.tx_byte_pool_start = test_pool_arena.test_pool_bytes;
    _txm_module_manager_object_pool.tx_byte_pool_size = TEST_POOL_BYTES;
    _txm_module_manager_object_pool.tx_byte_pool_available = TEST_POOL_BYTES;
    _txm_module_manager_object_pool_created = TX_TRUE;

    TX_MEMSET(&module_instance, 0, sizeof(TXM_MODULE_INSTANCE));
    module_instance.txm_module_instance_property_flags = TXM_MODULE_USER_MODE | TXM_MODULE_MEMORY_PROTECTION;

    /* Start with a different value so the test observes stack initialization.  */
    TX_MEMSET(user_stack, 0, sizeof(user_stack));

    /* Create module thread with TX_DONT_START.  */
    status = _txm_module_manager_thread_create(thread_ptr, "test_thread",
                                               dummy_shell, dummy_entry, 0,
                                               user_stack, sizeof(user_stack),
                                               1, 1, TX_NO_TIME_SLICE, TX_DONT_START,
                                               sizeof(TX_THREAD), &module_instance);

    test_expect("thread create status success", (ULONG) status, (ULONG) TX_SUCCESS);
    test_expect("kernel stack allocated", (ULONG) (thread_ptr -> tx_thread_module_kernel_stack_start != TX_NULL), (ULONG) TX_TRUE);

    if (thread_ptr -> tx_thread_module_kernel_stack_start == TX_NULL)
    {
        printf("ERROR: Kernel stack allocation failed\n");
        return(1);
    }

    saved_user_stack_start = thread_ptr -> tx_thread_stack_start;
    saved_user_stack_end = thread_ptr -> tx_thread_stack_end;
    saved_user_stack_highest_ptr = thread_ptr -> tx_thread_stack_highest_ptr;
    user_guard_ptr = (ULONG *) (((UCHAR *) saved_user_stack_end) + 1);

    /* Inspect the guard words left by thread creation.  */
    test_expect("module lower guard initialized", *(ULONG *) saved_user_stack_start, TX_STACK_FILL);
    test_expect("module upper guard initialized", *user_guard_ptr, TX_STACK_FILL);
    kernel_stack_size = thread_ptr -> tx_thread_module_kernel_stack_size;
    test_expect("kernel stack size valid", (ULONG) (kernel_stack_size > 0), (ULONG) TX_TRUE);
    guard_word_ptr = (ULONG *) thread_ptr -> tx_thread_module_kernel_stack_end;
    test_expect("kernel lower guard initialized", *(ULONG *) thread_ptr -> tx_thread_module_kernel_stack_start,
                TX_STACK_FILL);
    test_expect("kernel upper guard initialized", *guard_word_ptr, TX_STACK_FILL);

    test_expect("kernel stack highest ptr initialized to end",
                (ULONG) (ALIGN_TYPE) thread_ptr -> tx_thread_module_kernel_stack_highest_ptr,
                (ULONG) (ALIGN_TYPE) thread_ptr -> tx_thread_module_kernel_stack_end);

    /* 1. Test stack checking in user mode.  */
    test_stack_error_detected = 0;
    TX_THREAD_STACK_CHECK(thread_ptr);
    test_expect("stack check in user mode succeeds without error", (ULONG) test_stack_error_detected, 0);

    /* The kernel is active before the scheduler replaces the saved module pointer.  */
    thread_ptr -> tx_thread_module_current_user_mode = 0U;
    thread_ptr -> tx_thread_stack_start = thread_ptr -> tx_thread_module_kernel_stack_start;
    thread_ptr -> tx_thread_stack_end = thread_ptr -> tx_thread_module_kernel_stack_end;
    saved_kernel_pointer = thread_ptr -> tx_thread_module_kernel_stack_highest_ptr;

    TX_THREAD_STACK_CHECK(thread_ptr);
    test_expect("saved module pointer while kernel is active", test_stack_error_detected, 0U);
    test_expect("saved module pointer left kernel watermark intact",
                (ULONG) (thread_ptr -> tx_thread_module_kernel_stack_highest_ptr == saved_kernel_pointer), TX_TRUE);

    /* Save a kernel context and then restore the module's stack metadata.  */
    saved_kernel_pointer = guard_word_ptr - 32;
    thread_ptr -> tx_thread_stack_ptr = saved_kernel_pointer;
    thread_ptr -> tx_thread_module_current_user_mode = thread_ptr -> tx_thread_module_user_mode;
    thread_ptr -> tx_thread_stack_start = saved_user_stack_start;
    thread_ptr -> tx_thread_stack_end = saved_user_stack_end;
    TX_THREAD_STACK_CHECK(thread_ptr);
    test_expect("saved kernel pointer while module is active", test_stack_error_detected, 0U);
    test_expect("saved kernel pointer updates kernel watermark",
                (ULONG) (thread_ptr -> tx_thread_module_kernel_stack_highest_ptr == saved_kernel_pointer), TX_TRUE);
    test_expect("saved kernel pointer leaves module watermark intact",
                (ULONG) (thread_ptr -> tx_thread_stack_highest_ptr == saved_user_stack_highest_ptr), TX_TRUE);

    /* 3. Test return to user mode.  */
    thread_ptr -> tx_thread_module_current_user_mode = thread_ptr -> tx_thread_module_user_mode;
    thread_ptr -> tx_thread_stack_start = saved_user_stack_start;
    thread_ptr -> tx_thread_stack_end = saved_user_stack_end;
    thread_ptr -> tx_thread_stack_ptr = saved_user_stack_highest_ptr;

    TX_THREAD_STACK_CHECK(thread_ptr);
    test_expect("stack check after returning to user mode succeeds without spurious error", (ULONG) test_stack_error_detected, 0);

    /* 4. Test stack error detection on corrupted kernel guard pattern.  */
    thread_ptr -> tx_thread_module_current_user_mode = 0;
    thread_ptr -> tx_thread_stack_start = thread_ptr -> tx_thread_module_kernel_stack_start;
    thread_ptr -> tx_thread_stack_end = thread_ptr -> tx_thread_module_kernel_stack_end;
    thread_ptr -> tx_thread_stack_ptr = (VOID *) (((ALIGN_TYPE) thread_ptr -> tx_thread_module_kernel_stack_end) - 128);

    test_stack_error_detected = 0;
    *guard_word_ptr = 0x12345678;
    TX_THREAD_STACK_CHECK(thread_ptr);
    test_expect("corrupted kernel guard detected", (ULONG) test_stack_error_detected, 1);
    *guard_word_ptr = TX_STACK_FILL;

    /* 5. Test stack error detection on kernel stack overflow (SP below kernel stack start).  */
    thread_ptr -> tx_thread_module_current_user_mode = 0;
    test_stack_error_detected = 0;
    thread_ptr -> tx_thread_stack_ptr = (VOID *) (((ALIGN_TYPE) thread_ptr -> tx_thread_module_kernel_stack_start) - 4);
    TX_THREAD_STACK_CHECK(thread_ptr);
    test_expect("kernel stack overflow detected", (ULONG) test_stack_error_detected, 1);

    /* 6. Test stack error detection on user stack guard pattern corruption.  */
    thread_ptr -> tx_thread_module_current_user_mode = thread_ptr -> tx_thread_module_user_mode;
    thread_ptr -> tx_thread_stack_start = saved_user_stack_start;
    thread_ptr -> tx_thread_stack_end = saved_user_stack_end;
    thread_ptr -> tx_thread_stack_ptr = saved_user_stack_highest_ptr;

    test_stack_error_detected = 0;
    *user_guard_ptr = 0xDEADBEEF;
    TX_THREAD_STACK_CHECK(thread_ptr);
    test_expect("corrupted user guard detected", (ULONG) test_stack_error_detected, 1);
    *user_guard_ptr = TX_STACK_FILL;

    /* 7. Test stack error detection on user stack overflow.  */
    thread_ptr -> tx_thread_module_current_user_mode = thread_ptr -> tx_thread_module_user_mode;
    test_stack_error_detected = 0;
    thread_ptr -> tx_thread_stack_ptr = (VOID *) (((ALIGN_TYPE) saved_user_stack_start) - 4);
    TX_THREAD_STACK_CHECK(thread_ptr);
    test_expect("user stack overflow detected", (ULONG) test_stack_error_detected, 1);

    /* Find used words on both stacks when the saved pointer is still on the module stack.  */
    thread_ptr -> tx_thread_stack_ptr = saved_user_stack_highest_ptr;
    thread_ptr -> tx_thread_stack_highest_ptr = saved_user_stack_highest_ptr;
    thread_ptr -> tx_thread_module_kernel_stack_highest_ptr = saved_kernel_pointer;
    ((ULONG *) saved_user_stack_highest_ptr)[-1] = 0UL;
    ((ULONG *) saved_kernel_pointer)[-1] = 0UL;
    test_stack_error_detected = 0U;
    TX_THREAD_STACK_CHECK(thread_ptr);
    test_expect("scan checked both stacks without error", test_stack_error_detected, 0U);
    test_expect("module scan lowered its watermark",
                (ULONG) (thread_ptr -> tx_thread_stack_highest_ptr == ((ULONG *) saved_user_stack_highest_ptr - 1)),
                TX_TRUE);
    test_expect("kernel scan lowered its watermark",
                (ULONG) (thread_ptr -> tx_thread_module_kernel_stack_highest_ptr == ((ULONG *) saved_kernel_pointer - 1)),
                TX_TRUE);

    /* 10. Test kernel stack check under TXM_MODULE_KERNEL_STACK_MAINTENANCE_DISABLE scenario.
       When stack maintenance is disabled, stack_start is not updated by assembly scheduler,
       so stack_start remains pointing to user stack start while current_user_mode is 0.  */
    thread_ptr -> tx_thread_module_current_user_mode = 0;
    thread_ptr -> tx_thread_stack_start = saved_user_stack_start;
    thread_ptr -> tx_thread_stack_end = saved_user_stack_end;
    thread_ptr -> tx_thread_stack_ptr = (VOID *) (((ALIGN_TYPE) thread_ptr -> tx_thread_module_kernel_stack_end) - 64);
    test_stack_error_detected = 0;
    TX_THREAD_STACK_CHECK(thread_ptr);
    test_expect("kernel stack check succeeds when stack maintenance is disabled", (ULONG) test_stack_error_detected, 0);

    /* 11. Test boundary condition where highest_ptr equals stack_start (under-read guard).  */
    thread_ptr -> tx_thread_module_current_user_mode = 0;
    thread_ptr -> tx_thread_stack_start = thread_ptr -> tx_thread_module_kernel_stack_start;
    thread_ptr -> tx_thread_stack_end = thread_ptr -> tx_thread_module_kernel_stack_end;
    thread_ptr -> tx_thread_stack_ptr = thread_ptr -> tx_thread_module_kernel_stack_start;
    thread_ptr -> tx_thread_module_kernel_stack_highest_ptr = thread_ptr -> tx_thread_module_kernel_stack_start;
    test_stack_error_detected = 0;
    TX_THREAD_STACK_CHECK(thread_ptr);
    test_expect("boundary condition highest_ptr == stack_start handled safely", (ULONG) test_stack_error_detected, 0);

    *(ULONG *) thread_ptr -> tx_thread_module_kernel_stack_start = 0UL;
    TX_THREAD_STACK_CHECK(thread_ptr);
    test_expect("kernel lower guard checked", test_stack_error_detected, 1U);
    *(ULONG *) thread_ptr -> tx_thread_module_kernel_stack_start = TX_STACK_FILL;

    check_single_stack();
    test_expect("interrupt restores balanced", test_interrupt_disable_depth, 0U);
    test_expect("no interrupt restore underflow", test_interrupt_restore_underflows, 0U);

    if (test_failures == 0U)
    {
        printf("SUCCESS! %lu expectations\n", (unsigned long) test_checks);
        return(0);
    }

    printf("ERROR: %u expectation(s) failed of %lu\n", test_failures, (unsigned long) test_checks);
    return(1);
}
