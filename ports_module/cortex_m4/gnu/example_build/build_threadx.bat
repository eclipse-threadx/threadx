del tx.a
arm-none-eabi-gcc -c -g -mcpu=cortex-m4 -mfloat-abi=hard -mfpu=vfpv4 -mthumb tx_initialize_low_level.S
arm-none-eabi-gcc -c -g -mcpu=cortex-m4 -mfloat-abi=hard -mfpu=vfpv4 -mthumb ..\module_manager\src\tx_thread_stack_build.S
arm-none-eabi-gcc -c -g -mcpu=cortex-m4 -mfloat-abi=hard -mfpu=vfpv4 -mthumb ..\module_manager\src\tx_thread_schedule.S
arm-none-eabi-gcc -c -g -mcpu=cortex-m4 -mfloat-abi=hard -mfpu=vfpv4 -mthumb ..\module_manager\src\tx_thread_system_return.S
arm-none-eabi-gcc -c -g -mcpu=cortex-m4 -mfloat-abi=hard -mfpu=vfpv4 -mthumb ..\module_manager\src\tx_thread_context_save.S
arm-none-eabi-gcc -c -g -mcpu=cortex-m4 -mfloat-abi=hard -mfpu=vfpv4 -mthumb ..\module_manager\src\tx_thread_context_restore.S
arm-none-eabi-gcc -c -g -mcpu=cortex-m4 -mfloat-abi=hard -mfpu=vfpv4 -mthumb ..\module_manager\src\tx_thread_interrupt_control.S
arm-none-eabi-gcc -c -g -mcpu=cortex-m4 -mfloat-abi=hard -mfpu=vfpv4 -mthumb ..\module_manager\src\tx_timer_interrupt.S
arm-none-eabi-gcc -c -g -mcpu=cortex-m4 -mfloat-abi=hard -mfpu=vfpv4 -mthumb ..\module_manager\src\txm_module_manager_thread_stack_build.S

