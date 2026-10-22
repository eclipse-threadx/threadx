# Copyright (c) 2024 Microsoft Corporation
# Copyright (c) 2026 Eclipse ThreadX contributors
# SPDX-License-Identifier: MIT

target_sources(${PROJECT_NAME}
    PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}/src/txm_module_manager_absolute_load.c
    ${CMAKE_CURRENT_LIST_DIR}/src/txm_module_manager_absolute_load_extended.c
    ${CMAKE_CURRENT_LIST_DIR}/src/txm_module_manager_application_request.c
    ${CMAKE_CURRENT_LIST_DIR}/src/txm_module_manager_callback_request.c
    ${CMAKE_CURRENT_LIST_DIR}/src/txm_module_manager_event_flags_notify_trampoline.c
    ${CMAKE_CURRENT_LIST_DIR}/src/txm_module_manager_file_load.c
    ${CMAKE_CURRENT_LIST_DIR}/src/txm_module_manager_in_place_load.c
    ${CMAKE_CURRENT_LIST_DIR}/src/txm_module_manager_initialize.c
    ${CMAKE_CURRENT_LIST_DIR}/src/txm_module_manager_internal_load.c
    ${CMAKE_CURRENT_LIST_DIR}/src/txm_module_manager_kernel_dispatch.c
    ${CMAKE_CURRENT_LIST_DIR}/src/txm_module_manager_maximum_module_priority_set.c
    ${CMAKE_CURRENT_LIST_DIR}/src/txm_module_manager_memory_load.c
    ${CMAKE_CURRENT_LIST_DIR}/src/txm_module_manager_object_allocate.c
    ${CMAKE_CURRENT_LIST_DIR}/src/txm_module_manager_object_deallocate.c
    ${CMAKE_CURRENT_LIST_DIR}/src/txm_module_manager_object_pointer_get.c
    ${CMAKE_CURRENT_LIST_DIR}/src/txm_module_manager_object_pointer_get_extended.c
    ${CMAKE_CURRENT_LIST_DIR}/src/txm_module_manager_object_pool_create.c
    ${CMAKE_CURRENT_LIST_DIR}/src/txm_module_manager_properties_get.c
    ${CMAKE_CURRENT_LIST_DIR}/src/txm_module_manager_queue_notify_trampoline.c
    ${CMAKE_CURRENT_LIST_DIR}/src/txm_module_manager_semaphore_notify_trampoline.c
    ${CMAKE_CURRENT_LIST_DIR}/src/txm_module_manager_start.c
    ${CMAKE_CURRENT_LIST_DIR}/src/txm_module_manager_stop.c
    ${CMAKE_CURRENT_LIST_DIR}/src/txm_module_manager_thread_create.c
    ${CMAKE_CURRENT_LIST_DIR}/src/txm_module_manager_thread_notify_trampoline.c
    ${CMAKE_CURRENT_LIST_DIR}/src/txm_module_manager_thread_reset.c
    ${CMAKE_CURRENT_LIST_DIR}/src/txm_module_manager_timer_notify_trampoline.c
    ${CMAKE_CURRENT_LIST_DIR}/src/txm_module_manager_unload.c
    ${CMAKE_CURRENT_LIST_DIR}/src/txm_module_manager_util.c
)

target_include_directories(${PROJECT_NAME}
    SYSTEM
    PUBLIC
    ${CMAKE_CURRENT_LIST_DIR}/inc
    ${CMAKE_CURRENT_LIST_DIR}/../inc
)

# Define consumer check demonstrating compilation against the module-manager API
add_library(threadx-module-manager-consumer-check OBJECT EXCLUDE_FROM_ALL
    ${CMAKE_CURRENT_LIST_DIR}/consumer_check.c
)
target_link_libraries(threadx-module-manager-consumer-check PRIVATE azrtos::threadx)
