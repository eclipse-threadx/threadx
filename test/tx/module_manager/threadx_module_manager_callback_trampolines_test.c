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
#include "tx_timer.h"
#include "tx_queue.h"
#include "tx_semaphore.h"
#include "tx_mutex.h"
#include "tx_event_flags.h"
#include "tx_block_pool.h"
#include "tx_byte_pool.h"
#include "txm_module.h"
#include "txm_module_manager_util.h"

#ifndef TXM_MODULE_OBJECT_POINTER_GET_CALL_NOT_USED
#error "Expected unused dispatchers to be compiled out for targeted link"
#endif

/* Interrupt lock accounting expected by the host port shim.  */
unsigned int    test_interrupt_disable_depth;
unsigned int    test_interrupt_disable_max_depth;
unsigned int    test_interrupt_restore_underflows;

unsigned int    test_get_ipsr_value(void)
{
    return(0U);
}

/* Kernel object created lists required by txm_module_manager_util.c.  */
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

/* Module manager state globals required by callback and util sources.  */
volatile ULONG          _tx_thread_system_state;
TX_THREAD               *_tx_thread_current_ptr;
TX_TIMER_INTERNAL       *_tx_timer_expired_timer_ptr;
TX_BYTE_POOL            _txm_module_manager_object_pool;
UINT                    _txm_module_manager_object_pool_created;
TX_MUTEX                _txm_module_manager_mutex;
TXM_MODULE_INSTANCE     *_txm_module_manager_loaded_list_ptr;
ULONG                   _txm_module_manger_loaded_count;
UINT                    _txm_module_manager_ready;
ULONG                   _txm_module_manager_properties_supported;
ULONG                   _txm_module_manager_properties_required;
ULONG                   _txm_module_manager_callback_total_count;
ULONG                   _txm_module_manager_callback_error_count;
TX_NAME_CONST CHAR      _txm_module_manager_callback_request_queue_name[] = "Module Callback Request Queue";

/* Declarations for internal trampoline functions under test.  */
VOID  _txm_module_manager_queue_notify_trampoline(TX_QUEUE *queue_ptr);
VOID  _txm_module_manager_semaphore_notify_trampoline(TX_SEMAPHORE *semaphore_ptr);
VOID  _txm_module_manager_event_flags_notify_trampoline(TX_EVENT_FLAGS_GROUP *group_ptr);
VOID  _txm_module_manager_thread_notify_trampoline(TX_THREAD *thread_ptr, UINT type);
VOID  _txm_module_manager_timer_notify_trampoline(ULONG id);

static UINT     test_failures;
static ULONG    test_checks;

VOID  _txm_module_manager_alignment_adjust(TXM_MODULE_PREAMBLE *module_preamble, ULONG *code_size,
                                           ULONG *code_alignment, ULONG *data_size, ULONG *data_alignment)
{
    (VOID) module_preamble;
    (VOID) code_size;
    (VOID) code_alignment;
    (VOID) data_size;
    (VOID) data_alignment;
}

UINT  _txm_module_manager_application_request(ULONG request_id, ALIGN_TYPE param_1, ALIGN_TYPE param_2, ALIGN_TYPE param_3)
{
    (VOID) request_id;
    (VOID) param_1;
    (VOID) param_2;
    (VOID) param_3;
    return(TX_NOT_AVAILABLE);
}

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

/* Mocked kernel service call tracking for notify dispatchers.  */
static ULONG    stub_queue_notify_calls;
static VOID     (*stub_registered_queue_notify)(TX_QUEUE *);
static ULONG    stub_sem_notify_calls;
static VOID     (*stub_registered_sem_notify)(TX_SEMAPHORE *);
static ULONG    stub_ev_notify_calls;
static VOID     (*stub_registered_ev_notify)(TX_EVENT_FLAGS_GROUP *);
static ULONG    stub_thread_notify_calls;
static VOID     (*stub_registered_thread_notify)(TX_THREAD *, UINT);

UINT  _txe_queue_send_notify(TX_QUEUE *queue_ptr, VOID (*queue_send_notify)(TX_QUEUE *))
{
    stub_queue_notify_calls++;
    stub_registered_queue_notify = queue_send_notify;
    if (queue_ptr == TX_NULL)
    {
        return(TX_QUEUE_ERROR);
    }
    return(TX_SUCCESS);
}

