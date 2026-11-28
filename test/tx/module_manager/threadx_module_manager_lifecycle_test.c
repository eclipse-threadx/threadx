/***************************************************************************
 * Copyright (c) 2026-present Eclipse ThreadX contributors
 *
 * This program and the accompanying materials are made available under the
 * terms of the MIT License which is available at
 * https://opensource.org/licenses/MIT.
 *
 * SPDX-License-Identifier: MIT
 **************************************************************************/

#include <stdio.h>
#include <string.h>

#include "threadx_module_manager_host_test_port.h"

#include "tx_api.h"
#include "tx_initialize.h"
#include "tx_thread.h"
#include "tx_trace.h"
#include "tx_timer.h"
#include "tx_queue.h"
#include "tx_semaphore.h"
#include "tx_mutex.h"
#include "tx_event_flags.h"
#include "tx_block_pool.h"
#include "tx_byte_pool.h"
#include "txm_module.h"
#include "txm_module_manager_util.h"

/* Interrupt lock accounting expected by the host port shim.  */
unsigned int    test_interrupt_disable_depth;
unsigned int    test_interrupt_disable_max_depth;
unsigned int    test_interrupt_restore_underflows;

unsigned int    test_get_ipsr_value(void)
{
    return((unsigned int) _tx_thread_system_state);
}

/* Kernel state and created object lists used across lifecycle APIs.  */
volatile ULONG          _tx_thread_system_state;
volatile UINT           _tx_thread_preempt_disable;
TX_THREAD               *_tx_thread_current_ptr;
TX_THREAD               *_tx_thread_execute_ptr;
TX_THREAD               _tx_timer_thread;
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

#ifdef TX_ENABLE_EVENT_TRACE
TX_TRACE_HEADER         *_tx_trace_header_ptr;
TX_TRACE_BUFFER_ENTRY   *_tx_trace_buffer_start_ptr;
TX_TRACE_BUFFER_ENTRY   *_tx_trace_buffer_end_ptr;
TX_TRACE_BUFFER_ENTRY   *_tx_trace_buffer_current_ptr;
ULONG                   _tx_trace_event_enable_bits;
ULONG                   _tx_trace_simulated_time;
VOID                    (*_tx_trace_full_notify_function)(VOID *buffer);

VOID  _tx_trace_object_register(UCHAR object_type, VOID *object_ptr, TX_NAME_CONST CHAR *object_name,
                                ULONG parameter_1, ULONG parameter_2)
{
    (VOID) object_type;
    (VOID) object_ptr;
    (VOID) object_name;
    (VOID) parameter_1;
    (VOID) parameter_2;
}

UINT  _tx_trace_disable(VOID)
{
    _tx_trace_buffer_current_ptr = TX_NULL;
    return(TX_SUCCESS);
}
#endif

/* Accounting and failure injection controls for kernel/port stubs.  */
static UINT     test_failures;
static ULONG    test_checks;

static UINT     stub_byte_pool_create_status;
static UINT     stub_queue_create_status;
static UINT     stub_obj_alloc_fail_on_call;
static UINT     stub_obj_alloc_call_count;
static UINT     stub_stop_hang;

static ULONG    stub_queue_create_calls;
static ULONG    stub_queue_delete_calls;
static ULONG    stub_thread_terminate_calls;
static ULONG    stub_thread_delete_calls;
static ULONG    stub_thread_resume_calls;
static ULONG    stub_stack_build_calls;
static ULONG    stub_byte_release_calls;
static ULONG    stub_block_pool_delete_calls;
static ULONG    stub_byte_pool_delete_calls;
static ULONG    stub_ev_delete_calls;
static ULONG    stub_mutex_delete_calls;
static ULONG    stub_sem_delete_calls;
static ULONG    stub_timer_delete_calls;
static ULONG    stub_sleep_calls;

static TXM_MODULE_INSTANCE  *stub_stop_watch_module;

/* Tracked pool allocations backed directly by each TX_BYTE_POOL buffer.  */
#define STUB_POOL_SLOTS     64U

typedef struct STUB_ALLOC_STRUCT
{
    VOID    *ptr;
    ULONG   size;
    UINT    in_use;
} STUB_ALLOC;

static STUB_ALLOC   stub_allocs[STUB_POOL_SLOTS];

static VOID  test_expect(const char *description, ULONG actual, ULONG expected)
{
    test_checks++;
    if (actual != expected)
    {
        printf("FAIL: %s (expected %lu, got %lu)\n", description,
               (unsigned long) expected, (unsigned long) actual);
        test_failures++;
    }
}

static VOID  reset_stub_counters(VOID)
{
    stub_byte_pool_create_status = TX_SUCCESS;
    stub_queue_create_status = TX_SUCCESS;
    stub_obj_alloc_fail_on_call = 0U;
    stub_obj_alloc_call_count = 0U;
    stub_stop_hang = 0U;
    stub_queue_create_calls = 0U;
    stub_queue_delete_calls = 0U;
    stub_thread_terminate_calls = 0U;
    stub_thread_delete_calls = 0U;
    stub_thread_resume_calls = 0U;
    stub_stack_build_calls = 0U;
    stub_byte_release_calls = 0U;
    stub_block_pool_delete_calls = 0U;
    stub_byte_pool_delete_calls = 0U;
    stub_ev_delete_calls = 0U;
    stub_mutex_delete_calls = 0U;
    stub_sem_delete_calls = 0U;
    stub_timer_delete_calls = 0U;
    stub_sleep_calls = 0U;
    stub_stop_watch_module = TX_NULL;
}