arm-none-eabi-gcc -c -g -mcpu=cortex-m4 -mfloat-abi=hard -mfpu=vfpv4 -mthumb -I..\inc -I..\..\..\..\common\inc ..\..\..\..\common\src\tx_block_allocate.c
arm-none-eabi-gcc -c -g -mcpu=cortex-m4 -mfloat-abi=hard -mfpu=vfpv4 -mthumb -I..\inc -I..\..\..\..\common\inc ..\..\..\..\common\src\tx_block_pool_cleanup.c
arm-none-eabi-gcc -c -g -mcpu=cortex-m4 -mfloat-abi=hard -mfpu=vfpv4 -mthumb -I..\inc -I..\..\..\..\common\inc ..\..\..\..\common\src\tx_block_pool_create.c
arm-none-eabi-gcc -c -g -mcpu=cortex-m4 -mfloat-abi=hard -mfpu=vfpv4 -mthumb -I..\inc -I..\..\..\..\common\inc ..\..\..\..\common\src\tx_block_pool_delete.c
arm-none-eabi-gcc -c -g -mcpu=cortex-m4 -mfloat-abi=hard -mfpu=vfpv4 -mthumb -I..\inc -I..\..\..\..\common\inc ..\..\..\..\common\src\tx_block_pool_info_get.c
arm-none-eabi-gcc -c -g -mcpu=cortex-m4 -mfloat-abi=hard -mfpu=vfpv4 -mthumb -I..\inc -I..\..\..\..\common\inc ..\..\..\..\common\src\tx_block_pool_initialize.c
arm-none-eabi-gcc -c -g -mcpu=cortex-m4 -mfloat-abi=hard -mfpu=vfpv4 -mthumb -I..\inc -I..\..\..\..\common\inc ..\..\..\..\common\src\tx_block_pool_performance_info_get.c
arm-none-eabi-gcc -c -g -mcpu=cortex-m4 -mfloat-abi=hard -mfpu=vfpv4 -mthumb -I..\inc -I..\..\..\..\common\inc ..\..\..\..\common\src\tx_block_pool_performance_system_info_get.c
arm-none-eabi-gcc -c -g -mcpu=cortex-m4 -mfloat-abi=hard -mfpu=vfpv4 -mthumb -I..\inc -I..\..\..\..\common\inc ..\..\..\..\common\src\tx_block_pool_prioritize.c
arm-none-eabi-gcc -c -g -mcpu=cortex-m4 -mfloat-abi=hard -mfpu=vfpv4 -mthumb -I..\inc -I..\..\..\..\common\inc ..\..\..\..\common\src\tx_block_release.c
arm-none-eabi-gcc -c -g -mcpu=cortex-m4 -mfloat-abi=hard -mfpu=vfpv4 -mthumb -I..\inc -I..\..\..\..\common\inc ..\..\..\..\common\src\tx_byte_allocate.c
arm-none-eabi-gcc -c -g -mcpu=cortex-m4 -mfloat-abi=hard -mfpu=vfpv4 -mthumb -I..\inc -I..\..\..\..\common\inc ..\..\..\..\common\src\tx_byte_pool_cleanup.c
arm-none-eabi-gcc -c -g -mcpu=cortex-m4 -mfloat-abi=hard -mfpu=vfpv4 -mthumb -I..\inc -I..\..\..\..\common\inc ..\..\..\..\common\src\tx_byte_pool_create.c
arm-none-eabi-gcc -c -g -mcpu=cortex-m4 -mfloat-abi=hard -mfpu=vfpv4 -mthumb -I..\inc -I..\..\..\..\common\inc ..\..\..\..\common\src\tx_byte_pool_delete.c
arm-none-eabi-gcc -c -g -mcpu=cortex-m4 -mfloat-abi=hard -mfpu=vfpv4 -mthumb -I..\inc -I..\..\..\..\common\inc ..\..\..\..\common\src\tx_byte_pool_info_get.c
arm-none-eabi-gcc -c -g -mcpu=cortex-m4 -mfloat-abi=hard -mfpu=vfpv4 -mthumb -I..\inc -I..\..\..\..\common\inc ..\..\..\..\common\src\tx_byte_pool_initialize.c
arm-none-eabi-gcc -c -g -mcpu=cortex-m4 -mfloat-abi=hard -mfpu=vfpv4 -mthumb -I..\inc -I..\..\..\..\common\inc ..\..\..\..\common\src\tx_byte_pool_performance_info_get.c
arm-none-eabi-gcc -c -g -mcpu=cortex-m4 -mfloat-abi=hard -mfpu=vfpv4 -mthumb -I..\inc -I..\..\..\..\common\inc ..\..\..\..\common\src\tx_byte_pool_performance_system_info_get.c
arm-none-eabi-gcc -c -g -mcpu=cortex-m4 -mfloat-abi=hard -mfpu=vfpv4 -mthumb -I..\inc -I..\..\..\..\common\inc ..\..\..\..\common\src\tx_byte_pool_prioritize.c
arm-none-eabi-gcc -c -g -mcpu=cortex-m4 -mfloat-abi=hard -mfpu=vfpv4 -mthumb -I..\inc -I..\..\..\..\common\inc ..\..\..\..\common\src\tx_byte_pool_search.c
arm-none-eabi-gcc -c -g -mcpu=cortex-m4 -mfloat-abi=hard -mfpu=vfpv4 -mthumb -I..\inc -I..\..\..\..\common\inc ..\..\..\..\common\src\tx_byte_release.c
arm-none-eabi-gcc -c -g -mcpu=cortex-m4 -mfloat-abi=hard -mfpu=vfpv4 -mthumb -I..\inc -I..\..\..\..\common\inc ..\..\..\..\common\src\tx_event_flags_cleanup.c
arm-none-eabi-gcc -c -g -mcpu=cortex-m4 -mfloat-abi=hard -mfpu=vfpv4 -mthumb -I..\inc -I..\..\..\..\common\inc ..\..\..\..\common\src\tx_event_flags_create.c
arm-none-eabi-gcc -c -g -mcpu=cortex-m4 -mfloat-abi=hard -mfpu=vfpv4 -mthumb -I..\inc -I..\..\..\..\common\inc ..\..\..\..\common\src\tx_event_flags_delete.c
arm-none-eabi-gcc -c -g -mcpu=cortex-m4 -mfloat-abi=hard -mfpu=vfpv4 -mthumb -I..\inc -I..\..\..\..\common\inc ..\..\..\..\common\src\tx_event_flags_get.c
arm-none-eabi-gcc -c -g -mcpu=cortex-m4 -mfloat-abi=hard -mfpu=vfpv4 -mthumb -I..\inc -I..\..\..\..\common\inc ..\..\..\..\common\src\tx_event_flags_info_get.c
arm-none-eabi-gcc -c -g -mcpu=cortex-m4 -mfloat-abi=hard -mfpu=vfpv4 -mthumb -I..\inc -I..\..\..\..\common\inc ..\..\..\..\common\src\tx_event_flags_initialize.c
arm-none-eabi-gcc -c -g -mcpu=cortex-m4 -mfloat-abi=hard -mfpu=vfpv4 -mthumb -I..\inc -I..\..\..\..\common\inc ..\..\..\..\common\src\tx_event_flags_performance_info_get.c
arm-none-eabi-gcc -c -g -mcpu=cortex-m4 -mfloat-abi=hard -mfpu=vfpv4 -mthumb -I..\inc -I..\..\..\..\common\inc ..\..\..\..\common\src\tx_event_flags_performance_system_info_get.c
arm-none-eabi-gcc -c -g -mcpu=cortex-m4 -mfloat-abi=hard -mfpu=vfpv4 -mthumb -I..\inc -I..\..\..\..\common\inc ..\..\..\..\common\src\tx_event_flags_set.c
arm-none-eabi-gcc -c -g -mcpu=cortex-m4 -mfloat-abi=hard -mfpu=vfpv4 -mthumb -I..\inc -I..\..\..\..\common\inc ..\..\..\..\common\src\tx_event_flags_set_notify.c
arm-none-eabi-gcc -c -g -mcpu=cortex-m4 -mfloat-abi=hard -mfpu=vfpv4 -mthumb -I..\inc -I..\..\..\..\common\inc ..\..\..\..\common\src\tx_initialize_high_level.c
arm-none-eabi-gcc -c -g -mcpu=cortex-m4 -mfloat-abi=hard -mfpu=vfpv4 -mthumb -I..\inc -I..\..\..\..\common\inc ..\..\..\..\common\src\tx_initialize_kernel_enter.c
arm-none-eabi-gcc -c -g -mcpu=cortex-m4 -mfloat-abi=hard -mfpu=vfpv4 -mthumb -I..\inc -I..\..\..\..\common\inc ..\..\..\..\common\src\tx_initialize_kernel_setup.c
arm-none-eabi-gcc -c -g -mcpu=cortex-m4 -mfloat-abi=hard -mfpu=vfpv4 -mthumb -I..\inc -I..\..\..\..\common\inc ..\..\..\..\common\src\tx_mutex_cleanup.c
arm-none-eabi-gcc -c -g -mcpu=cortex-m4 -mfloat-abi=hard -mfpu=vfpv4 -mthumb -I..\inc -I..\..\..\..\common\inc ..\..\..\..\common\src\tx_mutex_create.c
arm-none-eabi-gcc -c -g -mcpu=cortex-m4 -mfloat-abi=hard -mfpu=vfpv4 -mthumb -I..\inc -I..\..\..\..\common\inc ..\..\..\..\common\src\tx_mutex_delete.c
arm-none-eabi-gcc -c -g -mcpu=cortex-m4 -mfloat-abi=hard -mfpu=vfpv4 -mthumb -I..\inc -I..\..\..\..\common\inc ..\..\..\..\common\src\tx_mutex_get.c
arm-none-eabi-gcc -c -g -mcpu=cortex-m4 -mfloat-abi=hard -mfpu=vfpv4 -mthumb -I..\inc -I..\..\..\..\common\inc ..\..\..\..\common\src\tx_mutex_info_get.c
arm-none-eabi-gcc -c -g -mcpu=cortex-m4 -mfloat-abi=hard -mfpu=vfpv4 -mthumb -I..\inc -I..\..\..\..\common\inc ..\..\..\..\common\src\tx_mutex_initialize.c
arm-none-eabi-gcc -c -g -mcpu=cortex-m4 -mfloat-abi=hard -mfpu=vfpv4 -mthumb -I..\inc -I..\..\..\..\common\inc ..\..\..\..\common\src\tx_mutex_performance_info_get.c
arm-none-eabi-gcc -c -g -mcpu=cortex-m4 -mfloat-abi=hard -mfpu=vfpv4 -mthumb -I..\inc -I..\..\..\..\common\inc ..\..\..\..\common\src\tx_mutex_performance_system_info_get.c
arm-none-eabi-gcc -c -g -mcpu=cortex-m4 -mfloat-abi=hard -mfpu=vfpv4 -mthumb -I..\inc -I..\..\..\..\common\inc ..\..\..\..\common\src\tx_mutex_prioritize.c
arm-none-eabi-gcc -c -g -mcpu=cortex-m4 -mfloat-abi=hard -mfpu=vfpv4 -mthumb -I..\inc -I..\..\..\..\common\inc ..\..\..\..\common\src\tx_mutex_priority_change.c
arm-none-eabi-gcc -c -g -mcpu=cortex-m4 -mfloat-abi=hard -mfpu=vfpv4 -mthumb -I..\inc -I..\..\..\..\common\inc ..\..\..\..\common\src\tx_mutex_put.c
arm-none-eabi-gcc -c -g -mcpu=cortex-m4 -mfloat-abi=hard -mfpu=vfpv4 -mthumb -I..\inc -I..\..\..\..\common\inc ..\..\..\..\common\src\tx_queue_cleanup.c
arm-none-eabi-gcc -c -g -mcpu=cortex-m4 -mfloat-abi=hard -mfpu=vfpv4 -mthumb -I..\inc -I..\..\..\..\common\inc ..\..\..\..\common\src\tx_queue_create.c
arm-none-eabi-gcc -c -g -mcpu=cortex-m4 -mfloat-abi=hard -mfpu=vfpv4 -mthumb -I..\inc -I..\..\..\..\common\inc ..\..\..\..\common\src\tx_queue_delete.c
arm-none-eabi-gcc -c -g -mcpu=cortex-m4 -mfloat-abi=hard -mfpu=vfpv4 -mthumb -I..\inc -I..\..\..\..\common\inc ..\..\..\..\common\src\tx_queue_flush.c
arm-none-eabi-gcc -c -g -mcpu=cortex-m4 -mfloat-abi=hard -mfpu=vfpv4 -mthumb -I..\inc -I..\..\..\..\common\inc ..\..\..\..\common\src\tx_queue_front_send.c
arm-none-eabi-gcc -c -g -mcpu=cortex-m4 -mfloat-abi=hard -mfpu=vfpv4 -mthumb -I..\inc -I..\..\..\..\common\inc ..\..\..\..\common\src\tx_queue_info_get.c
arm-none-eabi-gcc -c -g -mcpu=cortex-m4 -mfloat-abi=hard -mfpu=vfpv4 -mthumb -I..\inc -I..\..\..\..\common\inc ..\..\..\..\common\src\tx_queue_initialize.c
arm-none-eabi-gcc -c -g -mcpu=cortex-m4 -mfloat-abi=hard -mfpu=vfpv4 -mthumb -I..\inc -I..\..\..\..\common\inc ..\..\..\..\common\src\tx_queue_performance_info_get.c
arm-none-eabi-gcc -c -g -mcpu=cortex-m4 -mfloat-abi=hard -mfpu=vfpv4 -mthumb -I..\inc -I..\..\..\..\common\inc ..\..\..\..\common\src\tx_queue_performance_system_info_get.c
arm-none-eabi-gcc -c -g -mcpu=cortex-m4 -mfloat-abi=hard -mfpu=vfpv4 -mthumb -I..\inc -I..\..\..\..\common\inc ..\..\..\..\common\src\tx_queue_prioritize.c
arm-none-eabi-gcc -c -g -mcpu=cortex-m4 -mfloat-abi=hard -mfpu=vfpv4 -mthumb -I..\inc -I..\..\..\..\common\inc ..\..\..\..\common\src\tx_queue_receive.c
arm-none-eabi-gcc -c -g -mcpu=cortex-m4 -mfloat-abi=hard -mfpu=vfpv4 -mthumb -I..\inc -I..\..\..\..\common\inc ..\..\..\..\common\src\tx_queue_send.c
arm-none-eabi-gcc -c -g -mcpu=cortex-m4 -mfloat-abi=hard -mfpu=vfpv4 -mthumb -I..\inc -I..\..\..\..\common\inc ..\..\..\..\common\src\tx_queue_send_notify.c
arm-none-eabi-gcc -c -g -mcpu=cortex-m4 -mfloat-abi=hard -mfpu=vfpv4 -mthumb -I..\inc -I..\..\..\..\common\inc ..\..\..\..\common\src\tx_semaphore_ceiling_put.c
arm-none-eabi-gcc -c -g -mcpu=cortex-m4 -mfloat-abi=hard -mfpu=vfpv4 -mthumb -I..\inc -I..\..\..\..\common\inc ..\..\..\..\common\src\tx_semaphore_cleanup.c
arm-none-eabi-gcc -c -g -mcpu=cortex-m4 -mfloat-abi=hard -mfpu=vfpv4 -mthumb -I..\inc -I..\..\..\..\common\inc ..\..\..\..\common\src\tx_semaphore_create.c
arm-none-eabi-gcc -c -g -mcpu=cortex-m4 -mfloat-abi=hard -mfpu=vfpv4 -mthumb -I..\inc -I..\..\..\..\common\inc ..\..\..\..\common\src\tx_semaphore_delete.c
arm-none-eabi-gcc -c -g -mcpu=cortex-m4 -mfloat-abi=hard -mfpu=vfpv4 -mthumb -I..\inc -I..\..\..\..\common\inc ..\..\..\..\common\src\tx_semaphore_get.c
arm-none-eabi-gcc -c -g -mcpu=cortex-m4 -mfloat-abi=hard -mfpu=vfpv4 -mthumb -I..\inc -I..\..\..\..\common\inc ..\..\..\..\common\src\tx_semaphore_info_get.c
arm-none-eabi-gcc -c -g -mcpu=cortex-m4 -mfloat-abi=hard -mfpu=vfpv4 -mthumb -I..\inc -I..\..\..\..\common\inc ..\..\..\..\common\src\tx_semaphore_initialize.c
arm-none-eabi-gcc -c -g -mcpu=cortex-m4 -mfloat-abi=hard -mfpu=vfpv4 -mthumb -I..\inc -I..\..\..\..\common\inc ..\..\..\..\common\src\tx_semaphore_performance_info_get.c
arm-none-eabi-gcc -c -g -mcpu=cortex-m4 -mfloat-abi=hard -mfpu=vfpv4 -mthumb -I..\inc -I..\..\..\..\common\inc ..\..\..\..\common\src\tx_semaphore_performance_system_info_get.c
arm-none-eabi-gcc -c -g -mcpu=cortex-m4 -mfloat-abi=hard -mfpu=vfpv4 -mthumb -I..\inc -I..\..\..\..\common\inc ..\..\..\..\common\src\tx_semaphore_prioritize.c
arm-none-eabi-gcc -c -g -mcpu=cortex-m4 -mfloat-abi=hard -mfpu=vfpv4 -mthumb -I..\inc -I..\..\..\..\common\inc ..\..\..\..\common\src\tx_semaphore_put.c
arm-none-eabi-gcc -c -g -mcpu=cortex-m4 -mfloat-abi=hard -mfpu=vfpv4 -mthumb -I..\inc -I..\..\..\..\common\inc ..\..\..\..\common\src\tx_semaphore_put_notify.c
arm-none-eabi-gcc -c -g -mcpu=cortex-m4 -mfloat-abi=hard -mfpu=vfpv4 -mthumb -I..\inc -I..\..\..\..\common\inc ..\..\..\..\common\src\tx_thread_create.c
arm-none-eabi-gcc -c -g -mcpu=cortex-m4 -mfloat-abi=hard -mfpu=vfpv4 -mthumb -I..\inc -I..\..\..\..\common\inc ..\..\..\..\common\src\tx_thread_delete.c
arm-none-eabi-gcc -c -g -mcpu=cortex-m4 -mfloat-abi=hard -mfpu=vfpv4 -mthumb -I..\inc -I..\..\..\..\common\inc ..\..\..\..\common\src\tx_thread_entry_exit_notify.c
arm-none-eabi-gcc -c -g -mcpu=cortex-m4 -mfloat-abi=hard -mfpu=vfpv4 -mthumb -I..\inc -I..\..\..\..\common\inc ..\..\..\..\common\src\tx_thread_identify.c
arm-none-eabi-gcc -c -g -mcpu=cortex-m4 -mfloat-abi=hard -mfpu=vfpv4 -mthumb -I..\inc -I..\..\..\..\common\inc ..\..\..\..\common\src\tx_thread_info_get.c
arm-none-eabi-gcc -c -g -mcpu=cortex-m4 -mfloat-abi=hard -mfpu=vfpv4 -mthumb -I..\inc -I..\..\..\..\common\inc ..\..\..\..\common\src\tx_thread_initialize.c
arm-none-eabi-gcc -c -g -mcpu=cortex-m4 -mfloat-abi=hard -mfpu=vfpv4 -mthumb -I..\inc -I..\..\..\..\common\inc ..\..\..\..\common\src\tx_thread_performance_info_get.c
arm-none-eabi-gcc -c -g -mcpu=cortex-m4 -mfloat-abi=hard -mfpu=vfpv4 -mthumb -I..\inc -I..\..\..\..\common\inc ..\..\..\..\common\src\tx_thread_performance_system_info_get.c
arm-none-eabi-gcc -c -g -mcpu=cortex-m4 -mfloat-abi=hard -mfpu=vfpv4 -mthumb -I..\inc -I..\..\..\..\common\inc ..\..\..\..\common\src\tx_thread_preemption_change.c
arm-none-eabi-gcc -c -g -mcpu=cortex-m4 -mfloat-abi=hard -mfpu=vfpv4 -mthumb -I..\inc -I..\..\..\..\common\inc ..\..\..\..\common\src\tx_thread_priority_change.c
arm-none-eabi-gcc -c -g -mcpu=cortex-m4 -mfloat-abi=hard -mfpu=vfpv4 -mthumb -I..\inc -I..\..\..\..\common\inc ..\..\..\..\common\src\tx_thread_relinquish.c
arm-none-eabi-gcc -c -g -mcpu=cortex-m4 -mfloat-abi=hard -mfpu=vfpv4 -mthumb -I..\inc -I..\..\..\..\common\inc ..\..\..\..\common\src\tx_thread_reset.c
arm-none-eabi-gcc -c -g -mcpu=cortex-m4 -mfloat-abi=hard -mfpu=vfpv4 -mthumb -I..\inc -I..\..\..\..\common\inc ..\..\..\..\common\src\tx_thread_resume.c
arm-none-eabi-gcc -c -g -mcpu=cortex-m4 -mfloat-abi=hard -mfpu=vfpv4 -mthumb -I..\inc -I..\..\..\..\common\inc ..\..\..\..\common\src\tx_thread_shell_entry.c
arm-none-eabi-gcc -c -g -mcpu=cortex-m4 -mfloat-abi=hard -mfpu=vfpv4 -mthumb -I..\inc -I..\..\..\..\common\inc ..\..\..\..\common\src\tx_thread_sleep.c
arm-none-eabi-gcc -c -g -mcpu=cortex-m4 -mfloat-abi=hard -mfpu=vfpv4 -mthumb -I..\inc -I..\..\..\..\common\inc ..\..\..\..\common\src\tx_thread_stack_analyze.c
arm-none-eabi-gcc -c -g -mcpu=cortex-m4 -mfloat-abi=hard -mfpu=vfpv4 -mthumb -I..\inc -I..\..\..\..\common\inc ..\..\..\..\common\src\tx_thread_stack_error_handler.c
arm-none-eabi-gcc -c -g -mcpu=cortex-m4 -mfloat-abi=hard -mfpu=vfpv4 -mthumb -I..\inc -I..\..\..\..\common\inc ..\..\..\..\common\src\tx_thread_stack_error_notify.c
arm-none-eabi-gcc -c -g -mcpu=cortex-m4 -mfloat-abi=hard -mfpu=vfpv4 -mthumb -I..\inc -I..\..\..\..\common\inc ..\..\..\..\common\src\tx_thread_suspend.c
arm-none-eabi-gcc -c -g -mcpu=cortex-m4 -mfloat-abi=hard -mfpu=vfpv4 -mthumb -I..\inc -I..\..\..\..\common\inc ..\..\..\..\common\src\tx_thread_system_preempt_check.c
arm-none-eabi-gcc -c -g -mcpu=cortex-m4 -mfloat-abi=hard -mfpu=vfpv4 -mthumb -I..\inc -I..\..\..\..\common\inc ..\..\..\..\common\src\tx_thread_system_resume.c
arm-none-eabi-gcc -c -g -mcpu=cortex-m4 -mfloat-abi=hard -mfpu=vfpv4 -mthumb -I..\inc -I..\..\..\..\common\inc ..\..\..\..\common\src\tx_thread_system_suspend.c
arm-none-eabi-gcc -c -g -mcpu=cortex-m4 -mfloat-abi=hard -mfpu=vfpv4 -mthumb -I..\inc -I..\..\..\..\common\inc ..\..\..\..\common\src\tx_thread_terminate.c
arm-none-eabi-gcc -c -g -mcpu=cortex-m4 -mfloat-abi=hard -mfpu=vfpv4 -mthumb -I..\inc -I..\..\..\..\common\inc ..\..\..\..\common\src\tx_thread_time_slice.c
arm-none-eabi-gcc -c -g -mcpu=cortex-m4 -mfloat-abi=hard -mfpu=vfpv4 -mthumb -I..\inc -I..\..\..\..\common\inc ..\..\..\..\common\src\tx_thread_time_slice_change.c
arm-none-eabi-gcc -c -g -mcpu=cortex-m4 -mfloat-abi=hard -mfpu=vfpv4 -mthumb -I..\inc -I..\..\..\..\common\inc ..\..\..\..\common\src\tx_thread_timeout.c
arm-none-eabi-gcc -c -g -mcpu=cortex-m4 -mfloat-abi=hard -mfpu=vfpv4 -mthumb -I..\inc -I..\..\..\..\common\inc ..\..\..\..\common\src\tx_thread_wait_abort.c
arm-none-eabi-gcc -c -g -mcpu=cortex-m4 -mfloat-abi=hard -mfpu=vfpv4 -mthumb -I..\inc -I..\..\..\..\common\inc ..\..\..\..\common\src\tx_time_get.c
arm-none-eabi-gcc -c -g -mcpu=cortex-m4 -mfloat-abi=hard -mfpu=vfpv4 -mthumb -I..\inc -I..\..\..\..\common\inc ..\..\..\..\common\src\tx_time_set.c
arm-none-eabi-gcc -c -g -mcpu=cortex-m4 -mfloat-abi=hard -mfpu=vfpv4 -mthumb -I..\inc -I..\..\..\..\common\inc ..\..\..\..\common\src\tx_timer_activate.c
arm-none-eabi-gcc -c -g -mcpu=cortex-m4 -mfloat-abi=hard -mfpu=vfpv4 -mthumb -I..\inc -I..\..\..\..\common\inc ..\..\..\..\common\src\tx_timer_change.c
arm-none-eabi-gcc -c -g -mcpu=cortex-m4 -mfloat-abi=hard -mfpu=vfpv4 -mthumb -I..\inc -I..\..\..\..\common\inc ..\..\..\..\common\src\tx_timer_create.c
arm-none-eabi-gcc -c -g -mcpu=cortex-m4 -mfloat-abi=hard -mfpu=vfpv4 -mthumb -I..\inc -I..\..\..\..\common\inc ..\..\..\..\common\src\tx_timer_deactivate.c
arm-none-eabi-gcc -c -g -mcpu=cortex-m4 -mfloat-abi=hard -mfpu=vfpv4 -mthumb -I..\inc -I..\..\..\..\common\inc ..\..\..\..\common\src\tx_timer_delete.c
arm-none-eabi-gcc -c -g -mcpu=cortex-m4 -mfloat-abi=hard -mfpu=vfpv4 -mthumb -I..\inc -I..\..\..\..\common\inc ..\..\..\..\common\src\tx_timer_expiration_process.c
arm-none-eabi-gcc -c -g -mcpu=cortex-m4 -mfloat-abi=hard -mfpu=vfpv4 -mthumb -I..\inc -I..\..\..\..\common\inc ..\..\..\..\common\src\tx_timer_info_get.c
arm-none-eabi-gcc -c -g -mcpu=cortex-m4 -mfloat-abi=hard -mfpu=vfpv4 -mthumb -I..\inc -I..\..\..\..\common\inc ..\..\..\..\common\src\tx_timer_initialize.c
arm-none-eabi-gcc -c -g -mcpu=cortex-m4 -mfloat-abi=hard -mfpu=vfpv4 -mthumb -I..\inc -I..\..\..\..\common\inc ..\..\..\..\common\src\tx_timer_performance_info_get.c
arm-none-eabi-gcc -c -g -mcpu=cortex-m4 -mfloat-abi=hard -mfpu=vfpv4 -mthumb -I..\inc -I..\..\..\..\common\inc ..\..\..\..\common\src\tx_timer_performance_system_info_get.c
arm-none-eabi-gcc -c -g -mcpu=cortex-m4 -mfloat-abi=hard -mfpu=vfpv4 -mthumb -I..\inc -I..\..\..\..\common\inc ..\..\..\..\common\src\tx_timer_system_activate.c
arm-none-eabi-gcc -c -g -mcpu=cortex-m4 -mfloat-abi=hard -mfpu=vfpv4 -mthumb -I..\inc -I..\..\..\..\common\inc ..\..\..\..\common\src\tx_timer_system_deactivate.c
arm-none-eabi-gcc -c -g -mcpu=cortex-m4 -mfloat-abi=hard -mfpu=vfpv4 -mthumb -I..\inc -I..\..\..\..\common\inc ..\..\..\..\common\src\tx_timer_thread_entry.c
arm-none-eabi-gcc -c -g -mcpu=cortex-m4 -mfloat-abi=hard -mfpu=vfpv4 -mthumb -I..\inc -I..\..\..\..\common\inc ..\..\..\..\common\src\tx_trace_enable.c
arm-none-eabi-gcc -c -g -mcpu=cortex-m4 -mfloat-abi=hard -mfpu=vfpv4 -mthumb -I..\inc -I..\..\..\..\common\inc ..\..\..\..\common\src\tx_trace_disable.c
arm-none-eabi-gcc -c -g -mcpu=cortex-m4 -mfloat-abi=hard -mfpu=vfpv4 -mthumb -I..\inc -I..\..\..\..\common\inc ..\..\..\..\common\src\tx_trace_initialize.c
arm-none-eabi-gcc -c -g -mcpu=cortex-m4 -mfloat-abi=hard -mfpu=vfpv4 -mthumb -I..\inc -I..\..\..\..\common\inc ..\..\..\..\common\src\tx_trace_interrupt_control.c
arm-none-eabi-gcc -c -g -mcpu=cortex-m4 -mfloat-abi=hard -mfpu=vfpv4 -mthumb -I..\inc -I..\..\..\..\common\inc ..\..\..\..\common\src\tx_trace_isr_enter_insert.c
arm-none-eabi-gcc -c -g -mcpu=cortex-m4 -mfloat-abi=hard -mfpu=vfpv4 -mthumb -I..\inc -I..\..\..\..\common\inc ..\..\..\..\common\src\tx_trace_isr_exit_insert.c
arm-none-eabi-gcc -c -g -mcpu=cortex-m4 -mfloat-abi=hard -mfpu=vfpv4 -mthumb -I..\inc -I..\..\..\..\common\inc ..\..\..\..\common\src\tx_trace_object_register.c
arm-none-eabi-gcc -c -g -mcpu=cortex-m4 -mfloat-abi=hard -mfpu=vfpv4 -mthumb -I..\inc -I..\..\..\..\common\inc ..\..\..\..\common\src\tx_trace_object_unregister.c
arm-none-eabi-gcc -c -g -mcpu=cortex-m4 -mfloat-abi=hard -mfpu=vfpv4 -mthumb -I..\inc -I..\..\..\..\common\inc ..\..\..\..\common\src\tx_trace_user_event_insert.c
arm-none-eabi-gcc -c -g -mcpu=cortex-m4 -mfloat-abi=hard -mfpu=vfpv4 -mthumb -I..\inc -I..\..\..\..\common\inc ..\..\..\..\common\src\tx_trace_buffer_full_notify.c
arm-none-eabi-gcc -c -g -mcpu=cortex-m4 -mfloat-abi=hard -mfpu=vfpv4 -mthumb -I..\inc -I..\..\..\..\common\inc ..\..\..\..\common\src\tx_trace_event_filter.c
arm-none-eabi-gcc -c -g -mcpu=cortex-m4 -mfloat-abi=hard -mfpu=vfpv4 -mthumb -I..\inc -I..\..\..\..\common\inc ..\..\..\..\common\src\tx_trace_event_unfilter.c
arm-none-eabi-gcc -c -g -mcpu=cortex-m4 -mfloat-abi=hard -mfpu=vfpv4 -mthumb -I..\inc -I..\..\..\..\common\inc ..\..\..\..\common\src\txe_block_allocate.c
arm-none-eabi-gcc -c -g -mcpu=cortex-m4 -mfloat-abi=hard -mfpu=vfpv4 -mthumb -I..\inc -I..\..\..\..\common\inc ..\..\..\..\common\src\txe_block_pool_create.c
arm-none-eabi-gcc -c -g -mcpu=cortex-m4 -mfloat-abi=hard -mfpu=vfpv4 -mthumb -I..\inc -I..\..\..\..\common\inc ..\..\..\..\common\src\txe_block_pool_delete.c
arm-none-eabi-gcc -c -g -mcpu=cortex-m4 -mfloat-abi=hard -mfpu=vfpv4 -mthumb -I..\inc -I..\..\..\..\common\inc ..\..\..\..\common\src\txe_block_pool_info_get.c
arm-none-eabi-gcc -c -g -mcpu=cortex-m4 -mfloat-abi=hard -mfpu=vfpv4 -mthumb -I..\inc -I..\..\..\..\common\inc ..\..\..\..\common\src\txe_block_pool_prioritize.c
arm-none-eabi-gcc -c -g -mcpu=cortex-m4 -mfloat-abi=hard -mfpu=vfpv4 -mthumb -I..\inc -I..\..\..\..\common\inc ..\..\..\..\common\src\txe_block_release.c
arm-none-eabi-gcc -c -g -mcpu=cortex-m4 -mfloat-abi=hard -mfpu=vfpv4 -mthumb -I..\inc -I..\..\..\..\common\inc ..\..\..\..\common\src\txe_byte_allocate.c
arm-none-eabi-gcc -c -g -mcpu=cortex-m4 -mfloat-abi=hard -mfpu=vfpv4 -mthumb -I..\inc -I..\..\..\..\common\inc ..\..\..\..\common\src\txe_byte_pool_create.c
arm-none-eabi-gcc -c -g -mcpu=cortex-m4 -mfloat-abi=hard -mfpu=vfpv4 -mthumb -I..\inc -I..\..\..\..\common\inc ..\..\..\..\common\src\txe_byte_pool_delete.c
arm-none-eabi-gcc -c -g -mcpu=cortex-m4 -mfloat-abi=hard -mfpu=vfpv4 -mthumb -I..\inc -I..\..\..\..\common\inc ..\..\..\..\common\src\txe_byte_pool_info_get.c
arm-none-eabi-gcc -c -g -mcpu=cortex-m4 -mfloat-abi=hard -mfpu=vfpv4 -mthumb -I..\inc -I..\..\..\..\common\inc ..\..\..\..\common\src\txe_byte_pool_prioritize.c
arm-none-eabi-gcc -c -g -mcpu=cortex-m4 -mfloat-abi=hard -mfpu=vfpv4 -mthumb -I..\inc -I..\..\..\..\common\inc ..\..\..\..\common\src\txe_byte_release.c
arm-none-eabi-gcc -c -g -mcpu=cortex-m4 -mfloat-abi=hard -mfpu=vfpv4 -mthumb -I..\inc -I..\..\..\..\common\inc ..\..\..\..\common\src\txe_event_flags_create.c
arm-none-eabi-gcc -c -g -mcpu=cortex-m4 -mfloat-abi=hard -mfpu=vfpv4 -mthumb -I..\inc -I..\..\..\..\common\inc ..\..\..\..\common\src\txe_event_flags_delete.c
arm-none-eabi-gcc -c -g -mcpu=cortex-m4 -mfloat-abi=hard -mfpu=vfpv4 -mthumb -I..\inc -I..\..\..\..\common\inc ..\..\..\..\common\src\txe_event_flags_get.c
arm-none-eabi-gcc -c -g -mcpu=cortex-m4 -mfloat-abi=hard -mfpu=vfpv4 -mthumb -I..\inc -I..\..\..\..\common\inc ..\..\..\..\common\src\txe_event_flags_info_get.c
arm-none-eabi-gcc -c -g -mcpu=cortex-m4 -mfloat-abi=hard -mfpu=vfpv4 -mthumb -I..\inc -I..\..\..\..\common\inc ..\..\..\..\common\src\txe_event_flags_set.c
arm-none-eabi-gcc -c -g -mcpu=cortex-m4 -mfloat-abi=hard -mfpu=vfpv4 -mthumb -I..\inc -I..\..\..\..\common\inc ..\..\..\..\common\src\txe_event_flags_set_notify.c
arm-none-eabi-gcc -c -g -mcpu=cortex-m4 -mfloat-abi=hard -mfpu=vfpv4 -mthumb -I..\inc -I..\..\..\..\common\inc ..\..\..\..\common\src\txe_mutex_create.c
arm-none-eabi-gcc -c -g -mcpu=cortex-m4 -mfloat-abi=hard -mfpu=vfpv4 -mthumb -I..\inc -I..\..\..\..\common\inc ..\..\..\..\common\src\txe_mutex_delete.c
arm-none-eabi-gcc -c -g -mcpu=cortex-m4 -mfloat-abi=hard -mfpu=vfpv4 -mthumb -I..\inc -I..\..\..\..\common\inc ..\..\..\..\common\src\txe_mutex_get.c
arm-none-eabi-gcc -c -g -mcpu=cortex-m4 -mfloat-abi=hard -mfpu=vfpv4 -mthumb -I..\inc -I..\..\..\..\common\inc ..\..\..\..\common\src\txe_mutex_info_get.c
arm-none-eabi-gcc -c -g -mcpu=cortex-m4 -mfloat-abi=hard -mfpu=vfpv4 -mthumb -I..\inc -I..\..\..\..\common\inc ..\..\..\..\common\src\txe_mutex_prioritize.c
arm-none-eabi-gcc -c -g -mcpu=cortex-m4 -mfloat-abi=hard -mfpu=vfpv4 -mthumb -I..\inc -I..\..\..\..\common\inc ..\..\..\..\common\src\txe_mutex_put.c
arm-none-eabi-gcc -c -g -mcpu=cortex-m4 -mfloat-abi=hard -mfpu=vfpv4 -mthumb -I..\inc -I..\..\..\..\common\inc ..\..\..\..\common\src\txe_queue_create.c
arm-none-eabi-gcc -c -g -mcpu=cortex-m4 -mfloat-abi=hard -mfpu=vfpv4 -mthumb -I..\inc -I..\..\..\..\common\inc ..\..\..\..\common\src\txe_queue_delete.c
arm-none-eabi-gcc -c -g -mcpu=cortex-m4 -mfloat-abi=hard -mfpu=vfpv4 -mthumb -I..\inc -I..\..\..\..\common\inc ..\..\..\..\common\src\txe_queue_flush.c
arm-none-eabi-gcc -c -g -mcpu=cortex-m4 -mfloat-abi=hard -mfpu=vfpv4 -mthumb -I..\inc -I..\..\..\..\common\inc ..\..\..\..\common\src\txe_queue_front_send.c
arm-none-eabi-gcc -c -g -mcpu=cortex-m4 -mfloat-abi=hard -mfpu=vfpv4 -mthumb -I..\inc -I..\..\..\..\common\inc ..\..\..\..\common\src\txe_queue_info_get.c
arm-none-eabi-gcc -c -g -mcpu=cortex-m4 -mfloat-abi=hard -mfpu=vfpv4 -mthumb -I..\inc -I..\..\..\..\common\inc ..\..\..\..\common\src\txe_queue_prioritize.c
arm-none-eabi-gcc -c -g -mcpu=cortex-m4 -mfloat-abi=hard -mfpu=vfpv4 -mthumb -I..\inc -I..\..\..\..\common\inc ..\..\..\..\common\src\txe_queue_receive.c
arm-none-eabi-gcc -c -g -mcpu=cortex-m4 -mfloat-abi=hard -mfpu=vfpv4 -mthumb -I..\inc -I..\..\..\..\common\inc ..\..\..\..\common\src\txe_queue_send.c
arm-none-eabi-gcc -c -g -mcpu=cortex-m4 -mfloat-abi=hard -mfpu=vfpv4 -mthumb -I..\inc -I..\..\..\..\common\inc ..\..\..\..\common\src\txe_queue_send_notify.c
arm-none-eabi-gcc -c -g -mcpu=cortex-m4 -mfloat-abi=hard -mfpu=vfpv4 -mthumb -I..\inc -I..\..\..\..\common\inc ..\..\..\..\common\src\txe_semaphore_ceiling_put.c
arm-none-eabi-gcc -c -g -mcpu=cortex-m4 -mfloat-abi=hard -mfpu=vfpv4 -mthumb -I..\inc -I..\..\..\..\common\inc ..\..\..\..\common\src\txe_semaphore_create.c
arm-none-eabi-gcc -c -g -mcpu=cortex-m4 -mfloat-abi=hard -mfpu=vfpv4 -mthumb -I..\inc -I..\..\..\..\common\inc ..\..\..\..\common\src\txe_semaphore_delete.c
arm-none-eabi-gcc -c -g -mcpu=cortex-m4 -mfloat-abi=hard -mfpu=vfpv4 -mthumb -I..\inc -I..\..\..\..\common\inc ..\..\..\..\common\src\txe_semaphore_get.c
arm-none-eabi-gcc -c -g -mcpu=cortex-m4 -mfloat-abi=hard -mfpu=vfpv4 -mthumb -I..\inc -I..\..\..\..\common\inc ..\..\..\..\common\src\txe_semaphore_info_get.c
arm-none-eabi-gcc -c -g -mcpu=cortex-m4 -mfloat-abi=hard -mfpu=vfpv4 -mthumb -I..\inc -I..\..\..\..\common\inc ..\..\..\..\common\src\txe_semaphore_prioritize.c
arm-none-eabi-gcc -c -g -mcpu=cortex-m4 -mfloat-abi=hard -mfpu=vfpv4 -mthumb -I..\inc -I..\..\..\..\common\inc ..\..\..\..\common\src\txe_semaphore_put.c
arm-none-eabi-gcc -c -g -mcpu=cortex-m4 -mfloat-abi=hard -mfpu=vfpv4 -mthumb -I..\inc -I..\..\..\..\common\inc ..\..\..\..\common\src\txe_semaphore_put_notify.c
arm-none-eabi-gcc -c -g -mcpu=cortex-m4 -mfloat-abi=hard -mfpu=vfpv4 -mthumb -I..\inc -I..\..\..\..\common\inc ..\..\..\..\common\src\txe_thread_create.c
arm-none-eabi-gcc -c -g -mcpu=cortex-m4 -mfloat-abi=hard -mfpu=vfpv4 -mthumb -I..\inc -I..\..\..\..\common\inc ..\..\..\..\common\src\txe_thread_delete.c
arm-none-eabi-gcc -c -g -mcpu=cortex-m4 -mfloat-abi=hard -mfpu=vfpv4 -mthumb -I..\inc -I..\..\..\..\common\inc ..\..\..\..\common\src\txe_thread_entry_exit_notify.c
arm-none-eabi-gcc -c -g -mcpu=cortex-m4 -mfloat-abi=hard -mfpu=vfpv4 -mthumb -I..\inc -I..\..\..\..\common\inc ..\..\..\..\common\src\txe_thread_info_get.c
arm-none-eabi-gcc -c -g -mcpu=cortex-m4 -mfloat-abi=hard -mfpu=vfpv4 -mthumb -I..\inc -I..\..\..\..\common\inc ..\..\..\..\common\src\txe_thread_preemption_change.c
arm-none-eabi-gcc -c -g -mcpu=cortex-m4 -mfloat-abi=hard -mfpu=vfpv4 -mthumb -I..\inc -I..\..\..\..\common\inc ..\..\..\..\common\src\txe_thread_priority_change.c
arm-none-eabi-gcc -c -g -mcpu=cortex-m4 -mfloat-abi=hard -mfpu=vfpv4 -mthumb -I..\inc -I..\..\..\..\common\inc ..\..\..\..\common\src\txe_thread_relinquish.c
arm-none-eabi-gcc -c -g -mcpu=cortex-m4 -mfloat-abi=hard -mfpu=vfpv4 -mthumb -I..\inc -I..\..\..\..\common\inc ..\..\..\..\common\src\txe_thread_reset.c
arm-none-eabi-gcc -c -g -mcpu=cortex-m4 -mfloat-abi=hard -mfpu=vfpv4 -mthumb -I..\inc -I..\..\..\..\common\inc ..\..\..\..\common\src\txe_thread_resume.c
arm-none-eabi-gcc -c -g -mcpu=cortex-m4 -mfloat-abi=hard -mfpu=vfpv4 -mthumb -I..\inc -I..\..\..\..\common\inc ..\..\..\..\common\src\txe_thread_suspend.c
arm-none-eabi-gcc -c -g -mcpu=cortex-m4 -mfloat-abi=hard -mfpu=vfpv4 -mthumb -I..\inc -I..\..\..\..\common\inc ..\..\..\..\common\src\txe_thread_terminate.c
arm-none-eabi-gcc -c -g -mcpu=cortex-m4 -mfloat-abi=hard -mfpu=vfpv4 -mthumb -I..\inc -I..\..\..\..\common\inc ..\..\..\..\common\src\txe_thread_time_slice_change.c
arm-none-eabi-gcc -c -g -mcpu=cortex-m4 -mfloat-abi=hard -mfpu=vfpv4 -mthumb -I..\inc -I..\..\..\..\common\inc ..\..\..\..\common\src\txe_thread_wait_abort.c
arm-none-eabi-gcc -c -g -mcpu=cortex-m4 -mfloat-abi=hard -mfpu=vfpv4 -mthumb -I..\inc -I..\..\..\..\common\inc ..\..\..\..\common\src\txe_timer_activate.c
arm-none-eabi-gcc -c -g -mcpu=cortex-m4 -mfloat-abi=hard -mfpu=vfpv4 -mthumb -I..\inc -I..\..\..\..\common\inc ..\..\..\..\common\src\txe_timer_change.c
arm-none-eabi-gcc -c -g -mcpu=cortex-m4 -mfloat-abi=hard -mfpu=vfpv4 -mthumb -I..\inc -I..\..\..\..\common\inc ..\..\..\..\common\src\txe_timer_create.c
arm-none-eabi-gcc -c -g -mcpu=cortex-m4 -mfloat-abi=hard -mfpu=vfpv4 -mthumb -I..\inc -I..\..\..\..\common\inc ..\..\..\..\common\src\txe_timer_deactivate.c
arm-none-eabi-gcc -c -g -mcpu=cortex-m4 -mfloat-abi=hard -mfpu=vfpv4 -mthumb -I..\inc -I..\..\..\..\common\inc ..\..\..\..\common\src\txe_timer_delete.c
arm-none-eabi-gcc -c -g -mcpu=cortex-m4 -mfloat-abi=hard -mfpu=vfpv4 -mthumb -I..\inc -I..\..\..\..\common\inc ..\..\..\..\common\src\txe_timer_info_get.c

