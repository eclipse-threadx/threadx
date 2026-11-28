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

/* Interrupt lock accounting expected by the host port shim.  */
unsigned int    test_interrupt_disable_depth;
unsigned int    test_interrupt_disable_max_depth;
unsigned int    test_interrupt_restore_underflows;

unsigned int    test_get_ipsr_value(void)
{
    return(0U);
}

/* Kernel object created lists required by util and object_pointer_get_extended.  */
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

/* Module Manager bookkeeping required by dispatch/object helpers.  */
volatile ULONG          _tx_thread_system_state;
volatile UINT           _tx_thread_preempt_disable;
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

/* Backed object pool arena so allocated objects live inside the real object pool range.  */
#define SEC_POOL_BYTES  8192U
#define SEC_MAX_ALLOCS  32U

typedef struct SEC_ALLOC_SLOT_STRUCT
{
    UCHAR   *ptr;
    ULONG   size;
    UINT    used;
} SEC_ALLOC_SLOT;

static union
{
    ALIGN_TYPE  align;
    UCHAR       bytes[SEC_POOL_BYTES];
} sec_pool_mem;

static ULONG            sec_pool_offset;
static SEC_ALLOC_SLOT   sec_slots[SEC_MAX_ALLOCS];

static UINT     test_failures;
static ULONG    test_checks;
static ULONG    stub_int_ctl_calls;
static ULONG    stub_stack_err_calls;
static ULONG    stub_trace_calls;
static ULONG    stub_byte_release_calls;

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

VOID  _txm_module_manager_callback_request(TX_QUEUE *module_callback_queue, TXM_MODULE_CALLBACK_MESSAGE *callback_request)
{
    (VOID) module_callback_queue;
    (VOID) callback_request;
}