static VOID *arena_alloc(TX_BYTE_POOL *pool_ptr, ULONG size)
{
ULONG   rounded = (size + (ULONG)(sizeof(ALIGN_TYPE) - 1U)) & ~((ULONG)(sizeof(ALIGN_TYPE) - 1U));
ULONG   used;
UINT    i;
VOID    *p;

    if ((pool_ptr == TX_NULL) || (rounded == 0UL) || (rounded > pool_ptr -> tx_byte_pool_available))
    {
        return(TX_NULL);
    }
    used = pool_ptr -> tx_byte_pool_size - pool_ptr -> tx_byte_pool_available;
    p = (VOID *) (pool_ptr -> tx_byte_pool_start + used);
    pool_ptr -> tx_byte_pool_available -= rounded;
    for (i = 0; i < STUB_POOL_SLOTS; i++)
    {
        if (!stub_allocs[i].in_use)
        {
            stub_allocs[i].ptr = p;
            stub_allocs[i].size = rounded;
            stub_allocs[i].in_use = 1U;
            break;
        }
    }
    return(p);
}

static UINT arena_release(VOID *ptr)
{
UINT    i;

    for (i = 0; i < STUB_POOL_SLOTS; i++)
    {
        if (stub_allocs[i].in_use && (stub_allocs[i].ptr == ptr))
        {
            stub_allocs[i].in_use = 0U;
            return(TX_SUCCESS);
        }
    }
    return(TX_PTR_ERROR);
}

static UINT arena_active_allocs(VOID)
{
UINT    i;
UINT    count = 0U;

    for (i = 0; i < STUB_POOL_SLOTS; i++)
    {
        if (stub_allocs[i].in_use)
        {
            count++;
        }
    }
    return(count);
}

/* Kernel and port stand-in functions required by lifecycle files under test.  */
UINT  _tx_byte_pool_create(TX_BYTE_POOL *pool_ptr, CHAR *name_ptr, VOID *pool_start, ULONG pool_size)
{
    if (stub_byte_pool_create_status != TX_SUCCESS)
    {
        return(stub_byte_pool_create_status);
    }
    if ((pool_ptr == TX_NULL) || (pool_start == TX_NULL))
    {
        return(TX_PTR_ERROR);
    }
    if (pool_size < 128UL)
    {
        return(TX_SIZE_ERROR);
    }
    memset((VOID *) pool_ptr, 0, sizeof(TX_BYTE_POOL));
    pool_ptr -> tx_byte_pool_id = TX_BYTE_POOL_ID;
    pool_ptr -> tx_byte_pool_name = name_ptr;
    pool_ptr -> tx_byte_pool_start = (UCHAR *) pool_start;
    pool_ptr -> tx_byte_pool_size = pool_size;
    pool_ptr -> tx_byte_pool_available = pool_size;
    return(TX_SUCCESS);
}

UINT  _tx_byte_allocate(TX_BYTE_POOL *pool_ptr, VOID **memory_ptr, ULONG memory_size, ULONG wait_option)
{
VOID    *p;

    (VOID) wait_option;
    if ((pool_ptr == TX_NULL) || (pool_ptr -> tx_byte_pool_id != TX_BYTE_POOL_ID) || (memory_ptr == TX_NULL))
    {
        return(TX_PTR_ERROR);
    }
    if (pool_ptr == &_txm_module_manager_object_pool)
    {
        stub_obj_alloc_call_count++;
        if ((stub_obj_alloc_fail_on_call != 0U) && (stub_obj_alloc_call_count == stub_obj_alloc_fail_on_call))
        {
            return(TX_NO_MEMORY);
        }
    }
    p = arena_alloc(pool_ptr, memory_size);
    if (p == TX_NULL)
    {
        return(TX_NO_MEMORY);
    }
    *memory_ptr = p;
    return(TX_SUCCESS);
}

UINT  _txe_byte_allocate(TX_BYTE_POOL *pool_ptr, VOID **memory_ptr, ULONG memory_size, ULONG wait_option)
{
    return(_tx_byte_allocate(pool_ptr, memory_ptr, memory_size, wait_option));
}

UINT  _tx_byte_release(VOID *memory_ptr)
{
    stub_byte_release_calls++;
    return(arena_release(memory_ptr));
}

UINT  _txe_byte_release(VOID *memory_ptr)
{
    return(_tx_byte_release(memory_ptr));
}

UINT  _tx_mutex_create(TX_MUTEX *mutex_ptr, CHAR *name_ptr, UINT inherit)
{
    (VOID) inherit;
    if (mutex_ptr == TX_NULL)
    {
        return(TX_PTR_ERROR);
    }
    memset((VOID *) mutex_ptr, 0, sizeof(TX_MUTEX));
    mutex_ptr -> tx_mutex_id = TX_MUTEX_ID;
    mutex_ptr -> tx_mutex_name = name_ptr;
    return(TX_SUCCESS);
}

UINT  _tx_mutex_get(TX_MUTEX *mutex_ptr, ULONG wait_option)
{
    (VOID) wait_option;
    if (mutex_ptr == TX_NULL)
    {
        return(TX_PTR_ERROR);
    }
    mutex_ptr -> tx_mutex_ownership_count++;
    return(TX_SUCCESS);
}

UINT  _txe_mutex_get(TX_MUTEX *mutex_ptr, ULONG wait_option)
{
    return(_tx_mutex_get(mutex_ptr, wait_option));
}

UINT  _tx_mutex_put(TX_MUTEX *mutex_ptr)
{
    if ((mutex_ptr == TX_NULL) || (mutex_ptr -> tx_mutex_ownership_count == 0UL))
    {
        return(TX_NOT_OWNED);
    }
    mutex_ptr -> tx_mutex_ownership_count--;
    return(TX_SUCCESS);
}