UINT  _txe_semaphore_put_notify(TX_SEMAPHORE *semaphore_ptr, VOID (*semaphore_put_notify)(TX_SEMAPHORE *))
{
    stub_sem_notify_calls++;
    stub_registered_sem_notify = semaphore_put_notify;
    if (semaphore_ptr == TX_NULL)
    {
        return(TX_SEMAPHORE_ERROR);
    }
    return(TX_SUCCESS);
}

UINT  _txe_event_flags_set_notify(TX_EVENT_FLAGS_GROUP *group_ptr, VOID (*events_set_notify)(TX_EVENT_FLAGS_GROUP *))
{
    stub_ev_notify_calls++;
    stub_registered_ev_notify = events_set_notify;
    if (group_ptr == TX_NULL)
    {
        return(TX_GROUP_ERROR);
    }
    return(TX_SUCCESS);
}

UINT  _txe_thread_entry_exit_notify(TX_THREAD *thread_ptr, VOID (*thread_entry_exit_notify)(TX_THREAD *, UINT))
{
    stub_thread_notify_calls++;
    stub_registered_thread_notify = thread_entry_exit_notify;
    if (thread_ptr == TX_NULL)
    {
        return(TX_THREAD_ERROR);
    }
    return(TX_SUCCESS);
}

UINT  _txe_mutex_get(TX_MUTEX *mutex_ptr, ULONG wait_option)
{
    (VOID) mutex_ptr;
    (VOID) wait_option;
    return(TX_SUCCESS);
}

UINT  _txe_mutex_put(TX_MUTEX *mutex_ptr)
{
    (VOID) mutex_ptr;
    return(TX_SUCCESS);
}

/* Ring-buffer stand-in for _tx_queue_send matching ThreadX ULONG queue layout.  */
UINT  _tx_queue_send(TX_QUEUE *queue_ptr, VOID *source_ptr, ULONG wait_option)
{
ULONG   msg_words;
ULONG   *src;
ULONG   i;

    (VOID) wait_option;

    if ((queue_ptr == TX_NULL) || (queue_ptr -> tx_queue_id != TX_QUEUE_ID))
    {
        return(TX_QUEUE_ERROR);
    }
    if (queue_ptr -> tx_queue_available_storage == ((ULONG) 0))
    {
        return(TX_QUEUE_FULL);
    }

    msg_words = (ULONG) queue_ptr -> tx_queue_message_size;
    src = (ULONG *) source_ptr;
    for (i = 0; i < msg_words; i++)
    {
        queue_ptr -> tx_queue_write[i] = src[i];
    }

    queue_ptr -> tx_queue_write += msg_words;
    if (queue_ptr -> tx_queue_write >= queue_ptr -> tx_queue_end)
    {
        queue_ptr -> tx_queue_write = queue_ptr -> tx_queue_start;
    }
    queue_ptr -> tx_queue_enqueued++;
    queue_ptr -> tx_queue_available_storage--;
    return(TX_SUCCESS);
}

static VOID  init_callback_queue(TXM_MODULE_INSTANCE *mod)
{
TX_QUEUE    *q = &(mod -> txm_module_instance_callback_request_queue);
ULONG       msg_words = (ULONG) (sizeof(TXM_MODULE_CALLBACK_MESSAGE) / sizeof(ULONG));

    memset((VOID *) q, 0, sizeof(TX_QUEUE));
    q -> tx_queue_id = TX_QUEUE_ID;
    q -> tx_queue_name = _txm_module_manager_callback_request_queue_name;
    q -> tx_queue_message_size = (UINT) msg_words;
    q -> tx_queue_capacity = TXM_MODULE_CALLBACKS_QUEUE_DEPTH;
    q -> tx_queue_available_storage = TXM_MODULE_CALLBACKS_QUEUE_DEPTH;
    q -> tx_queue_enqueued = 0;
    q -> tx_queue_start = &(mod -> txm_module_instance_callback_request_queue_area[0]);
    q -> tx_queue_end = q -> tx_queue_start + (TXM_MODULE_CALLBACKS_QUEUE_DEPTH * msg_words);
    q -> tx_queue_read = q -> tx_queue_start;
    q -> tx_queue_write = q -> tx_queue_start;
}

