/***************************************************************************/
/* Copyright (c) 2026 Eclipse ThreadX contributors                         */
/*                                                                         */
/* This program and the accompanying materials are made available under    */
/* the terms of the MIT License which is available at                      */
/* https://opensource.org/licenses/MIT.                                    */
/*                                                                         */
/* SPDX-License-Identifier: MIT                                            */
/***************************************************************************/

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

/* Prototype for direct stack analyze call.  */
VOID _tx_thread_stack_analyze(TX_THREAD *thread_ptr);

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
    thread_ptr -> tx_thread_stack_ptr = (VOID *) (((ALIGN_TYPE) thread_ptr -> tx_thread_stack_end) - 64);
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
    ULONG *stack_scan;

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

    /* Initialize user stack with fill pattern.  */
    TX_MEMSET(user_stack, (int) 0xEF, sizeof(user_stack));
    *(ULONG *) user_stack = TX_STACK_FILL;

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

    /* Ensure entire user stack and guard pattern hold TX_STACK_FILL.  */
    stack_scan = (ULONG *) saved_user_stack_start;
    while (stack_scan <= (ULONG *) saved_user_stack_end)
    {
        *stack_scan = TX_STACK_FILL;
        stack_scan++;
    }
    *user_guard_ptr = TX_STACK_FILL;

    /* Verify kernel stack end is properly aligned and within the allocated buffer.  */
    kernel_stack_size = thread_ptr -> tx_thread_module_kernel_stack_size;
    test_expect("kernel stack size valid", (ULONG) (kernel_stack_size > 0), (ULONG) TX_TRUE);

    guard_word_ptr = (ULONG *) (((ALIGN_TYPE) thread_ptr -> tx_thread_module_kernel_stack_end) + sizeof(ULONG));

    /* Ensure entire kernel stack and guard pattern hold TX_STACK_FILL.  */
    stack_scan = (ULONG *) thread_ptr -> tx_thread_module_kernel_stack_start;
    while (stack_scan <= (ULONG *) thread_ptr -> tx_thread_module_kernel_stack_end)
    {
        *stack_scan = TX_STACK_FILL;
        stack_scan++;
    }
    *guard_word_ptr = TX_STACK_FILL;
    test_expect("kernel stack guard pattern initialized", *guard_word_ptr, (ULONG) TX_STACK_FILL);

    test_expect("kernel stack highest ptr initialized to end",
                (ULONG) (ALIGN_TYPE) thread_ptr -> tx_thread_module_kernel_stack_highest_ptr,
                (ULONG) (ALIGN_TYPE) thread_ptr -> tx_thread_module_kernel_stack_end);

    /* 1. Test stack checking in user mode.  */
    test_stack_error_detected = 0;
    TX_THREAD_STACK_CHECK(thread_ptr);
    test_expect("stack check in user mode succeeds without error", (ULONG) test_stack_error_detected, 0);

    /* 2. Test transition to kernel mode.  */
    thread_ptr -> tx_thread_module_current_user_mode = 0;
    thread_ptr -> tx_thread_stack_start = thread_ptr -> tx_thread_module_kernel_stack_start;
    thread_ptr -> tx_thread_stack_end = thread_ptr -> tx_thread_module_kernel_stack_end;
    thread_ptr -> tx_thread_stack_ptr = (VOID *) (((ALIGN_TYPE) thread_ptr -> tx_thread_module_kernel_stack_end) - 128);

    TX_THREAD_STACK_CHECK(thread_ptr);
    test_expect("stack check in kernel mode succeeds without error", (ULONG) test_stack_error_detected, 0);
    test_expect("kernel stack highest ptr updated to kernel stack ptr",
                (ULONG) (ALIGN_TYPE) thread_ptr -> tx_thread_module_kernel_stack_highest_ptr,
                (ULONG) (ALIGN_TYPE) thread_ptr -> tx_thread_stack_ptr);

    /* Verify user stack highest pointer was preserved and not corrupted.  */
    test_expect("user stack highest ptr not overwritten by kernel stack address",
                (ULONG) (ALIGN_TYPE) thread_ptr -> tx_thread_stack_highest_ptr,
                (ULONG) (ALIGN_TYPE) saved_user_stack_highest_ptr);

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

    /* 8. Test _tx_thread_stack_analyze on user stack.  */
    thread_ptr -> tx_thread_module_current_user_mode = thread_ptr -> tx_thread_module_user_mode;
    thread_ptr -> tx_thread_stack_start = saved_user_stack_start;
    thread_ptr -> tx_thread_stack_end = saved_user_stack_end;
    thread_ptr -> tx_thread_stack_ptr = saved_user_stack_highest_ptr;
    thread_ptr -> tx_thread_stack_highest_ptr = saved_user_stack_highest_ptr;
    _tx_thread_stack_analyze(thread_ptr);
    test_expect("user stack analyze preserves highest ptr validity",
                (ULONG) (thread_ptr -> tx_thread_stack_highest_ptr >= thread_ptr -> tx_thread_stack_start), (ULONG) TX_TRUE);

    /* 9. Test _tx_thread_stack_analyze on kernel stack.  */
    thread_ptr -> tx_thread_module_current_user_mode = 0;
    thread_ptr -> tx_thread_stack_start = thread_ptr -> tx_thread_module_kernel_stack_start;
    thread_ptr -> tx_thread_stack_end = thread_ptr -> tx_thread_module_kernel_stack_end;
    thread_ptr -> tx_thread_stack_ptr = (VOID *) (((ALIGN_TYPE) thread_ptr -> tx_thread_module_kernel_stack_end) - 128);
    thread_ptr -> tx_thread_module_kernel_stack_highest_ptr = thread_ptr -> tx_thread_stack_ptr;
    _tx_thread_stack_analyze(thread_ptr);
    test_expect("kernel stack analyze preserves highest ptr validity",
                (ULONG) (thread_ptr -> tx_thread_module_kernel_stack_highest_ptr >= thread_ptr -> tx_thread_module_kernel_stack_start), (ULONG) TX_TRUE);

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

    if (test_failures == 0U)
    {
        printf("SUCCESS! %lu expectations\n", (unsigned long) test_checks);
        return(0);
    }

    printf("ERROR: %u expectation(s) failed of %lu\n", test_failures, (unsigned long) test_checks);
    return(1);
}