VOID  _tx_thread_system_preempt_check(VOID)
{
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

UINT  _txe_byte_allocate(TX_BYTE_POOL *pool_ptr, VOID **memory_ptr, ULONG memory_size, ULONG wait_option)
{
ULONG   rounded = (memory_size + (ULONG)(sizeof(ALIGN_TYPE) - 1U)) & ~((ULONG)(sizeof(ALIGN_TYPE) - 1U));
UCHAR   *p;
UINT    i;

    (VOID) wait_option;
    if ((pool_ptr != &_txm_module_manager_object_pool) || (memory_ptr == TX_NULL))
    {
        return(TX_PTR_ERROR);
    }
    if ((rounded == 0UL) || ((sec_pool_offset + rounded) > SEC_POOL_BYTES))
    {
        return(TX_NO_MEMORY);
    }
    p = &sec_pool_mem.bytes[sec_pool_offset];
    sec_pool_offset += rounded;
    for (i = 0; i < SEC_MAX_ALLOCS; i++)
    {
        if (!sec_slots[i].used)
        {
            sec_slots[i].ptr = p;
            sec_slots[i].size = rounded;
            sec_slots[i].used = 1U;
            break;
        }
    }
    *memory_ptr = (VOID *) p;
    return(TX_SUCCESS);
}

UINT  _txe_byte_release(VOID *memory_ptr)
{
UINT    i;

    stub_byte_release_calls++;
    for (i = 0; i < SEC_MAX_ALLOCS; i++)
    {
        if (sec_slots[i].used && (sec_slots[i].ptr == (UCHAR *) memory_ptr))
        {
            sec_slots[i].used = 0U;
            return(TX_SUCCESS);
        }
    }
    /* Non-object-pool user pointer release passes through in TXM_BYTE_RELEASE_CALL test.  */
    return(TX_SUCCESS);
}

/* Kernel service stubs for dispatchers under test.  */
UINT  _tx_thread_interrupt_control(UINT new_posture)
{
    stub_int_ctl_calls++;
    return(new_posture);
}

UINT  _tx_thread_stack_error_notify(VOID (*stack_error_handler)(TX_THREAD *thread_ptr))
{
    (VOID) stack_error_handler;
    stub_stack_err_calls++;
    return(TX_SUCCESS);
}

UINT  _tx_trace_enable(VOID *trace_buffer_start, ULONG trace_buffer_size, ULONG registry_entries)
{
    (VOID) trace_buffer_start;
    (VOID) trace_buffer_size;
    (VOID) registry_entries;
    stub_trace_calls++;
    return(TX_SUCCESS);
}

UINT  _tx_trace_event_filter(ULONG event_filter_bits)
{
    (VOID) event_filter_bits;
    stub_trace_calls++;
    return(TX_SUCCESS);
}

UINT  _tx_trace_event_unfilter(ULONG event_unfilter_bits)
{
    (VOID) event_unfilter_bits;
    stub_trace_calls++;
    return(TX_SUCCESS);
}

UINT  _tx_trace_disable(VOID)
{
    stub_trace_calls++;
    return(TX_SUCCESS);
}

UINT  _tx_trace_interrupt_control(UINT new_posture)
{
    (VOID) new_posture;
    stub_trace_calls++;
    return(TX_SUCCESS);
}

VOID  _tx_trace_isr_enter_insert(ULONG isr_id)
{
    (VOID) isr_id;
    stub_trace_calls++;
}

VOID  _tx_trace_isr_exit_insert(ULONG isr_id)
{
    (VOID) isr_id;
    stub_trace_calls++;
}

UINT  _tx_trace_user_event_insert(ULONG event_id, ULONG info_1, ULONG info_2, ULONG info_3, ULONG info_4)
{
    (VOID) event_id;
    (VOID) info_1;
    (VOID) info_2;
    (VOID) info_3;
    (VOID) info_4;
    stub_trace_calls++;
    return(TX_SUCCESS);
}

#define REGISTER_NODE(node, next_f, prev_f, head, cnt) \
    do {                                               \
        if ((head) == TX_NULL)                         \
        {                                              \
            (head) = (node);                           \
            (node) -> next_f = (node);                 \
            (node) -> prev_f = (node);                 \
        }                                              \
        else                                           \
        {                                              \
            (node) -> next_f = (head);                 \
            (node) -> prev_f = (head) -> prev_f;       \
            (head) -> prev_f -> next_f = (node);       \
            (head) -> prev_f = (node);                 \
        }                                              \
        (cnt)++;                                       \
    } while (0)

#define UNREGISTER_NODE(node, next_f, prev_f, head, cnt) \
    do {                                                 \
        if ((cnt) <= 1UL)                                \
        {                                                \
            (head) = TX_NULL;                            \
            (cnt) = 0UL;                                 \
        }                                                \
        else                                             \
        {                                                \
            (node) -> next_f -> prev_f = (node) -> prev_f; \
            (node) -> prev_f -> next_f = (node) -> next_f; \
            if ((head) == (node)) (head) = (node) -> next_f; \
            (cnt)--;                                     \
        }                                                \
    } while (0)

UINT  _txe_byte_pool_create(TX_BYTE_POOL *pool_ptr, CHAR *name_ptr, VOID *pool_start, ULONG pool_size, UINT cb_size)
{
    (VOID) pool_start;
    (VOID) pool_size;
    (VOID) cb_size;
    memset((VOID *) pool_ptr, 0, sizeof(TX_BYTE_POOL));
    pool_ptr -> tx_byte_pool_id = TX_BYTE_POOL_ID;
    pool_ptr -> tx_byte_pool_name = name_ptr;
    REGISTER_NODE(pool_ptr, tx_byte_pool_created_next, tx_byte_pool_created_previous, _tx_byte_pool_created_ptr, _tx_byte_pool_created_count);
    return(TX_SUCCESS);
}

UINT  _txe_byte_pool_delete(TX_BYTE_POOL *pool_ptr)
{
    UNREGISTER_NODE(pool_ptr, tx_byte_pool_created_next, tx_byte_pool_created_previous, _tx_byte_pool_created_ptr, _tx_byte_pool_created_count);
    pool_ptr -> tx_byte_pool_id = 0UL;
    return(TX_SUCCESS);
}

UINT  _txe_timer_create(TX_TIMER *timer_ptr, CHAR *name_ptr, VOID (*exp_fn)(ULONG), ULONG exp_in,
                        ULONG init_ticks, ULONG resched_ticks, UINT auto_act, UINT cb_size)
{
    (VOID) exp_fn;
    (VOID) exp_in;
    (VOID) init_ticks;
    (VOID) resched_ticks;
    (VOID) auto_act;
    (VOID) cb_size;
    memset((VOID *) timer_ptr, 0, sizeof(TX_TIMER));
    timer_ptr -> tx_timer_id = TX_TIMER_ID;
    timer_ptr -> tx_timer_name = name_ptr;
    REGISTER_NODE(timer_ptr, tx_timer_created_next, tx_timer_created_previous, _tx_timer_created_ptr, _tx_timer_created_count);
    return(TX_SUCCESS);
}

UINT  _txe_timer_activate(TX_TIMER *timer_ptr)
{
    (VOID) timer_ptr;
    return(TX_SUCCESS);
}

UINT  _txe_timer_deactivate(TX_TIMER *timer_ptr)
{
    (VOID) timer_ptr;
    return(TX_SUCCESS);
}

UINT  _txe_timer_change(TX_TIMER *timer_ptr, ULONG init_ticks, ULONG resched_ticks)
{
    (VOID) timer_ptr;
    (VOID) init_ticks;
    (VOID) resched_ticks;
    return(TX_SUCCESS);
}

UINT  _txe_timer_delete(TX_TIMER *timer_ptr)
{
    UNREGISTER_NODE(timer_ptr, tx_timer_created_next, tx_timer_created_previous, _tx_timer_created_ptr, _tx_timer_created_count);
    timer_ptr -> tx_timer_id = 0UL;
    return(TX_SUCCESS);
}

UINT  _txe_mutex_create(TX_MUTEX *mutex_ptr, CHAR *name_ptr, UINT inherit, UINT cb_size)
{
    (VOID) inherit;
    (VOID) cb_size;
    memset((VOID *) mutex_ptr, 0, sizeof(TX_MUTEX));
    mutex_ptr -> tx_mutex_id = TX_MUTEX_ID;
    mutex_ptr -> tx_mutex_name = name_ptr;
    REGISTER_NODE(mutex_ptr, tx_mutex_created_next, tx_mutex_created_previous, _tx_mutex_created_ptr, _tx_mutex_created_count);
    return(TX_SUCCESS);
}

UINT  _txe_mutex_prioritize(TX_MUTEX *mutex_ptr)
{
    (VOID) mutex_ptr;
    return(TX_SUCCESS);
}

UINT  _txe_mutex_delete(TX_MUTEX *mutex_ptr)
{
    UNREGISTER_NODE(mutex_ptr, tx_mutex_created_next, tx_mutex_created_previous, _tx_mutex_created_ptr, _tx_mutex_created_count);
    mutex_ptr -> tx_mutex_id = 0UL;
    return(TX_SUCCESS);
}

UINT  _txe_semaphore_create(TX_SEMAPHORE *sem_ptr, CHAR *name_ptr, ULONG initial_count, UINT cb_size)
{
    (VOID) initial_count;
    (VOID) cb_size;
    memset((VOID *) sem_ptr, 0, sizeof(TX_SEMAPHORE));
    sem_ptr -> tx_semaphore_id = TX_SEMAPHORE_ID;
    sem_ptr -> tx_semaphore_name = name_ptr;
    REGISTER_NODE(sem_ptr, tx_semaphore_created_next, tx_semaphore_created_previous, _tx_semaphore_created_ptr, _tx_semaphore_created_count);
    return(TX_SUCCESS);
}

UINT  _txe_semaphore_ceiling_put(TX_SEMAPHORE *sem_ptr, ULONG ceiling)
{
    (VOID) sem_ptr;
    (VOID) ceiling;
    return(TX_SUCCESS);
}

UINT  _txe_semaphore_get(TX_SEMAPHORE *sem_ptr, ULONG wait_option)
{
    (VOID) sem_ptr;
    (VOID) wait_option;
    return(TX_SUCCESS);
}

UINT  _txe_semaphore_put(TX_SEMAPHORE *sem_ptr)
{
    (VOID) sem_ptr;
    return(TX_SUCCESS);
}

UINT  _txe_semaphore_delete(TX_SEMAPHORE *sem_ptr)
{
    UNREGISTER_NODE(sem_ptr, tx_semaphore_created_next, tx_semaphore_created_previous, _tx_semaphore_created_ptr, _tx_semaphore_created_count);
    sem_ptr -> tx_semaphore_id = 0UL;
    return(TX_SUCCESS);
}

UINT  _txe_event_flags_create(TX_EVENT_FLAGS_GROUP *group_ptr, CHAR *name_ptr, UINT cb_size)
{
    (VOID) cb_size;
    memset((VOID *) group_ptr, 0, sizeof(TX_EVENT_FLAGS_GROUP));
    group_ptr -> tx_event_flags_group_id = TX_EVENT_FLAGS_ID;
    group_ptr -> tx_event_flags_group_name = name_ptr;
    REGISTER_NODE(group_ptr, tx_event_flags_group_created_next, tx_event_flags_group_created_previous, _tx_event_flags_created_ptr, _tx_event_flags_created_count);
    return(TX_SUCCESS);
}

UINT  _txe_event_flags_set(TX_EVENT_FLAGS_GROUP *group_ptr, ULONG flags, UINT set_option)
{
    (VOID) group_ptr;
    (VOID) flags;
    (VOID) set_option;
    return(TX_SUCCESS);
}

UINT  _txe_event_flags_get(TX_EVENT_FLAGS_GROUP *group_ptr, ULONG requested, UINT get_option, ULONG *actual_ptr, ULONG wait_option)
{
    (VOID) group_ptr;
    (VOID) get_option;
    (VOID) wait_option;
    if (actual_ptr != TX_NULL)
    {
        *actual_ptr = requested;
    }
    return(TX_SUCCESS);
}

UINT  _txe_event_flags_delete(TX_EVENT_FLAGS_GROUP *group_ptr)
{
    UNREGISTER_NODE(group_ptr, tx_event_flags_group_created_next, tx_event_flags_group_created_previous, _tx_event_flags_created_ptr, _tx_event_flags_created_count);
    group_ptr -> tx_event_flags_group_id = 0UL;
    return(TX_SUCCESS);
}

/* Memory layout for module data, code, and outside host buffers.  */
typedef struct SEC_MODULE_DATA_STRUCT
{
    ALIGN_TYPE  extra[8];
    UCHAR       pool_buf[256];
    CHAR        name_buf[32];
    ULONG       out_ulong;
    VOID        *out_ptr;
} SEC_MODULE_DATA;

static SEC_MODULE_DATA      mod_rw;
static SEC_MODULE_DATA      outside_rw;
static UCHAR                mod_ro[128];
static TXM_MODULE_INSTANCE  user_mod;
static TXM_MODULE_INSTANCE  priv_mod;
static TX_THREAD            cur_thread;
static TX_BLOCK_POOL        app_block_pool;
static TX_BYTE_POOL         app_byte_pool;
static TX_QUEUE             app_queue;
static TX_THREAD            app_thread;

static VOID dummy_handler(TX_THREAD *t)
{
    (VOID) t;
}

static VOID dummy_timer_cb(ULONG id)
{
    (VOID) id;
}

static VOID setup_mod_instance(TXM_MODULE_INSTANCE *m, ULONG props)
{
    memset((VOID *) m, 0, sizeof(TXM_MODULE_INSTANCE));
    m -> txm_module_instance_id = TXM_MODULE_ID;
    m -> txm_module_instance_state = TXM_MODULE_STARTED;
    m -> txm_module_instance_property_flags = props;
    m -> txm_module_instance_data_start = (VOID *) &mod_rw;
    m -> txm_module_instance_data_end = (VOID *) (((UCHAR *) &mod_rw) + sizeof(mod_rw) - 1U);
    m -> txm_module_instance_code_start = (VOID *) mod_ro;
    m -> txm_module_instance_code_end = (VOID *) (mod_ro + sizeof(mod_ro) - 1U);
}

int main(void)
{
VOID        *obj_timer = TX_NULL;
VOID        *obj_mutex = TX_NULL;
VOID        *obj_sem = TX_NULL;
VOID        *obj_ev = TX_NULL;
VOID        *obj_bpool = TX_NULL;
VOID        *found_ptr = TX_NULL;
ALIGN_TYPE  res;

    _tx_thread_system_state = 0UL;
    memset((VOID *) &cur_thread, 0, sizeof(cur_thread));
    _tx_thread_current_ptr = &cur_thread;

    memset((VOID *) &_txm_module_manager_object_pool, 0, sizeof(_txm_module_manager_object_pool));
    _txm_module_manager_object_pool.tx_byte_pool_id = TX_BYTE_POOL_ID;
    _txm_module_manager_object_pool.tx_byte_pool_start = sec_pool_mem.bytes;
    _txm_module_manager_object_pool.tx_byte_pool_size = SEC_POOL_BYTES;
    _txm_module_manager_object_pool.tx_byte_pool_available = SEC_POOL_BYTES;
    _txm_module_manager_object_pool_created = TX_TRUE;

    setup_mod_instance(&user_mod, (TXM_MODULE_USER_MODE | TXM_MODULE_MEMORY_PROTECTION));
    setup_mod_instance(&priv_mod, 0UL);
    cur_thread.tx_thread_module_instance_ptr = &user_mod;

    /* 1. User-mode privileged call restrictions vs privileged module allowance.  */
    test_expect("user module refused TXM_THREAD_INTERRUPT_CONTROL_CALL",
                (ULONG) _txm_module_manager_kernel_dispatch(TXM_THREAD_INTERRUPT_CONTROL_CALL, TX_INT_DISABLE, 0, 0),
                TXM_MODULE_INVALID_PROPERTIES);
    test_expect("user module refused TXM_THREAD_STACK_ERROR_NOTIFY_CALL",
                (ULONG) _txm_module_manager_kernel_dispatch(TXM_THREAD_STACK_ERROR_NOTIFY_CALL, (ALIGN_TYPE) dummy_handler, 0, 0),
                TXM_MODULE_INVALID_PROPERTIES);
    test_expect("user module refused TXM_TRACE_ENABLE_CALL",
                (ULONG) _txm_module_manager_kernel_dispatch(TXM_TRACE_ENABLE_CALL, (ALIGN_TYPE) mod_rw.pool_buf, 128, 8),
                TXM_MODULE_INVALID_PROPERTIES);
    test_expect("user module refused TXM_TRACE_INTERRUPT_CONTROL_CALL",
                (ULONG) _txm_module_manager_kernel_dispatch(TXM_TRACE_INTERRUPT_CONTROL_CALL, 0, 0, 0),
                TXM_MODULE_INVALID_PROPERTIES);
    test_expect("user module refused TXM_TRACE_ISR_ENTER_INSERT_CALL",
                (ULONG) _txm_module_manager_kernel_dispatch(TXM_TRACE_ISR_ENTER_INSERT_CALL, 1, 0, 0),
                TXM_MODULE_INVALID_PROPERTIES);
    test_expect("user module refused TXM_TRACE_ISR_EXIT_INSERT_CALL",
                (ULONG) _txm_module_manager_kernel_dispatch(TXM_TRACE_ISR_EXIT_INSERT_CALL, 1, 0, 0),
                TXM_MODULE_INVALID_PROPERTIES);
    test_expect("interrupt/stack/trace kernel stubs never reached by user module",
                stub_int_ctl_calls + stub_stack_err_calls + stub_trace_calls, 0UL);

    /* TXM_TRACE_USER_EVENT_INSERT_CALL is allowed in user mode, but validates extra_parameters.  */
    test_expect("trace_user_event rejects outside extra_parameters",
                (ULONG) _txm_module_manager_kernel_dispatch(TXM_TRACE_USER_EVENT_INSERT_CALL, 10, 20, (ALIGN_TYPE) outside_rw.extra),
                TXM_MODULE_INVALID_MEMORY);
    mod_rw.extra[0] = 1;
    mod_rw.extra[1] = 2;
    mod_rw.extra[2] = 3;
    test_expect("trace_user_event accepts inside extra_parameters",
                (ULONG) _txm_module_manager_kernel_dispatch(TXM_TRACE_USER_EVENT_INSERT_CALL, 10, 20, (ALIGN_TYPE) mod_rw.extra),
                TX_SUCCESS);

    /* Privileged module can invoke all restricted services.  */
    cur_thread.tx_thread_module_instance_ptr = &priv_mod;
    test_expect("priv module allowed TXM_THREAD_INTERRUPT_CONTROL_CALL",
                (ULONG) _txm_module_manager_kernel_dispatch(TXM_THREAD_INTERRUPT_CONTROL_CALL, 0, 0, 0), 0UL);
    test_expect("priv module allowed TXM_THREAD_STACK_ERROR_NOTIFY_CALL",
                (ULONG) _txm_module_manager_kernel_dispatch(TXM_THREAD_STACK_ERROR_NOTIFY_CALL, (ALIGN_TYPE) dummy_handler, 0, 0),
                TX_SUCCESS);
    test_expect("priv module allowed TXM_TRACE_ENABLE_CALL",
                (ULONG) _txm_module_manager_kernel_dispatch(TXM_TRACE_ENABLE_CALL, (ALIGN_TYPE) mod_rw.pool_buf, 128, 8),
                TX_SUCCESS);
    test_expect("priv module allowed TXM_TRACE_INTERRUPT_CONTROL_CALL",
                (ULONG) _txm_module_manager_kernel_dispatch(TXM_TRACE_INTERRUPT_CONTROL_CALL, 0, 0, 0), TX_SUCCESS);
    test_expect("priv module allowed TXM_TRACE_ISR_ENTER_INSERT_CALL",
                (ULONG) _txm_module_manager_kernel_dispatch(TXM_TRACE_ISR_ENTER_INSERT_CALL, 1, 0, 0), TX_SUCCESS);
    test_expect("priv module allowed TXM_TRACE_ISR_EXIT_INSERT_CALL",
                (ULONG) _txm_module_manager_kernel_dispatch(TXM_TRACE_ISR_EXIT_INSERT_CALL, 1, 0, 0), TX_SUCCESS);

    /* 2. Unknown & application-range dispatch requests return TX_NOT_AVAILABLE cleanly.  */
    cur_thread.tx_thread_module_instance_ptr = &user_mod;
    test_expect("TXM_APPLICATION_REQUEST_ID_BASE forwards cleanly",
                (ULONG) _txm_module_manager_kernel_dispatch(TXM_APPLICATION_REQUEST_ID_BASE, 1, 2, 3), TX_NOT_AVAILABLE);

    /* 3. Object allocate & Timer/Mutex/Semaphore/EventFlags/BytePool create/use/delete + pointer checks.  */
    test_expect("object allocate refuses outside destination pointer",
                (ULONG) _txm_module_manager_kernel_dispatch(TXM_MODULE_OBJECT_ALLOCATE_CALL, (ALIGN_TYPE) &outside_rw.out_ptr, sizeof(TX_TIMER), 0),
                TXM_MODULE_INVALID_MEMORY);

    test_expect("allocate TX_TIMER inside pool",
                (ULONG) _txm_module_manager_kernel_dispatch(TXM_MODULE_OBJECT_ALLOCATE_CALL, (ALIGN_TYPE) &mod_rw.out_ptr, sizeof(TX_TIMER), 0),
                TX_SUCCESS);
    obj_timer = mod_rw.out_ptr;

    test_expect("allocate TX_MUTEX inside pool",
                (ULONG) _txm_module_manager_kernel_dispatch(TXM_MODULE_OBJECT_ALLOCATE_CALL, (ALIGN_TYPE) &mod_rw.out_ptr, sizeof(TX_MUTEX), 0),
                TX_SUCCESS);
    obj_mutex = mod_rw.out_ptr;

    test_expect("allocate TX_SEMAPHORE inside pool",
                (ULONG) _txm_module_manager_kernel_dispatch(TXM_MODULE_OBJECT_ALLOCATE_CALL, (ALIGN_TYPE) &mod_rw.out_ptr, sizeof(TX_SEMAPHORE), 0),
                TX_SUCCESS);
    obj_sem = mod_rw.out_ptr;

    test_expect("allocate TX_EVENT_FLAGS_GROUP inside pool",
                (ULONG) _txm_module_manager_kernel_dispatch(TXM_MODULE_OBJECT_ALLOCATE_CALL, (ALIGN_TYPE) &mod_rw.out_ptr, sizeof(TX_EVENT_FLAGS_GROUP), 0),
                TX_SUCCESS);
    obj_ev = mod_rw.out_ptr;

    test_expect("allocate TX_BYTE_POOL inside pool",
                (ULONG) _txm_module_manager_kernel_dispatch(TXM_MODULE_OBJECT_ALLOCATE_CALL, (ALIGN_TYPE) &mod_rw.out_ptr, sizeof(TX_BYTE_POOL), 0),
                TX_SUCCESS);
    obj_bpool = mod_rw.out_ptr;

    strcpy(mod_rw.name_buf, "sec_obj");

    /* Timer create/activate/change/deactivate & extra_parameters rejection.  */
    test_expect("timer_create rejects outside extra_parameters",
                (ULONG) _txm_module_manager_kernel_dispatch(TXM_TIMER_CREATE_CALL, (ALIGN_TYPE) obj_timer, (ALIGN_TYPE) mod_rw.name_buf, (ALIGN_TYPE) outside_rw.extra),
                TXM_MODULE_INVALID_MEMORY);
    mod_rw.extra[0] = (ALIGN_TYPE) dummy_timer_cb;
    mod_rw.extra[1] = 1UL;
    mod_rw.extra[2] = 5UL;
    mod_rw.extra[3] = 5UL;
    mod_rw.extra[4] = TX_AUTO_ACTIVATE;
    mod_rw.extra[5] = sizeof(TX_TIMER);
    test_expect("timer_create succeeds",
                (ULONG) _txm_module_manager_kernel_dispatch(TXM_TIMER_CREATE_CALL, (ALIGN_TYPE) obj_timer, (ALIGN_TYPE) mod_rw.name_buf, (ALIGN_TYPE) mod_rw.extra),
                TX_SUCCESS);
    test_expect("timer_activate succeeds",
                (ULONG) _txm_module_manager_kernel_dispatch(TXM_TIMER_ACTIVATE_CALL, (ALIGN_TYPE) obj_timer, 0, 0), TX_SUCCESS);
    test_expect("timer_change rejects mutex CB (type check)",
                (ULONG) _txm_module_manager_kernel_dispatch(TXM_TIMER_CHANGE_CALL, (ALIGN_TYPE) obj_mutex, 10, 20),
                TXM_MODULE_INVALID_MEMORY);
    test_expect("timer_change succeeds on timer object",
                (ULONG) _txm_module_manager_kernel_dispatch(TXM_TIMER_CHANGE_CALL, (ALIGN_TYPE) obj_timer, 10, 20), TX_SUCCESS);
    test_expect("timer_deactivate succeeds",
                (ULONG) _txm_module_manager_kernel_dispatch(TXM_TIMER_DEACTIVATE_CALL, (ALIGN_TYPE) obj_timer, 0, 0), TX_SUCCESS);

    /* Mutex, Semaphore, Event Flags create & use + type-confusion checks.  */
    mod_rw.extra[0] = TX_INHERIT;
    mod_rw.extra[1] = sizeof(TX_MUTEX);
    test_expect("mutex_create succeeds",
                (ULONG) _txm_module_manager_kernel_dispatch(TXM_MUTEX_CREATE_CALL, (ALIGN_TYPE) obj_mutex, (ALIGN_TYPE) mod_rw.name_buf, (ALIGN_TYPE) mod_rw.extra),
                TX_SUCCESS);
    test_expect("mutex_put rejects timer control block (type check)",
                (ULONG) _txm_module_manager_kernel_dispatch(TXM_MUTEX_PUT_CALL, (ALIGN_TYPE) obj_timer, 0, 0),
                TXM_MODULE_INVALID_MEMORY);
    test_expect("mutex_get succeeds",
                (ULONG) _txm_module_manager_kernel_dispatch(TXM_MUTEX_GET_CALL, (ALIGN_TYPE) obj_mutex, TX_NO_WAIT, 0), TX_SUCCESS);
    test_expect("mutex_prioritize succeeds",
                (ULONG) _txm_module_manager_kernel_dispatch(TXM_MUTEX_PRIORITIZE_CALL, (ALIGN_TYPE) obj_mutex, 0, 0), TX_SUCCESS);
    test_expect("mutex_put succeeds",
                (ULONG) _txm_module_manager_kernel_dispatch(TXM_MUTEX_PUT_CALL, (ALIGN_TYPE) obj_mutex, 0, 0), TX_SUCCESS);

    mod_rw.extra[0] = 1UL;
    mod_rw.extra[1] = sizeof(TX_SEMAPHORE);
    test_expect("semaphore_create succeeds",
                (ULONG) _txm_module_manager_kernel_dispatch(TXM_SEMAPHORE_CREATE_CALL, (ALIGN_TYPE) obj_sem, (ALIGN_TYPE) mod_rw.name_buf, (ALIGN_TYPE) mod_rw.extra),
                TX_SUCCESS);
    test_expect("semaphore_ceiling_put rejects mutex CB",
                (ULONG) _txm_module_manager_kernel_dispatch(TXM_SEMAPHORE_CEILING_PUT_CALL, (ALIGN_TYPE) obj_mutex, 2, 0),
                TXM_MODULE_INVALID_MEMORY);
    test_expect("semaphore_ceiling_put succeeds",
                (ULONG) _txm_module_manager_kernel_dispatch(TXM_SEMAPHORE_CEILING_PUT_CALL, (ALIGN_TYPE) obj_sem, 2, 0), TX_SUCCESS);
    test_expect("semaphore_get succeeds",
                (ULONG) _txm_module_manager_kernel_dispatch(TXM_SEMAPHORE_GET_CALL, (ALIGN_TYPE) obj_sem, TX_NO_WAIT, 0), TX_SUCCESS);
    test_expect("semaphore_put succeeds",
                (ULONG) _txm_module_manager_kernel_dispatch(TXM_SEMAPHORE_PUT_CALL, (ALIGN_TYPE) obj_sem, 0, 0), TX_SUCCESS);

    test_expect("event_flags_create succeeds",
                (ULONG) _txm_module_manager_kernel_dispatch(TXM_EVENT_FLAGS_CREATE_CALL, (ALIGN_TYPE) obj_ev, (ALIGN_TYPE) mod_rw.name_buf, sizeof(TX_EVENT_FLAGS_GROUP)),
                TX_SUCCESS);
    test_expect("event_flags_set succeeds",
                (ULONG) _txm_module_manager_kernel_dispatch(TXM_EVENT_FLAGS_SET_CALL, (ALIGN_TYPE) obj_ev, 0x05, TX_OR), TX_SUCCESS);
    mod_rw.extra[0] = TX_OR;
    mod_rw.extra[1] = (ALIGN_TYPE) &mod_rw.out_ulong;
    mod_rw.extra[2] = TX_NO_WAIT;
    test_expect("event_flags_get rejects outside actual_events_ptr",
                (ULONG) (mod_rw.extra[1] = (ALIGN_TYPE) &outside_rw.out_ulong,
                         _txm_module_manager_kernel_dispatch(TXM_EVENT_FLAGS_GET_CALL, (ALIGN_TYPE) obj_ev, 0x05, (ALIGN_TYPE) mod_rw.extra)),
                TXM_MODULE_INVALID_MEMORY);
    mod_rw.extra[1] = (ALIGN_TYPE) &mod_rw.out_ulong;
    test_expect("event_flags_get succeeds with inside output pointer",
                (ULONG) _txm_module_manager_kernel_dispatch(TXM_EVENT_FLAGS_GET_CALL, (ALIGN_TYPE) obj_ev, 0x05, (ALIGN_TYPE) mod_rw.extra),
                TX_SUCCESS);

    /* Byte pool create & TXM_BYTE_RELEASE_CALL underflow/bounds checks.  */
    mod_rw.extra[0] = (ALIGN_TYPE) mod_rw.pool_buf;
    mod_rw.extra[1] = sizeof(mod_rw.pool_buf);
    mod_rw.extra[2] = sizeof(TX_BYTE_POOL);
    test_expect("byte_pool_create succeeds",
                (ULONG) _txm_module_manager_kernel_dispatch(TXM_BYTE_POOL_CREATE_CALL, (ALIGN_TYPE) obj_bpool, (ALIGN_TYPE) mod_rw.name_buf, (ALIGN_TYPE) mod_rw.extra),
                TX_SUCCESS);
    stub_byte_release_calls = 0UL;
    test_expect("byte_release rejects integer-underflowing tiny address (< 2 words)",
                (ULONG) _txm_module_manager_kernel_dispatch(TXM_BYTE_RELEASE_CALL, (ALIGN_TYPE) sizeof(ALIGN_TYPE), 0, 0),
                TXM_MODULE_INVALID_MEMORY);
    test_expect("byte_release rejects block header straddling module data start",
                (ULONG) _txm_module_manager_kernel_dispatch(TXM_BYTE_RELEASE_CALL, (ALIGN_TYPE) (&mod_rw), 0, 0),
                TXM_MODULE_INVALID_MEMORY);
    test_expect("byte_release accepts address with 2-word header inside module data",
                (ULONG) _txm_module_manager_kernel_dispatch(TXM_BYTE_RELEASE_CALL, (ALIGN_TYPE) (&mod_rw.pool_buf[2U * sizeof(ALIGN_TYPE)]), 0, 0),
                TX_SUCCESS);
    test_expect("rejected byte_release calls never reached _txe_byte_release", stub_byte_release_calls, 1UL);

    /* 4. Object pointer get & extended security tests (app-owned + module-owned + protection boundary).  */
    memset((VOID *) &app_queue, 0, sizeof(app_queue));
    app_queue.tx_queue_id = TX_QUEUE_ID;
    app_queue.tx_queue_name = "app_q";
    REGISTER_NODE(&app_queue, tx_queue_created_next, tx_queue_created_previous, _tx_queue_created_ptr, _tx_queue_created_count);

    memset((VOID *) &app_thread, 0, sizeof(app_thread));
    app_thread.tx_thread_id = TX_THREAD_ID;
    app_thread.tx_thread_name = "app_t";
    REGISTER_NODE(&app_thread, tx_thread_created_next, tx_thread_created_previous, _tx_thread_created_ptr, _tx_thread_created_count);

    memset((VOID *) &app_block_pool, 0, sizeof(app_block_pool));
    app_block_pool.tx_block_pool_id = TX_BLOCK_POOL_ID;
    app_block_pool.tx_block_pool_name = "app_bp";
    REGISTER_NODE(&app_block_pool, tx_block_pool_created_next, tx_block_pool_created_previous, _tx_block_pool_created_ptr, _tx_block_pool_created_count);

    memset((VOID *) &app_byte_pool, 0, sizeof(app_byte_pool));
    app_byte_pool.tx_byte_pool_id = TX_BYTE_POOL_ID;
    app_byte_pool.tx_byte_pool_name = "app_byp";
    REGISTER_NODE(&app_byte_pool, tx_byte_pool_created_next, tx_byte_pool_created_previous, _tx_byte_pool_created_ptr, _tx_byte_pool_created_count);

    /* Memory-protected module CANNOT look up block pools or byte pools.  */
    strcpy(mod_rw.name_buf, "app_bp");
    res = _txm_module_manager_kernel_dispatch(TXM_MODULE_OBJECT_POINTER_GET_EXTENDED_CALL, TXM_BLOCK_POOL_OBJECT, (ALIGN_TYPE) mod_rw.name_buf, (ALIGN_TYPE) (mod_rw.extra[0] = 6U, mod_rw.extra[1] = (ALIGN_TYPE) &mod_rw.out_ptr, (ALIGN_TYPE) mod_rw.extra));
    test_expect("protected module denied TXM_BLOCK_POOL_OBJECT lookup", (ULONG) res, TXM_MODULE_INVALID);

    strcpy(mod_rw.name_buf, "sec_obj");
    res = _txm_module_manager_kernel_dispatch(TXM_MODULE_OBJECT_POINTER_GET_EXTENDED_CALL, TXM_BYTE_POOL_OBJECT, (ALIGN_TYPE) mod_rw.name_buf, (ALIGN_TYPE) (mod_rw.extra[0] = 7U, mod_rw.extra[1] = (ALIGN_TYPE) &mod_rw.out_ptr, (ALIGN_TYPE) mod_rw.extra));
    test_expect("protected module denied TXM_BYTE_POOL_OBJECT lookup", (ULONG) res, TXM_MODULE_INVALID);

    strcpy(mod_rw.name_buf, "app_byp");
    res = _txm_module_manager_kernel_dispatch(TXM_MODULE_OBJECT_POINTER_GET_EXTENDED_CALL, TXM_BYTE_POOL_OBJECT, (ALIGN_TYPE) mod_rw.name_buf, (ALIGN_TYPE) (mod_rw.extra[0] = 7U, mod_rw.extra[1] = (ALIGN_TYPE) &mod_rw.out_ptr, (ALIGN_TYPE) mod_rw.extra));
    test_expect("protected module denied app-owned TXM_BYTE_POOL_OBJECT lookup", (ULONG) res, TXM_MODULE_INVALID);

    /* Out-of-module output pointer / out-of-module name / length overflow refused.  */
    mod_rw.extra[0] = 5U;
    mod_rw.extra[1] = (ALIGN_TYPE) &outside_rw.out_ptr;
    test_expect("object_pointer_get_extended rejects outside output pointer",
                (ULONG) _txm_module_manager_kernel_dispatch(TXM_MODULE_OBJECT_POINTER_GET_EXTENDED_CALL, TXM_QUEUE_OBJECT, (ALIGN_TYPE) mod_rw.name_buf, (ALIGN_TYPE) mod_rw.extra),
                TXM_MODULE_INVALID_MEMORY);
    mod_rw.extra[0] = (ALIGN_TYPE) (~((ALIGN_TYPE) 0));
    mod_rw.extra[1] = (ALIGN_TYPE) &mod_rw.out_ptr;
    test_expect("object_pointer_get_extended rejects UINT_MAX string length overflow",
                (ULONG) _txm_module_manager_kernel_dispatch(TXM_MODULE_OBJECT_POINTER_GET_EXTENDED_CALL, TXM_QUEUE_OBJECT, (ALIGN_TYPE) mod_rw.name_buf, (ALIGN_TYPE) mod_rw.extra),
                TXM_MODULE_INVALID_MEMORY);

    /* Valid lookups for thread, queue, timer, mutex, semaphore, event_flags.  */
    strcpy(mod_rw.name_buf, "app_q");
    mod_rw.extra[0] = 5U;
    mod_rw.extra[1] = (ALIGN_TYPE) &mod_rw.out_ptr;
    mod_rw.out_ptr = TX_NULL;
    test_expect("object_pointer_get_extended finds TXM_QUEUE_OBJECT",
                (ULONG) _txm_module_manager_kernel_dispatch(TXM_MODULE_OBJECT_POINTER_GET_EXTENDED_CALL, TXM_QUEUE_OBJECT, (ALIGN_TYPE) mod_rw.name_buf, (ALIGN_TYPE) mod_rw.extra),
                TX_SUCCESS);
    test_expect("found queue pointer matches app_queue", (ULONG) (mod_rw.out_ptr == (VOID *) &app_queue), 1UL);

    strcpy(mod_rw.name_buf, "app_t");
    test_expect("object_pointer_get_extended finds TXM_THREAD_OBJECT",
                (ULONG) _txm_module_manager_kernel_dispatch(TXM_MODULE_OBJECT_POINTER_GET_EXTENDED_CALL, TXM_THREAD_OBJECT, (ALIGN_TYPE) mod_rw.name_buf, (ALIGN_TYPE) mod_rw.extra),
                TX_SUCCESS);
    test_expect("found thread pointer matches app_thread", (ULONG) (mod_rw.out_ptr == (VOID *) &app_thread), 1UL);

    strcpy(mod_rw.name_buf, "sec_obj");
    mod_rw.extra[0] = 7U;
    test_expect("object_pointer_get_extended finds TXM_TIMER_OBJECT",
                (ULONG) _txm_module_manager_kernel_dispatch(TXM_MODULE_OBJECT_POINTER_GET_EXTENDED_CALL, TXM_TIMER_OBJECT, (ALIGN_TYPE) mod_rw.name_buf, (ALIGN_TYPE) mod_rw.extra),
                TX_SUCCESS);
    test_expect("object_pointer_get_extended finds TXM_MUTEX_OBJECT",
                (ULONG) _txm_module_manager_kernel_dispatch(TXM_MODULE_OBJECT_POINTER_GET_EXTENDED_CALL, TXM_MUTEX_OBJECT, (ALIGN_TYPE) mod_rw.name_buf, (ALIGN_TYPE) mod_rw.extra),
                TX_SUCCESS);
    test_expect("object_pointer_get_extended finds TXM_SEMAPHORE_OBJECT",
                (ULONG) _txm_module_manager_kernel_dispatch(TXM_MODULE_OBJECT_POINTER_GET_EXTENDED_CALL, TXM_SEMAPHORE_OBJECT, (ALIGN_TYPE) mod_rw.name_buf, (ALIGN_TYPE) mod_rw.extra),
                TX_SUCCESS);
    test_expect("object_pointer_get_extended finds TXM_EVENT_FLAGS_OBJECT",
                (ULONG) _txm_module_manager_kernel_dispatch(TXM_MODULE_OBJECT_POINTER_GET_EXTENDED_CALL, TXM_EVENT_FLAGS_OBJECT, (ALIGN_TYPE) mod_rw.name_buf, (ALIGN_TYPE) mod_rw.extra),
                TX_SUCCESS);

    /* Unprotected direct API calls for block/byte pool + error cases.  */
    cur_thread.tx_thread_module_instance_ptr = &priv_mod;
    test_expect("direct object_pointer_get_extended NULL args return TX_PTR_ERROR",
                _txm_module_manager_object_pointer_get_extended(TXM_QUEUE_OBJECT, TX_NULL, 5U, &found_ptr), TX_PTR_ERROR);
    test_expect("direct object_pointer_get_extended bad option returns TX_OPTION_ERROR",
                _txm_module_manager_object_pointer_get_extended(999U, "app_q", 5U, &found_ptr), TX_OPTION_ERROR);
    test_expect("unprotected module finds TXM_BLOCK_POOL_OBJECT",
                _txm_module_manager_object_pointer_get_extended(TXM_BLOCK_POOL_OBJECT, "app_bp", 6U, &found_ptr), TX_SUCCESS);
    test_expect("unprotected legacy object_pointer_get finds TXM_BYTE_POOL_OBJECT",
                _txm_module_manager_object_pointer_get(TXM_BYTE_POOL_OBJECT, "sec_obj", &found_ptr), TX_SUCCESS);
    test_expect("missing object returns TX_NO_INSTANCE",
                _txm_module_manager_object_pointer_get_extended(TXM_QUEUE_OBJECT, "missing", 7U, &found_ptr), TX_NO_INSTANCE);

    /* Cleanly delete created objects through dispatchers (triggering automatic deallocate).  */
    cur_thread.tx_thread_module_instance_ptr = &user_mod;
    test_expect("timer_delete succeeds and deallocates",
                (ULONG) _txm_module_manager_kernel_dispatch(TXM_TIMER_DELETE_CALL, (ALIGN_TYPE) obj_timer, 0, 0), TX_SUCCESS);
    test_expect("mutex_delete succeeds and deallocates",
                (ULONG) _txm_module_manager_kernel_dispatch(TXM_MUTEX_DELETE_CALL, (ALIGN_TYPE) obj_mutex, 0, 0), TX_SUCCESS);
    test_expect("semaphore_delete succeeds and deallocates",
                (ULONG) _txm_module_manager_kernel_dispatch(TXM_SEMAPHORE_DELETE_CALL, (ALIGN_TYPE) obj_sem, 0, 0), TX_SUCCESS);
    test_expect("event_flags_delete succeeds and deallocates",
                (ULONG) _txm_module_manager_kernel_dispatch(TXM_EVENT_FLAGS_DELETE_CALL, (ALIGN_TYPE) obj_ev, 0, 0), TX_SUCCESS);
    test_expect("byte_pool_delete succeeds and deallocates",
                (ULONG) _txm_module_manager_kernel_dispatch(TXM_BYTE_POOL_DELETE_CALL, (ALIGN_TYPE) obj_bpool, 0, 0), TX_SUCCESS);
    test_expect("all allocated module objects freed back to 0", user_mod.txm_module_instance_object_list_count, 0UL);

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