static VOID  pop_callback_message(TX_QUEUE *q, TXM_MODULE_CALLBACK_MESSAGE *out)
{
ULONG   msg_words = (ULONG) q -> tx_queue_message_size;

    memcpy((VOID *) out, (VOID *) q -> tx_queue_read, sizeof(TXM_MODULE_CALLBACK_MESSAGE));
    q -> tx_queue_read += msg_words;
    if (q -> tx_queue_read >= q -> tx_queue_end)
    {
        q -> tx_queue_read = q -> tx_queue_start;
    }
    q -> tx_queue_enqueued--;
    q -> tx_queue_available_storage++;
}

/* Module objects and callbacks.  */
static UCHAR                        mod_data_buf[256];
static UCHAR                        mod_code_buf[256];
static TXM_MODULE_INSTANCE          mod_a;
static TX_THREAD                    calling_thread;
static TX_QUEUE                     app_queue;
static TX_SEMAPHORE                 app_sem;
static TX_EVENT_FLAGS_GROUP         app_ev;
static TX_THREAD                    app_thread;
static TXM_MODULE_THREAD_ENTRY_INFO app_thread_info;
static TX_TIMER                     app_timer;

static VOID dummy_app_fn(VOID)
{
}

static VOID dummy_thread_exit_cb(TX_THREAD *t, UINT type)
{
    (VOID) t;
    (VOID) type;
}

static VOID dummy_timer_cb(ULONG id)
{
    (VOID) id;
}

static VOID setup_module(TXM_MODULE_INSTANCE *m, ULONG state, ULONG props)
{
    memset((VOID *) m, 0, sizeof(TXM_MODULE_INSTANCE));
    m -> txm_module_instance_id = TXM_MODULE_ID;
    m -> txm_module_instance_state = state;
    m -> txm_module_instance_property_flags = props;
    m -> txm_module_instance_data_start = (VOID *) mod_data_buf;
    m -> txm_module_instance_data_end = (VOID *) (mod_data_buf + sizeof(mod_data_buf) - 1U);
    m -> txm_module_instance_code_start = (VOID *) mod_code_buf;
    m -> txm_module_instance_code_end = (VOID *) (mod_code_buf + sizeof(mod_code_buf) - 1U);
    init_callback_queue(m);
}

#define REGISTER_OBJ(ptr, next_f, head, cnt) \
    do {                                     \
        (ptr) -> next_f = (head);            \
        (head) = (ptr);                      \
        (cnt)++;                             \
    } while (0)

