/***************************************************************************
 * Copyright (c) 2026 Eclipse ThreadX contributors
 *
 * This program and the accompanying materials are made available under the
 * terms of the MIT License which is available at
 * https://opensource.org/licenses/MIT.
 *
 * SPDX-License-Identifier: MIT
 **************************************************************************/

#include "tx_api.h"
#include "txm_module.h"

static TX_THREAD module_manager_thread;
static ULONG module_manager_stack[1024 / sizeof(ULONG)];
static UCHAR object_memory_pool[16384];
static UCHAR module_data_area[32768];
static TXM_MODULE_INSTANCE test_module;

static VOID test_fault_handler(TX_THREAD *thread, TXM_MODULE_INSTANCE *module)
{
    (VOID) thread;
    (VOID) module;
}

static VOID module_manager_entry(ULONG input)
{
    (VOID) input;
    txm_module_manager_initialize((VOID *) module_data_area, sizeof(module_data_area));
    txm_module_manager_object_pool_create(object_memory_pool, sizeof(object_memory_pool));
    txm_module_manager_memory_fault_notify(test_fault_handler);
    txm_module_manager_maximum_module_priority_set(&test_module, 5);
    txm_module_manager_start(&test_module);
    txm_module_manager_stop(&test_module);
    txm_module_manager_unload(&test_module);
}

VOID tx_application_define(VOID *first_unused_memory)
{
    (VOID) first_unused_memory;
    tx_thread_create(&module_manager_thread, "Module Manager",
                     module_manager_entry, 0,
                     module_manager_stack, sizeof(module_manager_stack),
                     1, 1, TX_NO_TIME_SLICE, TX_AUTO_START);
}

int main(void)
{
    tx_kernel_enter();
    return 0;
}