UINT  _txe_mutex_put(TX_MUTEX *mutex_ptr)
{
    return(_tx_mutex_put(mutex_ptr));
}

UINT  _tx_queue_create(TX_QUEUE *queue_ptr, CHAR *name_ptr, UINT message_size, VOID *queue_start, ULONG queue_size)
{
    stub_queue_create_calls++;
    if (stub_queue_create_status != TX_SUCCESS)
    {
        return(stub_queue_create_status);
    }
    memset((VOID *) queue_ptr, 0, sizeof(TX_QUEUE));
    queue_ptr -> tx_queue_id = TX_QUEUE_ID;
    queue_ptr -> tx_queue_name = name_ptr;
    queue_ptr -> tx_queue_message_size = message_size;
    queue_ptr -> tx_queue_start = (ULONG *) queue_start;
    queue_ptr -> tx_queue_end = (ULONG *) (((UCHAR *) queue_start) + queue_size);
    queue_ptr -> tx_queue_read = queue_ptr -> tx_queue_start;
    queue_ptr -> tx_queue_write = queue_ptr -> tx_queue_start;
    return(TX_SUCCESS);
}

UINT  _tx_queue_delete(TX_QUEUE *queue_ptr)
{
    stub_queue_delete_calls++;
    if ((queue_ptr == TX_NULL) || (queue_ptr -> tx_queue_id != TX_QUEUE_ID))
    {
        return(TX_QUEUE_ERROR);
    }
    if (queue_ptr -> tx_queue_created_next != TX_NULL)
    {
        TX_QUEUE *next = queue_ptr -> tx_queue_created_next;
        TX_QUEUE *prev = queue_ptr -> tx_queue_created_previous;
        if (_tx_queue_created_count <= 1UL)
        {
            _tx_queue_created_ptr = TX_NULL;
            _tx_queue_created_count = 0UL;
        }
        else
        {
            next -> tx_queue_created_previous = prev;
            prev -> tx_queue_created_next = next;
            if (_tx_queue_created_ptr == queue_ptr)
            {
                _tx_queue_created_ptr = next;
            }
            _tx_queue_created_count--;
        }
    }
    queue_ptr -> tx_queue_id = 0UL;
    return(TX_SUCCESS);
}

UINT  _tx_thread_create(TX_THREAD *thread_ptr, TX_NAME_CONST CHAR *name_ptr,
                        VOID (*entry_function)(ULONG), ULONG entry_input,
                        VOID *stack_start, ULONG stack_size,
                        UINT priority, UINT preempt_threshold,
                        ULONG time_slice, UINT auto_start)
{
    (VOID) entry_function;
    (VOID) entry_input;
    (VOID) time_slice;

    memset((VOID *) thread_ptr, 0, sizeof(TX_THREAD));
    thread_ptr -> tx_thread_id = TX_THREAD_ID;
    thread_ptr -> tx_thread_name = name_ptr;
    thread_ptr -> tx_thread_stack_start = stack_start;
    thread_ptr -> tx_thread_stack_size = stack_size;
    thread_ptr -> tx_thread_stack_end = (VOID *) (((UCHAR *) stack_start) + stack_size - 1U);
    thread_ptr -> tx_thread_priority = priority;
    thread_ptr -> tx_thread_preempt_threshold = preempt_threshold;
    thread_ptr -> tx_thread_state = (auto_start == TX_AUTO_START) ? TX_READY : TX_SUSPENDED;
    return(TX_SUCCESS);
}

UINT  _tx_thread_resume(TX_THREAD *thread_ptr)
{
    stub_thread_resume_calls++;
    if (thread_ptr != TX_NULL)
    {
        thread_ptr -> tx_thread_state = TX_READY;
    }
    return(TX_SUCCESS);
}

UINT  _tx_thread_terminate(TX_THREAD *thread_ptr)
{
    stub_thread_terminate_calls++;
    if ((thread_ptr != TX_NULL) && (thread_ptr -> tx_thread_id == TX_THREAD_ID))
    {
        thread_ptr -> tx_thread_state = TX_TERMINATED;
        return(TX_SUCCESS);
    }
    return(TX_THREAD_ERROR);
}

UINT  _tx_thread_delete(TX_THREAD *thread_ptr)
{
    stub_thread_delete_calls++;
    if ((thread_ptr == TX_NULL) || (thread_ptr -> tx_thread_id != TX_THREAD_ID))
    {
        return(TX_THREAD_ERROR);
    }
    if (thread_ptr -> tx_thread_created_next != TX_NULL)
    {
        TX_THREAD *next = thread_ptr -> tx_thread_created_next;
        TX_THREAD *prev = thread_ptr -> tx_thread_created_previous;
        if (_tx_thread_created_count <= 1UL)
        {
            _tx_thread_created_ptr = TX_NULL;
            _tx_thread_created_count = 0UL;
        }
        else
        {
            next -> tx_thread_created_previous = prev;
            prev -> tx_thread_created_next = next;
            if (_tx_thread_created_ptr == thread_ptr)
            {
                _tx_thread_created_ptr = next;
            }
            _tx_thread_created_count--;
        }
    }
    thread_ptr -> tx_thread_id = 0UL;
    return(TX_SUCCESS);
}

UINT  _tx_thread_sleep(ULONG timer_ticks)
{
    (VOID) timer_ticks;
    stub_sleep_calls++;
    if ((stub_stop_watch_module != TX_NULL) && (!stub_stop_hang))
    {
        stub_stop_watch_module -> txm_module_instance_start_stop_thread.tx_thread_state = TX_COMPLETED;
    }
    return(TX_SUCCESS);
}