arm-none-eabi-gcc -c -g -mcpu=cortex-m4 -mfloat-abi=hard -mfpu=vfpv4 -mthumb -I..\inc -I..\..\..\..\common\inc -I..\..\..\..\common_modules\inc -I..\..\..\..\common_modules\module_manager\inc ..\module_manager\src\txm_module_manager_alignment_adjust.c
arm-none-eabi-gcc -c -g -mcpu=cortex-m4 -mfloat-abi=hard -mfpu=vfpv4 -mthumb -I..\inc -I..\..\..\..\common\inc -I..\..\..\..\common_modules\inc -I..\..\..\..\common_modules\module_manager\inc ..\..\..\..\common_modules\module_manager\src\txm_module_manager_application_request.c
arm-none-eabi-gcc -c -g -mcpu=cortex-m4 -mfloat-abi=hard -mfpu=vfpv4 -mthumb -I..\inc -I..\..\..\..\common\inc -I..\..\..\..\common_modules\inc -I..\..\..\..\common_modules\module_manager\inc ..\..\..\..\common_modules\module_manager\src\txm_module_manager_callback_request.c
arm-none-eabi-gcc -c -g -mcpu=cortex-m4 -mfloat-abi=hard -mfpu=vfpv4 -mthumb -I..\inc -I..\..\..\..\common\inc -I..\..\..\..\common_modules\inc -I..\..\..\..\common_modules\module_manager\inc ..\..\..\..\common_modules\module_manager\src\txm_module_manager_event_flags_notify_trampoline.c
arm-none-eabi-gcc -c -g -mcpu=cortex-m4 -mfloat-abi=hard -mfpu=vfpv4 -mthumb -I..\inc -I..\..\..\..\common\inc -I..\..\..\..\common_modules\inc -I..\..\..\..\common_modules\module_manager\inc ..\module_manager\src\txm_module_manager_external_memory_enable.c
arm-none-eabi-gcc -c -g -mcpu=cortex-m4 -mfloat-abi=hard -mfpu=vfpv4 -mthumb -I..\inc -I..\..\..\..\common\inc -I..\..\..\..\common_modules\inc -I..\..\..\..\common_modules\module_manager\inc ..\..\..\..\common_modules\module_manager\src\txm_module_manager_file_load.c
arm-none-eabi-gcc -c -g -mcpu=cortex-m4 -mfloat-abi=hard -mfpu=vfpv4 -mthumb -I..\inc -I..\..\..\..\common\inc -I..\..\..\..\common_modules\inc -I..\..\..\..\common_modules\module_manager\inc ..\..\..\..\common_modules\module_manager\src\txm_module_manager_in_place_load.c
arm-none-eabi-gcc -c -g -mcpu=cortex-m4 -mfloat-abi=hard -mfpu=vfpv4 -mthumb -I..\inc -I..\..\..\..\common\inc -I..\..\..\..\common_modules\inc -I..\..\..\..\common_modules\module_manager\inc ..\..\..\..\common_modules\module_manager\src\txm_module_manager_internal_load.c
arm-none-eabi-gcc -c -g -mcpu=cortex-m4 -mfloat-abi=hard -mfpu=vfpv4 -mthumb -I..\inc -I..\..\..\..\common\inc -I..\..\..\..\common_modules\inc -I..\..\..\..\common_modules\module_manager\inc ..\..\..\..\common_modules\module_manager\src\txm_module_manager_initialize.c
arm-none-eabi-gcc -c -g -mcpu=cortex-m4 -mfloat-abi=hard -mfpu=vfpv4 -mthumb -I..\inc -I..\..\..\..\common\inc -I..\..\..\..\common_modules\inc -I..\..\..\..\common_modules\module_manager\inc ..\..\..\..\common_modules\module_manager\src\txm_module_manager_kernel_dispatch.c
arm-none-eabi-gcc -c -g -mcpu=cortex-m4 -mfloat-abi=hard -mfpu=vfpv4 -mthumb -I..\inc -I..\..\..\..\common\inc -I..\..\..\..\common_modules\inc -I..\..\..\..\common_modules\module_manager\inc ..\..\..\..\common_modules\module_manager\src\txm_module_manager_maximum_module_priority_set.c
arm-none-eabi-gcc -c -g -mcpu=cortex-m4 -mfloat-abi=hard -mfpu=vfpv4 -mthumb -I..\inc -I..\..\..\..\common\inc -I..\..\..\..\common_modules\inc -I..\..\..\..\common_modules\module_manager\inc ..\module_manager\src\txm_module_manager_memory_fault_handler.c
arm-none-eabi-gcc -c -g -mcpu=cortex-m4 -mfloat-abi=hard -mfpu=vfpv4 -mthumb -I..\inc -I..\..\..\..\common\inc -I..\..\..\..\common_modules\inc -I..\..\..\..\common_modules\module_manager\inc ..\module_manager\src\txm_module_manager_memory_fault_notify.c
arm-none-eabi-gcc -c -g -mcpu=cortex-m4 -mfloat-abi=hard -mfpu=vfpv4 -mthumb -I..\inc -I..\..\..\..\common\inc -I..\..\..\..\common_modules\inc -I..\..\..\..\common_modules\module_manager\inc ..\..\..\..\common_modules\module_manager\src\txm_module_manager_memory_load.c
arm-none-eabi-gcc -c -g -mcpu=cortex-m4 -mfloat-abi=hard -mfpu=vfpv4 -mthumb -I..\inc -I..\..\..\..\common\inc -I..\..\..\..\common_modules\inc -I..\..\..\..\common_modules\module_manager\inc ..\..\..\..\common_modules\module_manager\src\txm_module_manager_object_allocate.c
arm-none-eabi-gcc -c -g -mcpu=cortex-m4 -mfloat-abi=hard -mfpu=vfpv4 -mthumb -I..\inc -I..\..\..\..\common\inc -I..\..\..\..\common_modules\inc -I..\..\..\..\common_modules\module_manager\inc ..\..\..\..\common_modules\module_manager\src\txm_module_manager_object_deallocate.c
arm-none-eabi-gcc -c -g -mcpu=cortex-m4 -mfloat-abi=hard -mfpu=vfpv4 -mthumb -I..\inc -I..\..\..\..\common\inc -I..\..\..\..\common_modules\inc -I..\..\..\..\common_modules\module_manager\inc ..\..\..\..\common_modules\module_manager\src\txm_module_manager_object_pointer_get.c
arm-none-eabi-gcc -c -g -mcpu=cortex-m4 -mfloat-abi=hard -mfpu=vfpv4 -mthumb -I..\inc -I..\..\..\..\common\inc -I..\..\..\..\common_modules\inc -I..\..\..\..\common_modules\module_manager\inc ..\..\..\..\common_modules\module_manager\src\txm_module_manager_object_pointer_get_extended.c
arm-none-eabi-gcc -c -g -mcpu=cortex-m4 -mfloat-abi=hard -mfpu=vfpv4 -mthumb -I..\inc -I..\..\..\..\common\inc -I..\..\..\..\common_modules\inc -I..\..\..\..\common_modules\module_manager\inc ..\..\..\..\common_modules\module_manager\src\txm_module_manager_object_pool_create.c
arm-none-eabi-gcc -c -g -mcpu=cortex-m4 -mfloat-abi=hard -mfpu=vfpv4 -mthumb -I..\inc -I..\..\..\..\common\inc -I..\..\..\..\common_modules\inc -I..\..\..\..\common_modules\module_manager\inc ..\..\..\..\common_modules\module_manager\src\txm_module_manager_properties_get.c
arm-none-eabi-gcc -c -g -mcpu=cortex-m4 -mfloat-abi=hard -mfpu=vfpv4 -mthumb -I..\inc -I..\..\..\..\common\inc -I..\..\..\..\common_modules\inc -I..\..\..\..\common_modules\module_manager\inc ..\..\..\..\common_modules\module_manager\src\txm_module_manager_queue_notify_trampoline.c
arm-none-eabi-gcc -c -g -mcpu=cortex-m4 -mfloat-abi=hard -mfpu=vfpv4 -mthumb -I..\inc -I..\..\..\..\common\inc -I..\..\..\..\common_modules\inc -I..\..\..\..\common_modules\module_manager\inc ..\..\..\..\common_modules\module_manager\src\txm_module_manager_semaphore_notify_trampoline.c
arm-none-eabi-gcc -c -g -mcpu=cortex-m4 -mfloat-abi=hard -mfpu=vfpv4 -mthumb -I..\inc -I..\..\..\..\common\inc -I..\..\..\..\common_modules\inc -I..\..\..\..\common_modules\module_manager\inc ..\module_manager\src\txm_module_manager_mm_register_setup.c
arm-none-eabi-gcc -c -g -mcpu=cortex-m4 -mfloat-abi=hard -mfpu=vfpv4 -mthumb -I..\inc -I..\..\..\..\common\inc -I..\..\..\..\common_modules\inc -I..\..\..\..\common_modules\module_manager\inc ..\..\..\..\common_modules\module_manager\src\txm_module_manager_start.c
arm-none-eabi-gcc -c -g -mcpu=cortex-m4 -mfloat-abi=hard -mfpu=vfpv4 -mthumb -I..\inc -I..\..\..\..\common\inc -I..\..\..\..\common_modules\inc -I..\..\..\..\common_modules\module_manager\inc ..\..\..\..\common_modules\module_manager\src\txm_module_manager_stop.c
arm-none-eabi-gcc -c -g -mcpu=cortex-m4 -mfloat-abi=hard -mfpu=vfpv4 -mthumb -I..\inc -I..\..\..\..\common\inc -I..\..\..\..\common_modules\inc -I..\..\..\..\common_modules\module_manager\inc ..\..\..\..\common_modules\module_manager\src\txm_module_manager_thread_create.c
arm-none-eabi-gcc -c -g -mcpu=cortex-m4 -mfloat-abi=hard -mfpu=vfpv4 -mthumb -I..\inc -I..\..\..\..\common\inc -I..\..\..\..\common_modules\inc -I..\..\..\..\common_modules\module_manager\inc ..\..\..\..\common_modules\module_manager\src\txm_module_manager_thread_notify_trampoline.c
arm-none-eabi-gcc -c -g -mcpu=cortex-m4 -mfloat-abi=hard -mfpu=vfpv4 -mthumb -I..\inc -I..\..\..\..\common\inc -I..\..\..\..\common_modules\inc -I..\..\..\..\common_modules\module_manager\inc ..\..\..\..\common_modules\module_manager\src\txm_module_manager_thread_reset.c
arm-none-eabi-gcc -c -g -mcpu=cortex-m4 -mfloat-abi=hard -mfpu=vfpv4 -mthumb -I..\inc -I..\..\..\..\common\inc -I..\..\..\..\common_modules\inc -I..\..\..\..\common_modules\module_manager\inc ..\..\..\..\common_modules\module_manager\src\txm_module_manager_timer_notify_trampoline.c
arm-none-eabi-gcc -c -g -mcpu=cortex-m4 -mfloat-abi=hard -mfpu=vfpv4 -mthumb -I..\inc -I..\..\..\..\common\inc -I..\..\..\..\common_modules\inc -I..\..\..\..\common_modules\module_manager\inc ..\..\..\..\common_modules\module_manager\src\txm_module_manager_unload.c
arm-none-eabi-gcc -c -g -mcpu=cortex-m4 -mfloat-abi=hard -mfpu=vfpv4 -mthumb -I..\inc -I..\..\..\..\common\inc -I..\..\..\..\common_modules\inc -I..\..\..\..\common_modules\module_manager\inc ..\..\..\..\common_modules\module_manager\src\txm_module_manager_util.c