int main(void)
{
TXM_MODULE_CALLBACK_MESSAGE msg;
TXM_MODULE_CALLBACK_MESSAGE popped;
TX_QUEUE                    bad_queue;
ULONG                       i;
ALIGN_TYPE                  ret;

    _tx_thread_system_state = 0;
    memset((VOID *) &calling_thread, 0, sizeof(calling_thread));
    _tx_thread_current_ptr = &calling_thread;

    setup_module(&mod_a, TXM_MODULE_STARTED, (TXM_MODULE_USER_MODE | TXM_MODULE_MEMORY_PROTECTION));
    calling_thread.tx_thread_module_instance_ptr = &mod_a;

    /* Prepare valid kernel objects and add to created lists.  */
    memset((VOID *) &app_queue, 0, sizeof(app_queue));
    app_queue.tx_queue_id = TX_QUEUE_ID;
    REGISTER_OBJ(&app_queue, tx_queue_created_next, _tx_queue_created_ptr, _tx_queue_created_count);

    memset((VOID *) &app_sem, 0, sizeof(app_sem));
    app_sem.tx_semaphore_id = TX_SEMAPHORE_ID;
    REGISTER_OBJ(&app_sem, tx_semaphore_created_next, _tx_semaphore_created_ptr, _tx_semaphore_created_count);

    memset((VOID *) &app_ev, 0, sizeof(app_ev));
    app_ev.tx_event_flags_group_id = TX_EVENT_FLAGS_ID;
    REGISTER_OBJ(&app_ev, tx_event_flags_group_created_next, _tx_event_flags_created_ptr, _tx_event_flags_created_count);

    memset((VOID *) &app_thread, 0, sizeof(app_thread));
    app_thread.tx_thread_id = TX_THREAD_ID;
    app_thread.tx_thread_module_instance_ptr = (VOID *) &mod_a;
    memset((VOID *) &app_thread_info, 0, sizeof(app_thread_info));
    app_thread_info.txm_module_thread_entry_info_exit_notify = dummy_thread_exit_cb;
    app_thread.tx_thread_module_entry_info_ptr = (VOID *) &app_thread_info;
    REGISTER_OBJ(&app_thread, tx_thread_created_next, _tx_thread_created_ptr, _tx_thread_created_count);

    memset((VOID *) &app_timer, 0, sizeof(app_timer));
    app_timer.tx_timer_id = TX_TIMER_ID;
    app_timer.tx_timer_module_instance = (VOID *) &mod_a;
    app_timer.tx_timer_module_expiration_function = dummy_timer_cb;
    app_timer.tx_timer_internal.tx_timer_internal_timeout_param = 0xCAFEUL;
    REGISTER_OBJ(&app_timer, tx_timer_created_next, _tx_timer_created_ptr, _tx_timer_created_count);

    /* 1. Direct _txm_module_manager_callback_request tests.  */
    memset((VOID *) &msg, 0, sizeof(msg));
    msg.txm_module_callback_message_type = TXM_QUEUE_SEND_CALLBACK;
    msg.txm_module_callback_message_application_function = dummy_app_fn;
    msg.txm_module_callback_message_param_1 = (ALIGN_TYPE) &app_queue;

    memset((VOID *) &bad_queue, 0, sizeof(bad_queue));
    _txm_module_manager_callback_request(&bad_queue, &msg);
    test_expect("callback error count incremented on invalid queue",
                _txm_module_manager_callback_error_count, 1UL);

    _txm_module_manager_callback_request(&(mod_a.txm_module_instance_callback_request_queue), &msg);
    test_expect("callback total count becomes 1", _txm_module_manager_callback_total_count, 1UL);
    test_expect("queue holds 1 enqueued message",
                mod_a.txm_module_instance_callback_request_queue.tx_queue_enqueued, 1UL);

    /* Duplicate message while still queued increments activation_count without growing queue.  */
    _txm_module_manager_callback_request(&(mod_a.txm_module_instance_callback_request_queue), &msg);
    test_expect("queue still holds only 1 message after dedup",
                mod_a.txm_module_instance_callback_request_queue.tx_queue_enqueued, 1UL);
    pop_callback_message(&(mod_a.txm_module_instance_callback_request_queue), &popped);
    test_expect("deduplicated activation_count is 1", popped.txm_module_callback_message_activation_count, 1UL);

    /* Force wrap-around by advancing read/write past middle and filling to capacity.  */
    for (i = 0; i < TXM_MODULE_CALLBACKS_QUEUE_DEPTH; i++)
    {
        msg.txm_module_callback_message_param_2 = (ALIGN_TYPE) (i + 1U);
        _txm_module_manager_callback_request(&(mod_a.txm_module_instance_callback_request_queue), &msg);
    }
    test_expect("callback queue filled to capacity",
                mod_a.txm_module_instance_callback_request_queue.tx_queue_enqueued,
                (ULONG) TXM_MODULE_CALLBACKS_QUEUE_DEPTH);

    /* Now enqueued == TXM_MODULE_CALLBACKS_QUEUE_DEPTH; test dedup after wrap and full error.  */
    msg.txm_module_callback_message_param_2 = (ALIGN_TYPE) TXM_MODULE_CALLBACKS_QUEUE_DEPTH;
    _txm_module_manager_callback_request(&(mod_a.txm_module_instance_callback_request_queue), &msg);
    test_expect("dedup keeps queue depth unchanged on wrapped full ring",
                mod_a.txm_module_instance_callback_request_queue.tx_queue_enqueued,
                (ULONG) TXM_MODULE_CALLBACKS_QUEUE_DEPTH);
    test_expect("callback error count still 1 after dedup",
                _txm_module_manager_callback_error_count, 1UL);

    msg.txm_module_callback_message_param_2 = (ALIGN_TYPE) 9999U;
    _txm_module_manager_callback_request(&(mod_a.txm_module_instance_callback_request_queue), &msg);
    test_expect("callback error count incremented on full queue",
                _txm_module_manager_callback_error_count, 2UL);

    while (mod_a.txm_module_instance_callback_request_queue.tx_queue_enqueued > 0UL)
    {
        pop_callback_message(&(mod_a.txm_module_instance_callback_request_queue), &popped);
    }

    /* 2. Dispatcher notify registration & clear + trampolines across all 4 notify dispatchers + timer trampoline.  */
    ret = _txm_module_manager_kernel_dispatch(TXM_QUEUE_SEND_NOTIFY_CALL, (ALIGN_TYPE) &app_queue, (ALIGN_TYPE) dummy_app_fn, 0);
    test_expect("queue_send_notify dispatch succeeds", (ULONG) ret, TX_SUCCESS);
    test_expect("queue_send_notify installs trampoline", (ALIGN_TYPE) stub_registered_queue_notify == (ALIGN_TYPE) _txm_module_manager_queue_notify_trampoline, 1UL);

    ret = _txm_module_manager_kernel_dispatch(TXM_SEMAPHORE_PUT_NOTIFY_CALL, (ALIGN_TYPE) &app_sem, (ALIGN_TYPE) dummy_app_fn, 0);
    test_expect("semaphore_put_notify dispatch succeeds", (ULONG) ret, TX_SUCCESS);
    test_expect("semaphore_put_notify installs trampoline", (ALIGN_TYPE) stub_registered_sem_notify == (ALIGN_TYPE) _txm_module_manager_semaphore_notify_trampoline, 1UL);

    ret = _txm_module_manager_kernel_dispatch(TXM_EVENT_FLAGS_SET_NOTIFY_CALL, (ALIGN_TYPE) &app_ev, (ALIGN_TYPE) dummy_app_fn, 0);
    test_expect("event_flags_set_notify dispatch succeeds", (ULONG) ret, TX_SUCCESS);
    test_expect("event_flags_set_notify installs trampoline", (ALIGN_TYPE) stub_registered_ev_notify == (ALIGN_TYPE) _txm_module_manager_event_flags_notify_trampoline, 1UL);

    ret = _txm_module_manager_kernel_dispatch(TXM_THREAD_ENTRY_EXIT_NOTIFY_CALL, (ALIGN_TYPE) &app_thread, (ALIGN_TYPE) dummy_app_fn, 0);
    test_expect("thread_entry_exit_notify dispatch succeeds", (ULONG) ret, TX_SUCCESS);
    test_expect("thread_entry_exit_notify installs trampoline", (ALIGN_TYPE) stub_registered_thread_notify == (ALIGN_TYPE) _txm_module_manager_thread_notify_trampoline, 1UL);

    /* Configure timer fields directly as timer create dispatcher does.  */
    app_timer.tx_timer_module_instance = (VOID *) &mod_a;
    app_timer.tx_timer_module_expiration_function = (VOID (*)(ULONG)) dummy_app_fn;
    app_timer.tx_timer_internal.tx_timer_internal_timeout_param = 0xCAFEUL;
    _tx_timer_expired_timer_ptr = &(app_timer.tx_timer_internal);

    /* Fire all 5 trampolines while module is STARTED.  */
    _txm_module_manager_queue_notify_trampoline(&app_queue);
    _txm_module_manager_semaphore_notify_trampoline(&app_sem);
    _txm_module_manager_event_flags_notify_trampoline(&app_ev);
    _txm_module_manager_thread_notify_trampoline(&app_thread, TX_THREAD_EXIT);
    _txm_module_manager_timer_notify_trampoline(0xCAFEUL);
    test_expect("all 5 trampolines enqueued when module STARTED",
                mod_a.txm_module_instance_callback_request_queue.tx_queue_enqueued, 5UL);

    pop_callback_message(&(mod_a.txm_module_instance_callback_request_queue), &popped);
    test_expect("msg 1 type is TXM_QUEUE_SEND_CALLBACK", popped.txm_module_callback_message_type, TXM_QUEUE_SEND_CALLBACK);
    pop_callback_message(&(mod_a.txm_module_instance_callback_request_queue), &popped);
    test_expect("msg 2 type is TXM_SEMAPHORE_PUT_CALLBACK", popped.txm_module_callback_message_type, TXM_SEMAPHORE_PUT_CALLBACK);
    pop_callback_message(&(mod_a.txm_module_instance_callback_request_queue), &popped);
    test_expect("msg 3 type is TXM_EVENTS_SET_CALLBACK", popped.txm_module_callback_message_type, TXM_EVENTS_SET_CALLBACK);
    pop_callback_message(&(mod_a.txm_module_instance_callback_request_queue), &popped);
    test_expect("msg 4 type is TXM_THREAD_ENTRY_EXIT_CALLBACK", popped.txm_module_callback_message_type, TXM_THREAD_ENTRY_EXIT_CALLBACK);
    test_expect("thread exit notify preserves exit type param", (ULONG) popped.txm_module_callback_message_param_2, TX_THREAD_EXIT);
    pop_callback_message(&(mod_a.txm_module_instance_callback_request_queue), &popped);
    test_expect("msg 5 type is TXM_TIMER_CALLBACK", popped.txm_module_callback_message_type, TXM_TIMER_CALLBACK);
    test_expect("timer callback preserves timeout param", (ULONG) popped.txm_module_callback_message_param_1, 0xCAFEUL);

    /* Stopped module must reject all 5 trampolines without enqueueing and increment error_count.  */
    mod_a.txm_module_instance_state = TXM_MODULE_STOPPED;
    _txm_module_manager_queue_notify_trampoline(&app_queue);
    _txm_module_manager_semaphore_notify_trampoline(&app_sem);
    _txm_module_manager_event_flags_notify_trampoline(&app_ev);
    _txm_module_manager_thread_notify_trampoline(&app_thread, TX_THREAD_ENTRY);
    _txm_module_manager_timer_notify_trampoline(0xCAFEUL);
    test_expect("stopped module enqueues 0 trampoline messages",
                mod_a.txm_module_instance_callback_request_queue.tx_queue_enqueued, 0UL);
    test_expect("error count increments 5 times for stopped module",
                _txm_module_manager_callback_error_count, 7UL);

    /* Invalid module instance (cleared ID) and NULL module must also fail safely.  */
    mod_a.txm_module_instance_id = 0UL;
    mod_a.txm_module_instance_state = TXM_MODULE_STARTED;
    _txm_module_manager_queue_notify_trampoline(&app_queue);
    _txm_module_manager_semaphore_notify_trampoline(&app_sem);
    _txm_module_manager_event_flags_notify_trampoline(&app_ev);
    _txm_module_manager_thread_notify_trampoline(&app_thread, TX_THREAD_ENTRY);
    _txm_module_manager_timer_notify_trampoline(0xCAFEUL);
    test_expect("invalid module ID enqueues 0 messages",
                mod_a.txm_module_instance_callback_request_queue.tx_queue_enqueued, 0UL);
    test_expect("error count increments 5 times for invalid module ID",
                _txm_module_manager_callback_error_count, 12UL);

    /* Clearing notifications with param_1 == TX_NULL clears the registered function.  */
    mod_a.txm_module_instance_id = TXM_MODULE_ID;
    test_expect("queue notify clear succeeds",
                (ULONG) _txm_module_manager_kernel_dispatch(TXM_QUEUE_SEND_NOTIFY_CALL, (ALIGN_TYPE) &app_queue, 0, 0), TX_SUCCESS);
    test_expect("queue notify cleared to NULL", (ULONG) (stub_registered_queue_notify == TX_NULL), 1UL);
    test_expect("sem notify clear succeeds",
                (ULONG) _txm_module_manager_kernel_dispatch(TXM_SEMAPHORE_PUT_NOTIFY_CALL, (ALIGN_TYPE) &app_sem, 0, 0), TX_SUCCESS);
    test_expect("sem notify cleared to NULL", (ULONG) (stub_registered_sem_notify == TX_NULL), 1UL);
    test_expect("event flags notify clear succeeds",
                (ULONG) _txm_module_manager_kernel_dispatch(TXM_EVENT_FLAGS_SET_NOTIFY_CALL, (ALIGN_TYPE) &app_ev, 0, 0), TX_SUCCESS);
    test_expect("event flags notify cleared to NULL", (ULONG) (stub_registered_ev_notify == TX_NULL), 1UL);
    test_expect("thread notify clear succeeds",
                (ULONG) _txm_module_manager_kernel_dispatch(TXM_THREAD_ENTRY_EXIT_NOTIFY_CALL, (ALIGN_TYPE) &app_thread, 0, 0), TX_SUCCESS);
    test_expect("thread notify cleared to NULL", (ULONG) (stub_registered_thread_notify == TX_NULL), 1UL);

    /* Invalid dispatch call ID must return TX_NOT_AVAILABLE.  */
    test_expect("unknown kernel dispatch ID fails cleanly",
                (ULONG) _txm_module_manager_kernel_dispatch(0x9999UL, 0, 0, 0), (ULONG) TX_NOT_AVAILABLE);

    /* Interrupt balance verification.  */
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