VOID  _tx_thread_system_resume(TX_THREAD *thread_ptr)
{
    if (thread_ptr != TX_NULL)
    {
        thread_ptr -> tx_thread_state = TX_READY;
    }
}

VOID  _tx_thread_system_preempt_check(VOID)
{
}

VOID  _tx_thread_timeout(ULONG timer_input)
{
    (VOID) timer_input;
}

VOID  _txm_module_manager_thread_stack_build(TX_THREAD *thread_ptr, VOID (*shell_function)(TX_THREAD *, TXM_MODULE_INSTANCE *))
{
    (VOID) thread_ptr;
    (VOID) shell_function;
    stub_stack_build_calls++;
}

ALIGN_TYPE  _txm_module_manager_user_mode_entry(ULONG kernel_request, ALIGN_TYPE param_1, ALIGN_TYPE param_2, ALIGN_TYPE param_3)
{
    (VOID) kernel_request;
    (VOID) param_1;
    (VOID) param_2;
    (VOID) param_3;
    return(0);
}

VOID  _txm_module_manager_alignment_adjust(TXM_MODULE_PREAMBLE *module_preamble, ULONG *code_size,
                                           ULONG *code_alignment, ULONG *data_size, ULONG *data_alignment)
{
    (VOID) module_preamble;
    (VOID) code_size;
    (VOID) code_alignment;
    (VOID) data_size;
    (VOID) data_alignment;
}

VOID  _txm_module_manager_mm_register_setup(TXM_MODULE_INSTANCE *module_instance)
{
    (VOID) module_instance;
}

VOID  _txm_module_manager_queue_notify_trampoline(TX_QUEUE *queue_ptr)
{
    (VOID) queue_ptr;
}

VOID  _txm_module_manager_event_flags_notify_trampoline(TX_EVENT_FLAGS_GROUP *group_ptr)
{
    (VOID) group_ptr;
}

VOID  _txm_module_manager_semaphore_notify_trampoline(TX_SEMAPHORE *semaphore_ptr)
{
    (VOID) semaphore_ptr;
}

ALIGN_TYPE  _txm_module_manager_kernel_dispatch(ULONG kernel_request, ALIGN_TYPE param_0, ALIGN_TYPE param_1, ALIGN_TYPE param_2)
{
    (VOID) kernel_request;
    (VOID) param_0;
    (VOID) param_1;
    (VOID) param_2;
    return(0);
}

UINT  _tx_block_pool_delete(TX_BLOCK_POOL *pool_ptr)
{
    stub_block_pool_delete_calls++;
    _tx_block_pool_created_ptr = TX_NULL;
    _tx_block_pool_created_count = 0UL;
    pool_ptr -> tx_block_pool_id = 0UL;
    return(TX_SUCCESS);
}

UINT  _tx_byte_pool_delete(TX_BYTE_POOL *pool_ptr)
{
    stub_byte_pool_delete_calls++;
    _tx_byte_pool_created_ptr = TX_NULL;
    _tx_byte_pool_created_count = 0UL;
    pool_ptr -> tx_byte_pool_id = 0UL;
    return(TX_SUCCESS);
}

UINT  _tx_event_flags_delete(TX_EVENT_FLAGS_GROUP *group_ptr)
{
    stub_ev_delete_calls++;
    _tx_event_flags_created_ptr = TX_NULL;
    _tx_event_flags_created_count = 0UL;
    group_ptr -> tx_event_flags_group_id = 0UL;
    return(TX_SUCCESS);
}

UINT  _tx_mutex_delete(TX_MUTEX *mutex_ptr)
{
    stub_mutex_delete_calls++;
    _tx_mutex_created_ptr = TX_NULL;
    _tx_mutex_created_count = 0UL;
    mutex_ptr -> tx_mutex_id = 0UL;
    return(TX_SUCCESS);
}

UINT  _tx_semaphore_delete(TX_SEMAPHORE *semaphore_ptr)
{
    stub_sem_delete_calls++;
    _tx_semaphore_created_ptr = TX_NULL;
    _tx_semaphore_created_count = 0UL;
    semaphore_ptr -> tx_semaphore_id = 0UL;
    return(TX_SUCCESS);
}

UINT  _tx_timer_delete(TX_TIMER *timer_ptr)
{
    stub_timer_delete_calls++;
    _tx_timer_created_ptr = TX_NULL;
    _tx_timer_created_count = 0UL;
    timer_ptr -> tx_timer_id = 0UL;
    return(TX_SUCCESS);
}

/* Module image fixtures in RAM (code area with TXM_MODULE_PREAMBLE).  */
typedef struct TEST_MODULE_IMAGE_STRUCT
{
    TXM_MODULE_PREAMBLE preamble;
    UCHAR               code_body[256];
} TEST_MODULE_IMAGE;

static TEST_MODULE_IMAGE    test_image_1;
static TEST_MODULE_IMAGE    test_image_2;
static union
{
    ALIGN_TYPE  align;
    UCHAR       bytes[16384];
} manager_pool_mem;
static union
{
    ALIGN_TYPE  align;
    UCHAR       bytes[16384];
} object_pool_mem;
static TXM_MODULE_INSTANCE  mod_1;
static TXM_MODULE_INSTANCE  mod_2;
static TXM_MODULE_INSTANCE  bad_mod;
static TX_THREAD            host_thread;

static VOID dummy_shell(TX_THREAD *t, TXM_MODULE_INSTANCE *m)
{
    (VOID) t;
    (VOID) m;
}

static VOID dummy_thread_entry(ULONG input)
{
    (VOID) input;
}