arm-none-eabi-ar -r tx.a tx_thread_stack_build.o tx_thread_schedule.o tx_thread_system_return.o tx_thread_context_save.o tx_thread_context_restore.o tx_timer_interrupt.o
arm-none-eabi-ar -r tx.a tx_thread_interrupt_control.o tx_initialize_low_level.o
arm-none-eabi-ar -r tx.a tx_block_allocate.o tx_block_pool_cleanup.o tx_block_pool_create.o tx_block_pool_delete.o tx_block_pool_info_get.o
arm-none-eabi-ar -r tx.a tx_block_pool_initialize.o tx_block_pool_performance_info_get.o tx_block_pool_performance_system_info_get.o tx_block_pool_prioritize.o
arm-none-eabi-ar -r tx.a tx_block_release.o tx_byte_allocate.o tx_byte_pool_cleanup.o tx_byte_pool_create.o tx_byte_pool_delete.o tx_byte_pool_info_get.o
arm-none-eabi-ar -r tx.a tx_byte_pool_initialize.o tx_byte_pool_performance_info_get.o tx_byte_pool_performance_system_info_get.o tx_byte_pool_prioritize.o
arm-none-eabi-ar -r tx.a tx_byte_pool_search.o tx_byte_release.o tx_event_flags_cleanup.o tx_event_flags_create.o tx_event_flags_delete.o tx_event_flags_get.o
arm-none-eabi-ar -r tx.a tx_event_flags_info_get.o tx_event_flags_initialize.o tx_event_flags_performance_info_get.o tx_event_flags_performance_system_info_get.o
arm-none-eabi-ar -r tx.a tx_event_flags_set.o tx_event_flags_set_notify.o tx_initialize_high_level.o tx_initialize_kernel_enter.o tx_initialize_kernel_setup.o
arm-none-eabi-ar -r tx.a tx_mutex_cleanup.o tx_mutex_create.o tx_mutex_delete.o tx_mutex_get.o tx_mutex_info_get.o tx_mutex_initialize.o tx_mutex_performance_info_get.o
arm-none-eabi-ar -r tx.a tx_mutex_performance_system_info_get.o tx_mutex_prioritize.o tx_mutex_priority_change.o tx_mutex_put.o tx_queue_cleanup.o tx_queue_create.o
arm-none-eabi-ar -r tx.a tx_queue_delete.o tx_queue_flush.o tx_queue_front_send.o tx_queue_info_get.o tx_queue_initialize.o tx_queue_performance_info_get.o 
arm-none-eabi-ar -r tx.a tx_queue_performance_system_info_get.o tx_queue_prioritize.o tx_queue_receive.o tx_queue_send.o tx_queue_send_notify.o tx_semaphore_ceiling_put.o
arm-none-eabi-ar -r tx.a tx_semaphore_cleanup.o tx_semaphore_create.o tx_semaphore_delete.o tx_semaphore_get.o tx_semaphore_info_get.o tx_semaphore_initialize.o
arm-none-eabi-ar -r tx.a tx_semaphore_performance_info_get.o tx_semaphore_performance_system_info_get.o tx_semaphore_prioritize.o tx_semaphore_put.o tx_semaphore_put_notify.o
arm-none-eabi-ar -r tx.a tx_thread_create.o tx_thread_delete.o tx_thread_entry_exit_notify.o tx_thread_identify.o tx_thread_info_get.o tx_thread_initialize.o
arm-none-eabi-ar -r tx.a tx_thread_performance_info_get.o tx_thread_performance_system_info_get.o tx_thread_preemption_change.o tx_thread_priority_change.o tx_thread_relinquish.o
arm-none-eabi-ar -r tx.a tx_thread_reset.o tx_thread_resume.o tx_thread_shell_entry.o tx_thread_sleep.o tx_thread_stack_analyze.o tx_thread_stack_error_handler.o
arm-none-eabi-ar -r tx.a tx_thread_stack_error_notify.o tx_thread_suspend.o tx_thread_system_preempt_check.o tx_thread_system_resume.o tx_thread_system_suspend.o
arm-none-eabi-ar -r tx.a tx_thread_terminate.o tx_thread_time_slice.o tx_thread_time_slice_change.o tx_thread_timeout.o tx_thread_wait_abort.o tx_time_get.o
arm-none-eabi-ar -r tx.a tx_time_set.o tx_timer_activate.o tx_timer_change.o tx_timer_create.o tx_timer_deactivate.o tx_timer_delete.o tx_timer_expiration_process.o
arm-none-eabi-ar -r tx.a tx_timer_info_get.o tx_timer_initialize.o tx_timer_performance_info_get.o tx_timer_performance_system_info_get.o tx_timer_system_activate.o
arm-none-eabi-ar -r tx.a tx_timer_system_deactivate.o tx_timer_thread_entry.o tx_trace_enable.o tx_trace_disable.o tx_trace_initialize.o tx_trace_interrupt_control.o 
arm-none-eabi-ar -r tx.a tx_trace_isr_enter_insert.o tx_trace_isr_exit_insert.o tx_trace_object_register.o tx_trace_object_unregister.o tx_trace_user_event_insert.o
arm-none-eabi-ar -r tx.a tx_trace_buffer_full_notify.o tx_trace_event_filter.o tx_trace_event_unfilter.o
arm-none-eabi-ar -r tx.a txe_block_allocate.o txe_block_pool_create.o txe_block_pool_delete.o txe_block_pool_info_get.o txe_block_pool_prioritize.o txe_block_release.o 
arm-none-eabi-ar -r tx.a txe_byte_allocate.o txe_byte_pool_create.o txe_byte_pool_delete.o txe_byte_pool_info_get.o txe_byte_pool_prioritize.o txe_byte_release.o 
arm-none-eabi-ar -r tx.a txe_event_flags_create.o txe_event_flags_delete.o txe_event_flags_get.o txe_event_flags_info_get.o txe_event_flags_set.o 
arm-none-eabi-ar -r tx.a txe_event_flags_set_notify.o txe_mutex_create.o txe_mutex_delete.o txe_mutex_get.o txe_mutex_info_get.o txe_mutex_prioritize.o
arm-none-eabi-ar -r tx.a txe_mutex_put.o txe_queue_create.o txe_queue_delete.o txe_queue_flush.o txe_queue_front_send.o txe_queue_info_get.o txe_queue_prioritize.o
arm-none-eabi-ar -r tx.a txe_queue_receive.o txe_queue_send.o txe_queue_send_notify.o txe_semaphore_ceiling_put.o txe_semaphore_create.o txe_semaphore_delete.o
arm-none-eabi-ar -r tx.a txe_semaphore_get.o txe_semaphore_info_get.o txe_semaphore_prioritize.o txe_semaphore_put.o txe_semaphore_put_notify.o txe_thread_create.o
arm-none-eabi-ar -r tx.a txe_thread_delete.o txe_thread_entry_exit_notify.o txe_thread_info_get.o txe_thread_preemption_change.o txe_thread_priority_change.o 
arm-none-eabi-ar -r tx.a txe_thread_relinquish.o txe_thread_reset.o txe_thread_resume.o txe_thread_suspend.o txe_thread_terminate.o txe_thread_time_slice_change.o
arm-none-eabi-ar -r tx.a txe_thread_wait_abort.o txe_timer_activate.o txe_timer_change.o txe_timer_create.o txe_timer_deactivate.o txe_timer_delete.o txe_timer_info_get.o

arm-none-eabi-ar -r tx.a txm_module_manager_alignment_adjust.o txm_module_manager_application_request.o txm_module_manager_callback_request.o
arm-none-eabi-ar -r tx.a txm_module_manager_event_flags_notify_trampoline.o txm_module_manager_external_memory_enable.o txm_module_manager_file_load.o
arm-none-eabi-ar -r tx.a txm_module_manager_in_place_load.o txm_module_manager_initialize.o
arm-none-eabi-ar -r tx.a txm_module_manager_kernel_dispatch.o txm_module_manager_maximum_module_priority_set.o txm_module_manager_memory_fault_handler.o
arm-none-eabi-ar -r tx.a txm_module_manager_memory_fault_notify.o txm_module_manager_memory_load.o txm_module_manager_object_pointer_get.o
arm-none-eabi-ar -r tx.a txm_module_manager_object_pool_create.o txm_module_manager_queue_notify_trampoline.o txm_module_manager_semaphore_notify_trampoline.o
arm-none-eabi-ar -r tx.a txm_module_manager_mm_register_setup.o txm_module_manager_start.o txm_module_manager_stop.o
arm-none-eabi-ar -r tx.a txm_module_manager_thread_create.o txm_module_manager_thread_notify_trampoline.o txm_module_manager_thread_reset.o
arm-none-eabi-ar -r tx.a txm_module_manager_timer_notify_trampoline.o txm_module_manager_unload.o txm_module_manager_thread_stack_build.o
arm-none-eabi-ar -r tx.a txm_module_manager_internal_load.o
arm-none-eabi-ar -r tx.a txm_module_manager_object_allocate.o
arm-none-eabi-ar -r tx.a txm_module_manager_object_deallocate.o
arm-none-eabi-ar -r tx.a txm_module_manager_object_pointer_get_extended.o
arm-none-eabi-ar -r tx.a txm_module_manager_properties_get.o
arm-none-eabi-ar -r tx.a txm_module_manager_util.o