static VOID init_valid_image(TEST_MODULE_IMAGE *img, ULONG props, UINT prio)
{
    memset((VOID *) img, 0, sizeof(TEST_MODULE_IMAGE));
    img -> preamble.txm_module_preamble_id = TXM_MODULE_ID;
    img -> preamble.txm_module_preamble_version_major = 1UL;
    img -> preamble.txm_module_preamble_version_minor = 0UL;
    img -> preamble.txm_module_preamble_preamble_size = (ULONG) (sizeof(TXM_MODULE_PREAMBLE) / sizeof(ULONG));
    img -> preamble.txm_module_preamble_application_module_id = 42UL;
    img -> preamble.txm_module_preamble_property_flags = props;
    img -> preamble.txm_module_preamble_shell_entry_function = (ULONG) sizeof(TXM_MODULE_PREAMBLE);
    img -> preamble.txm_module_preamble_start_function = (ULONG) (sizeof(TXM_MODULE_PREAMBLE) + 16U);
    img -> preamble.txm_module_preamble_stop_function = (ULONG) (sizeof(TXM_MODULE_PREAMBLE) + 32U);
    img -> preamble.txm_module_preamble_callback_function = (ULONG) (sizeof(TXM_MODULE_PREAMBLE) + 48U);
    img -> preamble.txm_module_preamble_start_stop_priority = (ULONG) prio;
    img -> preamble.txm_module_preamble_start_stop_stack_size = 512UL;
    img -> preamble.txm_module_preamble_callback_priority = (ULONG) prio;
    img -> preamble.txm_module_preamble_callback_stack_size = 512UL;
    img -> preamble.txm_module_preamble_code_size = (ULONG) sizeof(TEST_MODULE_IMAGE);
    img -> preamble.txm_module_preamble_data_size = 512UL;
}

int main(void)
{
ULONG       props_out = 0UL;
VOID        *code_alloc_block = TX_NULL;
TX_THREAD   mod_worker_thread;
TX_THREAD   non_mod_thread;
UCHAR       mod_worker_stack[1024];
TX_QUEUE    *owned_queue = TX_NULL;
TX_SEMAPHORE *owned_sem = TX_NULL;
TX_MUTEX    *owned_mutex = TX_NULL;
TX_EVENT_FLAGS_GROUP *owned_ev = TX_NULL;
TX_BLOCK_POOL *owned_bp = TX_NULL;
TX_BYTE_POOL *owned_byp = TX_NULL;
TX_TIMER    *owned_timer = TX_NULL;

    _tx_thread_system_state = 0UL;
    memset((VOID *) &host_thread, 0, sizeof(host_thread));
    _tx_thread_current_ptr = &host_thread;

    /* 1. Calls before manager initialization must return TX_NOT_AVAILABLE.  */
    test_expect("internal_load before init returns TX_NOT_AVAILABLE",
                _txm_module_manager_internal_load(&mod_1, "m1", &test_image_1, sizeof(test_image_1), TX_NULL, 0UL, 0UL),
                TX_NOT_AVAILABLE);
    test_expect("properties_get before init returns TX_NOT_AVAILABLE",
                _txm_module_manager_properties_get(&mod_1, &props_out), TX_NOT_AVAILABLE);
    test_expect("max_priority_set before init returns TX_NOT_AVAILABLE",
                _txm_module_manager_maximum_module_priority_set(&mod_1, 10U), TX_NOT_AVAILABLE);
    test_expect("start before init returns TX_NOT_AVAILABLE",
                _txm_module_manager_start(&mod_1), TX_NOT_AVAILABLE);
    test_expect("stop before init returns TX_NOT_AVAILABLE",
                _txm_module_manager_stop(&mod_1), TX_NOT_AVAILABLE);
    test_expect("unload before init returns TX_NOT_AVAILABLE",
                _txm_module_manager_unload(&mod_1), TX_NOT_AVAILABLE);

    /* 2. Initialize & Object Pool Create tests.  */
    _tx_thread_system_state = 1UL;
    test_expect("initialize from interrupt returns TX_CALLER_ERROR",
                _txm_module_manager_initialize(manager_pool_mem.bytes, sizeof(manager_pool_mem.bytes)),
                TX_CALLER_ERROR);
    test_expect("manager stays not ready after ISR initialize attempt", _txm_module_manager_ready, TX_FALSE);
    _tx_thread_system_state = 0UL;

    stub_byte_pool_create_status = TX_SUCCESS;
    _txm_module_manager_object_pool_created = TX_TRUE;
    test_expect("initialize succeeds with valid memory",
                _txm_module_manager_initialize(manager_pool_mem.bytes, sizeof(manager_pool_mem.bytes)),
                TX_SUCCESS);
    test_expect("manager ready flag is TX_TRUE", _txm_module_manager_ready, TX_TRUE);
    test_expect("initialize resets object_pool_created to TX_FALSE", _txm_module_manager_object_pool_created, TX_FALSE);

    test_expect("object_pool_create succeeds",
                _txm_module_manager_object_pool_create(object_pool_mem.bytes, sizeof(object_pool_mem.bytes)),
                TX_SUCCESS);
    test_expect("object_pool_created is TX_TRUE", _txm_module_manager_object_pool_created, TX_TRUE);

    /* 3. Internal load validation, property enforcement, and loaded list linking.  */
    init_valid_image(&test_image_1, (TXM_MODULE_USER_MODE | TXM_MODULE_MEMORY_PROTECTION), 16U);
    init_valid_image(&test_image_2, TXM_MODULE_USER_MODE, 16U);

    _tx_thread_system_state = 1UL;
    test_expect("internal_load from interrupt returns TX_CALLER_ERROR",
                _txm_module_manager_internal_load(&mod_1, "m1", &test_image_1, sizeof(test_image_1), TX_NULL, 0UL, 0UL),
                TX_CALLER_ERROR);
    _tx_thread_system_state = 0UL;

    test_expect("internal_load NULL module returns TX_PTR_ERROR",
                _txm_module_manager_internal_load(TX_NULL, "m1", &test_image_1, sizeof(test_image_1), TX_NULL, 0UL, 0UL),
                TX_PTR_ERROR);

    /* TXM_MODULE_MEMORY_PROTECTION without TXM_MODULE_USER_MODE must fail.  */
    test_image_1.preamble.txm_module_preamble_property_flags = TXM_MODULE_MEMORY_PROTECTION;
    test_expect("internal_load rejects memory protection without user mode",
                _txm_module_manager_internal_load(&mod_1, "m1", &test_image_1, sizeof(test_image_1), TX_NULL, 0UL, 0UL),
                TXM_MODULE_INVALID_PROPERTIES);
    test_image_1.preamble.txm_module_preamble_property_flags = (TXM_MODULE_USER_MODE | TXM_MODULE_MEMORY_PROTECTION);

    /* Allocate a fake code allocation block from manager pool to verify unload frees code_allocation_ptr too.  */
    test_expect("code block test alloc succeeds",
                _tx_byte_allocate(&_txm_module_manager_byte_pool, &code_alloc_block, 512UL, TX_NO_WAIT), TX_SUCCESS);
    test_expect("internal_load valid mod_1 succeeds",
                _txm_module_manager_internal_load(&mod_1, "mod_1", &test_image_1, sizeof(test_image_1),
                                                  code_alloc_block, 512UL, 0UL),
                TX_SUCCESS);
    test_expect("internal_load duplicate mod_1 returns TXM_MODULE_ALREADY_LOADED",
                _txm_module_manager_internal_load(&mod_1, "mod_1", &test_image_1, sizeof(test_image_1),
                                                  TX_NULL, 0UL, 0UL),
                TXM_MODULE_ALREADY_LOADED);
    test_expect("internal_load valid mod_2 succeeds",
                _txm_module_manager_internal_load(&mod_2, "mod_2", &test_image_2, sizeof(test_image_2),
                                                  TX_NULL, 0UL, 0UL),
                TX_SUCCESS);
    test_expect("loaded module count == 2", _txm_module_manger_loaded_count, 2UL);

    /* 4. properties_get & maximum_module_priority_set tests.  */
    memset((VOID *) &bad_mod, 0, sizeof(bad_mod));
    test_expect("properties_get NULL module returns TX_PTR_ERROR",
                _txm_module_manager_properties_get(TX_NULL, &props_out), TX_PTR_ERROR);
    test_expect("properties_get NULL out ptr returns TX_PTR_ERROR",
                _txm_module_manager_properties_get(&mod_1, TX_NULL), TX_PTR_ERROR);
    test_expect("properties_get cleared-ID module returns TX_PTR_ERROR",
                _txm_module_manager_properties_get(&bad_mod, &props_out), TX_PTR_ERROR);
    test_expect("properties_get valid mod_1 returns TX_SUCCESS",
                _txm_module_manager_properties_get(&mod_1, &props_out), TX_SUCCESS);
    test_expect("properties_get returns loaded property flags",
                props_out, (TXM_MODULE_USER_MODE | TXM_MODULE_MEMORY_PROTECTION));

    test_expect("max_priority_set NULL module returns TX_PTR_ERROR",
                _txm_module_manager_maximum_module_priority_set(TX_NULL, 10U), TX_PTR_ERROR);
    test_expect("max_priority_set succeeds on loaded module",
                _txm_module_manager_maximum_module_priority_set(&mod_1, 10U), TX_SUCCESS);
    test_expect("max_priority updated to 10", mod_1.txm_module_instance_maximum_priority, 10UL);

    /* Enforce privilege ceiling in start(): preamble priority 16 >= 10 succeeds, 5 < 10 fails.  */
    mod_1.txm_module_instance_start_stop_priority = 5UL;
    test_expect("start refuses privileged start_stop_priority (< max)",
                _txm_module_manager_start(&mod_1), TX_PRIORITY_ERROR);
    mod_1.txm_module_instance_start_stop_priority = 16UL;

    mod_1.txm_module_instance_callback_priority = 5UL;
    test_expect("start refuses privileged callback_priority (< max)",
                _txm_module_manager_start(&mod_1), TX_PRIORITY_ERROR);
    mod_1.txm_module_instance_callback_priority = 16UL;

    /* 5. start() failure rollback tests (queue fail, 1st thread fail, 2nd thread fail).  */
    reset_stub_counters();
    stub_queue_create_status = TX_NO_MEMORY;
    test_expect("start returns TX_START_ERROR when callback queue create fails",
                _txm_module_manager_start(&mod_1), TX_START_ERROR);
    test_expect("module stays LOADED after queue failure", mod_1.txm_module_instance_state, TXM_MODULE_LOADED);

    reset_stub_counters();
    stub_obj_alloc_fail_on_call = 1U;
    test_expect("start rolls back queue when start_stop_thread fails", _txm_module_manager_start(&mod_1), TX_NO_MEMORY);
    test_expect("callback queue deleted on start thread failure", stub_queue_delete_calls, 1UL);
    test_expect("module stays LOADED after start thread failure", mod_1.txm_module_instance_state, TXM_MODULE_LOADED);

    reset_stub_counters();
    stub_obj_alloc_fail_on_call = 2U;
    test_expect("start rolls back start_stop_thread + queue when callback thread fails",
                _txm_module_manager_start(&mod_1), TX_NO_MEMORY);
    test_expect("start_stop_thread terminated on callback thread failure", stub_thread_terminate_calls, 1UL);
    test_expect("start_stop_thread deleted on callback thread failure", stub_thread_delete_calls, 1UL);
    test_expect("callback queue deleted on callback thread failure", stub_queue_delete_calls, 1UL);

    /* Valid start().  */
    reset_stub_counters();
    test_expect("start NULL module returns TX_PTR_ERROR",
                _txm_module_manager_start(TX_NULL), TX_PTR_ERROR);
    test_expect("valid start succeeds", _txm_module_manager_start(&mod_1), TX_SUCCESS);
    test_expect("start state becomes TXM_MODULE_STARTED", mod_1.txm_module_instance_state, TXM_MODULE_STARTED);
    test_expect("start while already started returns TX_START_ERROR",
                _txm_module_manager_start(&mod_1), TX_START_ERROR);
    test_expect("max_priority_set refused while started",
                _txm_module_manager_maximum_module_priority_set(&mod_1, 12U), TX_START_ERROR);
    test_expect("unload refused while started (TX_NOT_DONE)",
                _txm_module_manager_unload(&mod_1), TX_NOT_DONE);

    /* 6. thread_create & thread_reset tests.  */
    reset_stub_counters();
    memset((VOID *) &mod_worker_thread, 0, sizeof(mod_worker_thread));
    test_expect("_txm_module_manager_thread_create succeeds",
                _txm_module_manager_thread_create(&mod_worker_thread, "mwrk", dummy_shell,
                                                  dummy_thread_entry, 0UL,
                                                  mod_worker_stack, sizeof(mod_worker_stack),
                                                  16U, 16U, TX_NO_TIME_SLICE, TX_DONT_START,
                                                  sizeof(TX_THREAD), &mod_1),
                TX_SUCCESS);
    test_expect("thread_create allocates kernel stack for user-mode module",
                (ULONG) (mod_worker_thread.tx_thread_module_kernel_stack_start != TX_NULL), 1UL);

    /* Invalid thread_reset requests.  */
    _tx_thread_current_ptr = &mod_worker_thread;
    test_expect("thread_reset refuses current thread",
                _txm_module_manager_thread_reset(&mod_worker_thread), TX_NOT_DONE);
    /* txm_module_manager_thread_reset performs a second TX_RESTORE on current_thread mismatch.  */
    test_expect("current-thread mismatch triggers extra TX_RESTORE", (ULONG) test_interrupt_restore_underflows, 1UL);
    test_interrupt_restore_underflows = 0U;
    _tx_thread_current_ptr = &host_thread;

    memset((VOID *) &non_mod_thread, 0, sizeof(non_mod_thread));
    non_mod_thread.tx_thread_id = TX_THREAD_ID;
    non_mod_thread.tx_thread_state = TX_COMPLETED;
    test_expect("thread_reset refuses non-module completed thread",
                _txm_module_manager_thread_reset(&non_mod_thread), TX_NOT_DONE);

    mod_worker_thread.tx_thread_state = TX_SUSPENDED;
    test_expect("thread_reset refuses TX_SUSPENDED thread",
                _txm_module_manager_thread_reset(&mod_worker_thread), TX_NOT_DONE);

    /* Valid thread_reset on TX_COMPLETED & TX_TERMINATED.  */
    reset_stub_counters();
    mod_worker_thread.tx_thread_state = TX_COMPLETED;
    test_expect("thread_reset succeeds on completed module thread",
                _txm_module_manager_thread_reset(&mod_worker_thread), TX_SUCCESS);
    test_expect("thread_reset rebuilds stack", stub_stack_build_calls, 1UL);
    test_expect("thread_reset moves thread to TX_SUSPENDED", mod_worker_thread.tx_thread_state, TX_SUSPENDED);

    mod_worker_thread.tx_thread_state = TX_TERMINATED;
    test_expect("thread_reset succeeds on terminated module thread",
                _txm_module_manager_thread_reset(&mod_worker_thread), TX_SUCCESS);

    /* 7. Allocate objects of each kernel type owned by mod_1 so stop() exercises cleanup loops.  */
    _txm_module_manager_object_allocate((VOID **) &owned_queue, sizeof(TX_QUEUE), &mod_1);
    memset((VOID *) owned_queue, 0, sizeof(TX_QUEUE));
    owned_queue -> tx_queue_id = TX_QUEUE_ID;
    owned_queue -> tx_queue_created_next = owned_queue;
    owned_queue -> tx_queue_created_previous = owned_queue;
    _tx_queue_created_ptr = owned_queue;
    _tx_queue_created_count = 1UL;

    _txm_module_manager_object_allocate((VOID **) &owned_sem, sizeof(TX_SEMAPHORE), &mod_1);
    memset((VOID *) owned_sem, 0, sizeof(TX_SEMAPHORE));
    owned_sem -> tx_semaphore_id = TX_SEMAPHORE_ID;
    owned_sem -> tx_semaphore_created_next = owned_sem;
    owned_sem -> tx_semaphore_created_previous = owned_sem;
    _tx_semaphore_created_ptr = owned_sem;
    _tx_semaphore_created_count = 1UL;

    _txm_module_manager_object_allocate((VOID **) &owned_mutex, sizeof(TX_MUTEX), &mod_1);
    memset((VOID *) owned_mutex, 0, sizeof(TX_MUTEX));
    owned_mutex -> tx_mutex_id = TX_MUTEX_ID;
    owned_mutex -> tx_mutex_created_next = owned_mutex;
    owned_mutex -> tx_mutex_created_previous = owned_mutex;
    _tx_mutex_created_ptr = owned_mutex;
    _tx_mutex_created_count = 1UL;

    _txm_module_manager_object_allocate((VOID **) &owned_ev, sizeof(TX_EVENT_FLAGS_GROUP), &mod_1);
    memset((VOID *) owned_ev, 0, sizeof(TX_EVENT_FLAGS_GROUP));
    owned_ev -> tx_event_flags_group_id = TX_EVENT_FLAGS_ID;
    owned_ev -> tx_event_flags_group_created_next = owned_ev;
    owned_ev -> tx_event_flags_group_created_previous = owned_ev;
    _tx_event_flags_created_ptr = owned_ev;
    _tx_event_flags_created_count = 1UL;

    _txm_module_manager_object_allocate((VOID **) &owned_bp, sizeof(TX_BLOCK_POOL), &mod_1);
    memset((VOID *) owned_bp, 0, sizeof(TX_BLOCK_POOL));
    owned_bp -> tx_block_pool_id = TX_BLOCK_POOL_ID;
    owned_bp -> tx_block_pool_created_next = owned_bp;
    owned_bp -> tx_block_pool_created_previous = owned_bp;
    _tx_block_pool_created_ptr = owned_bp;
    _tx_block_pool_created_count = 1UL;

    _txm_module_manager_object_allocate((VOID **) &owned_byp, sizeof(TX_BYTE_POOL), &mod_1);
    memset((VOID *) owned_byp, 0, sizeof(TX_BYTE_POOL));
    owned_byp -> tx_byte_pool_id = TX_BYTE_POOL_ID;
    owned_byp -> tx_byte_pool_created_next = owned_byp;
    owned_byp -> tx_byte_pool_created_previous = owned_byp;
    _tx_byte_pool_created_ptr = owned_byp;
    _tx_byte_pool_created_count = 1UL;

    _txm_module_manager_object_allocate((VOID **) &owned_timer, sizeof(TX_TIMER), &mod_1);
    memset((VOID *) owned_timer, 0, sizeof(TX_TIMER));
    owned_timer -> tx_timer_id = TX_TIMER_ID;
    owned_timer -> tx_timer_created_next = owned_timer;
    owned_timer -> tx_timer_created_previous = owned_timer;
    _tx_timer_created_ptr = owned_timer;
    _tx_timer_created_count = 1UL;

    /* 8. Stop & Unload tests.  */
    reset_stub_counters();
    stub_stop_watch_module = &mod_1;
    _tx_thread_system_state = 1UL;
    test_expect("stop from interrupt returns TX_CALLER_ERROR",
                _txm_module_manager_stop(&mod_1), TX_CALLER_ERROR);
    _tx_thread_system_state = 0UL;

    test_expect("stop NULL module returns TX_PTR_ERROR",
                _txm_module_manager_stop(TX_NULL), TX_PTR_ERROR);
    test_expect("stop valid STARTED mod_1 succeeds",
                _txm_module_manager_stop(&mod_1), TX_SUCCESS);
    test_expect("mod_1 state becomes TXM_MODULE_STOPPED", mod_1.txm_module_instance_state, TXM_MODULE_STOPPED);
    test_expect("stop deletes all module-owned created objects",
                (ULONG) ((stub_queue_delete_calls >= 1UL) && (stub_sem_delete_calls == 1UL) &&
                         (stub_mutex_delete_calls == 1UL) && (stub_ev_delete_calls == 1UL) &&
                         (stub_block_pool_delete_calls == 1UL) && (stub_byte_pool_delete_calls == 1UL) &&
                         (stub_timer_delete_calls == 1UL)),
                1UL);
    test_expect("stop releases all object pool allocations owned by mod_1",
                mod_1.txm_module_instance_object_list_count, 0UL);
    test_expect("stop while already stopped returns TX_START_ERROR",
                _txm_module_manager_stop(&mod_1), TX_START_ERROR);

    /* Unload mod_1 and mod_2, verifying list unlinking and memory release.  */
    _tx_thread_system_state = 1UL;
    test_expect("unload from interrupt returns TX_CALLER_ERROR",
                _txm_module_manager_unload(&mod_1), TX_CALLER_ERROR);
    _tx_thread_system_state = 0UL;

    test_expect("unload NULL returns TX_PTR_ERROR",
                _txm_module_manager_unload(TX_NULL), TX_PTR_ERROR);
    test_expect("unload mod_1 succeeds", _txm_module_manager_unload(&mod_1), TX_SUCCESS);
    test_expect("mod_1 state becomes TXM_MODULE_UNLOADED", mod_1.txm_module_instance_state, TXM_MODULE_UNLOADED);
    test_expect("mod_1 ID cleared on unload", mod_1.txm_module_instance_id, 0UL);
    test_expect("loaded count decrements to 1", _txm_module_manger_loaded_count, 1UL);
    test_expect("unload mod_2 succeeds", _txm_module_manager_unload(&mod_2), TX_SUCCESS);
    test_expect("loaded count decrements to 0 and list is empty",
                (ULONG) ((_txm_module_manger_loaded_count == 0UL) && (_txm_module_manager_loaded_list_ptr == TX_NULL)),
                1UL);
    test_expect("all arena allocations freed after unloading both modules", (ULONG) arena_active_allocs(), 0UL);

    test_expect("interrupt disable depth balanced to 0", (ULONG) test_interrupt_disable_depth, 0UL);
    test_expect("interrupt restore underflows == 0", (ULONG) test_interrupt_restore_underflows, 0UL);

    if (test_failures == 0U)
    {
        printf("SUCCESS! %lu expectations\n", (unsigned long) test_checks);
        return(0);
    }

    printf("ERROR: %lu expectation(s) failed of %lu\n",
           (unsigned long) test_failures, (unsigned long) test_checks);
    return(1);
}
