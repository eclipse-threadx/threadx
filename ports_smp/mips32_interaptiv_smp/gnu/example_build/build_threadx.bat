del tx.a
mips-sde-elf-gcc -c -g -mmt -EL tx_initialize_low_level.S
mips-sde-elf-gcc -c -g -mmt -EL tx_thread_stack_build.S
mips-sde-elf-gcc -c -g -mmt -EL tx_thread_schedule.S
mips-sde-elf-gcc -c -g -mmt -EL tx_thread_system_return.S
mips-sde-elf-gcc -c -g -mmt -EL tx_thread_context_save.S
mips-sde-elf-gcc -c -g -mmt -EL tx_thread_context_restore.S
mips-sde-elf-gcc -c -g -mmt -EL tx_thread_interrupt_control.S
mips-sde-elf-gcc -c -g -mmt -EL tx_thread_smp_core_get.S
mips-sde-elf-gcc -c -g -mmt -EL tx_thread_smp_core_preempt.S
mips-sde-elf-gcc -c -g -mmt -EL tx_thread_smp_current_state_get.S
mips-sde-elf-gcc -c -g -mmt -EL tx_thread_smp_current_thread_get.S
mips-sde-elf-gcc -c -g -mmt -EL tx_thread_smp_initialize_wait.S
mips-sde-elf-gcc -c -g -mmt -EL tx_thread_smp_low_level_initialize.S
mips-sde-elf-gcc -c -g -mmt -EL tx_thread_smp_protect.S
mips-sde-elf-gcc -c -g -mmt -EL tx_thread_smp_time_get.S
mips-sde-elf-gcc -c -g -mmt -EL tx_thread_smp_unprotect.S
mips-sde-elf-gcc -c -g -mmt -EL tx_timer_interrupt.S
mips-sde-elf-gcc -c -g -mmt -EL tx_block_allocate.c
mips-sde-elf-gcc -c -g -mmt -EL tx_block_pool_cleanup.c
mips-sde-elf-gcc -c -g -mmt -EL tx_block_pool_create.c
mips-sde-elf-gcc -c -g -mmt -EL tx_block_pool_delete.c
mips-sde-elf-gcc -c -g -mmt -EL tx_block_pool_info_get.c
mips-sde-elf-gcc -c -g -mmt -EL tx_block_pool_initialize.c
mips-sde-elf-gcc -c -g -mmt -EL tx_block_pool_performance_info_get.c
mips-sde-elf-gcc -c -g -mmt -EL tx_block_pool_performance_system_info_get.c
mips-sde-elf-gcc -c -g -mmt -EL tx_block_pool_prioritize.c
mips-sde-elf-gcc -c -g -mmt -EL tx_block_release.c
mips-sde-elf-gcc -c -g -mmt -EL tx_byte_allocate.c
mips-sde-elf-gcc -c -g -mmt -EL tx_byte_pool_cleanup.c
mips-sde-elf-gcc -c -g -mmt -EL tx_byte_pool_create.c
mips-sde-elf-gcc -c -g -mmt -EL tx_byte_pool_delete.c
mips-sde-elf-gcc -c -g -mmt -EL tx_byte_pool_info_get.c
mips-sde-elf-gcc -c -g -mmt -EL tx_byte_pool_initialize.c
mips-sde-elf-gcc -c -g -mmt -EL tx_byte_pool_performance_info_get.c
mips-sde-elf-gcc -c -g -mmt -EL tx_byte_pool_performance_system_info_get.c
mips-sde-elf-gcc -c -g -mmt -EL tx_byte_pool_prioritize.c
mips-sde-elf-gcc -c -g -mmt -EL tx_byte_pool_search.c
mips-sde-elf-gcc -c -g -mmt -EL tx_byte_release.c
mips-sde-elf-gcc -c -g -mmt -EL tx_event_flags_cleanup.c
mips-sde-elf-gcc -c -g -mmt -EL tx_event_flags_create.c
mips-sde-elf-gcc -c -g -mmt -EL tx_event_flags_delete.c
mips-sde-elf-gcc -c -g -mmt -EL tx_event_flags_get.c
mips-sde-elf-gcc -c -g -mmt -EL tx_event_flags_info_get.c
mips-sde-elf-gcc -c -g -mmt -EL tx_event_flags_initialize.c
mips-sde-elf-gcc -c -g -mmt -EL tx_event_flags_performance_info_get.c
mips-sde-elf-gcc -c -g -mmt -EL tx_event_flags_performance_system_info_get.c
mips-sde-elf-gcc -c -g -mmt -EL tx_event_flags_set.c
mips-sde-elf-gcc -c -g -mmt -EL tx_event_flags_set_notify.c
mips-sde-elf-gcc -c -g -mmt -EL tx_initialize_high_level.c
mips-sde-elf-gcc -c -g -mmt -EL tx_initialize_kernel_enter.c
mips-sde-elf-gcc -c -g -mmt -EL tx_initialize_kernel_setup.c
mips-sde-elf-gcc -c -g -mmt -EL tx_mutex_cleanup.c
mips-sde-elf-gcc -c -g -mmt -EL tx_mutex_create.c
mips-sde-elf-gcc -c -g -mmt -EL tx_mutex_delete.c
mips-sde-elf-gcc -c -g -mmt -EL tx_mutex_get.c
mips-sde-elf-gcc -c -g -mmt -EL tx_mutex_info_get.c
mips-sde-elf-gcc -c -g -mmt -EL tx_mutex_initialize.c
mips-sde-elf-gcc -c -g -mmt -EL tx_mutex_performance_info_get.c
mips-sde-elf-gcc -c -g -mmt -EL tx_mutex_performance_system_info_get.c
mips-sde-elf-gcc -c -g -mmt -EL tx_mutex_prioritize.c
mips-sde-elf-gcc -c -g -mmt -EL tx_mutex_priority_change.c
mips-sde-elf-gcc -c -g -mmt -EL tx_mutex_put.c
mips-sde-elf-gcc -c -g -mmt -EL tx_queue_cleanup.c
mips-sde-elf-gcc -c -g -mmt -EL tx_queue_create.c
mips-sde-elf-gcc -c -g -mmt -EL tx_queue_delete.c
mips-sde-elf-gcc -c -g -mmt -EL tx_queue_flush.c
mips-sde-elf-gcc -c -g -mmt -EL tx_queue_front_send.c
mips-sde-elf-gcc -c -g -mmt -EL tx_queue_info_get.c
mips-sde-elf-gcc -c -g -mmt -EL tx_queue_initialize.c
mips-sde-elf-gcc -c -g -mmt -EL tx_queue_performance_info_get.c
mips-sde-elf-gcc -c -g -mmt -EL tx_queue_performance_system_info_get.c
mips-sde-elf-gcc -c -g -mmt -EL tx_queue_prioritize.c
mips-sde-elf-gcc -c -g -mmt -EL tx_queue_receive.c
mips-sde-elf-gcc -c -g -mmt -EL tx_queue_send.c
mips-sde-elf-gcc -c -g -mmt -EL tx_queue_send_notify.c
mips-sde-elf-gcc -c -g -mmt -EL tx_semaphore_ceiling_put.c
mips-sde-elf-gcc -c -g -mmt -EL tx_semaphore_cleanup.c
mips-sde-elf-gcc -c -g -mmt -EL tx_semaphore_create.c
mips-sde-elf-gcc -c -g -mmt -EL tx_semaphore_delete.c
mips-sde-elf-gcc -c -g -mmt -EL tx_semaphore_get.c
mips-sde-elf-gcc -c -g -mmt -EL tx_semaphore_info_get.c
mips-sde-elf-gcc -c -g -mmt -EL tx_semaphore_initialize.c
mips-sde-elf-gcc -c -g -mmt -EL tx_semaphore_performance_info_get.c
mips-sde-elf-gcc -c -g -mmt -EL tx_semaphore_performance_system_info_get.c
mips-sde-elf-gcc -c -g -mmt -EL tx_semaphore_prioritize.c
mips-sde-elf-gcc -c -g -mmt -EL tx_semaphore_put.c
mips-sde-elf-gcc -c -g -mmt -EL tx_semaphore_put_notify.c
mips-sde-elf-gcc -c -g -mmt -EL tx_thread_create.c
mips-sde-elf-gcc -c -g -mmt -EL tx_thread_delete.c
mips-sde-elf-gcc -c -g -mmt -EL tx_thread_entry_exit_notify.c
mips-sde-elf-gcc -c -g -mmt -EL tx_thread_identify.c
mips-sde-elf-gcc -c -g -mmt -EL tx_thread_info_get.c
mips-sde-elf-gcc -c -g -mmt -EL tx_thread_initialize.c
mips-sde-elf-gcc -c -g -mmt -EL tx_thread_performance_info_get.c
mips-sde-elf-gcc -c -g -mmt -EL tx_thread_performance_system_info_get.c
mips-sde-elf-gcc -c -g -mmt -EL tx_thread_preemption_change.c
mips-sde-elf-gcc -c -g -mmt -EL tx_thread_priority_change.c
mips-sde-elf-gcc -c -g -mmt -EL tx_thread_relinquish.c
mips-sde-elf-gcc -c -g -mmt -EL tx_thread_reset.c
mips-sde-elf-gcc -c -g -mmt -EL tx_thread_resume.c
mips-sde-elf-gcc -c -g -mmt -EL tx_thread_shell_entry.c
mips-sde-elf-gcc -c -g -mmt -EL tx_thread_sleep.c
mips-sde-elf-gcc -c -g -mmt -EL tx_thread_smp_core_exclude.c
mips-sde-elf-gcc -c -g -mmt -EL tx_thread_smp_core_exclude_get.c
mips-sde-elf-gcc -c -g -mmt -EL tx_thread_smp_current_state_set.c
mips-sde-elf-gcc -c -g -mmt -EL tx_thread_smp_debug_entry_insert.c
mips-sde-elf-gcc -c -g -mmt -EL tx_thread_smp_high_level_initialize.c
mips-sde-elf-gcc -c -g -mmt -EL tx_thread_smp_rebalance_execute_list.c
mips-sde-elf-gcc -c -g -mmt -EL tx_thread_smp_utilities.c
mips-sde-elf-gcc -c -g -mmt -EL tx_thread_stack_analyze.c
mips-sde-elf-gcc -c -g -mmt -EL tx_thread_stack_error_handler.c
mips-sde-elf-gcc -c -g -mmt -EL tx_thread_stack_error_notify.c
mips-sde-elf-gcc -c -g -mmt -EL tx_thread_suspend.c
mips-sde-elf-gcc -c -g -mmt -EL tx_thread_system_preempt_check.c
mips-sde-elf-gcc -c -g -mmt -EL tx_thread_system_resume.c
mips-sde-elf-gcc -c -g -mmt -EL tx_thread_system_suspend.c
mips-sde-elf-gcc -c -g -mmt -EL tx_thread_terminate.c
mips-sde-elf-gcc -c -g -mmt -EL tx_thread_time_slice.c
mips-sde-elf-gcc -c -g -mmt -EL tx_thread_time_slice_change.c
mips-sde-elf-gcc -c -g -mmt -EL tx_thread_timeout.c
mips-sde-elf-gcc -c -g -mmt -EL tx_thread_wait_abort.c
mips-sde-elf-gcc -c -g -mmt -EL tx_time_get.c
mips-sde-elf-gcc -c -g -mmt -EL tx_time_set.c
mips-sde-elf-gcc -c -g -mmt -EL tx_timer_activate.c
mips-sde-elf-gcc -c -g -mmt -EL tx_timer_change.c
mips-sde-elf-gcc -c -g -mmt -EL tx_timer_create.c
mips-sde-elf-gcc -c -g -mmt -EL tx_timer_deactivate.c
mips-sde-elf-gcc -c -g -mmt -EL tx_timer_delete.c
mips-sde-elf-gcc -c -g -mmt -EL tx_timer_expiration_process.c
mips-sde-elf-gcc -c -g -mmt -EL tx_timer_info_get.c
mips-sde-elf-gcc -c -g -mmt -EL tx_timer_initialize.c
mips-sde-elf-gcc -c -g -mmt -EL tx_timer_performance_info_get.c
mips-sde-elf-gcc -c -g -mmt -EL tx_timer_performance_system_info_get.c
mips-sde-elf-gcc -c -g -mmt -EL tx_timer_system_activate.c
mips-sde-elf-gcc -c -g -mmt -EL tx_timer_system_deactivate.c
mips-sde-elf-gcc -c -g -mmt -EL tx_timer_thread_entry.c
mips-sde-elf-gcc -c -g -mmt -EL tx_trace_enable.c
mips-sde-elf-gcc -c -g -mmt -EL tx_trace_disable.c
mips-sde-elf-gcc -c -g -mmt -EL tx_trace_initialize.c
mips-sde-elf-gcc -c -g -mmt -EL tx_trace_interrupt_control.c
mips-sde-elf-gcc -c -g -mmt -EL tx_trace_isr_enter_insert.c
mips-sde-elf-gcc -c -g -mmt -EL tx_trace_isr_exit_insert.c
mips-sde-elf-gcc -c -g -mmt -EL tx_trace_object_register.c
mips-sde-elf-gcc -c -g -mmt -EL tx_trace_object_unregister.c
mips-sde-elf-gcc -c -g -mmt -EL tx_trace_user_event_insert.c
mips-sde-elf-gcc -c -g -mmt -EL tx_trace_buffer_full_notify.c
mips-sde-elf-gcc -c -g -mmt -EL tx_trace_event_filter.c
mips-sde-elf-gcc -c -g -mmt -EL tx_trace_event_unfilter.c
mips-sde-elf-gcc -c -g -mmt -EL txe_block_allocate.c
mips-sde-elf-gcc -c -g -mmt -EL txe_block_pool_create.c
mips-sde-elf-gcc -c -g -mmt -EL txe_block_pool_delete.c
mips-sde-elf-gcc -c -g -mmt -EL txe_block_pool_info_get.c
mips-sde-elf-gcc -c -g -mmt -EL txe_block_pool_prioritize.c
mips-sde-elf-gcc -c -g -mmt -EL txe_block_release.c
mips-sde-elf-gcc -c -g -mmt -EL txe_byte_allocate.c
mips-sde-elf-gcc -c -g -mmt -EL txe_byte_pool_create.c
mips-sde-elf-gcc -c -g -mmt -EL txe_byte_pool_delete.c
mips-sde-elf-gcc -c -g -mmt -EL txe_byte_pool_info_get.c
mips-sde-elf-gcc -c -g -mmt -EL txe_byte_pool_prioritize.c
mips-sde-elf-gcc -c -g -mmt -EL txe_byte_release.c
mips-sde-elf-gcc -c -g -mmt -EL txe_event_flags_create.c
mips-sde-elf-gcc -c -g -mmt -EL txe_event_flags_delete.c
mips-sde-elf-gcc -c -g -mmt -EL txe_event_flags_get.c
mips-sde-elf-gcc -c -g -mmt -EL txe_event_flags_info_get.c
mips-sde-elf-gcc -c -g -mmt -EL txe_event_flags_set.c
mips-sde-elf-gcc -c -g -mmt -EL txe_event_flags_set_notify.c
mips-sde-elf-gcc -c -g -mmt -EL txe_mutex_create.c
mips-sde-elf-gcc -c -g -mmt -EL txe_mutex_delete.c
mips-sde-elf-gcc -c -g -mmt -EL txe_mutex_get.c
mips-sde-elf-gcc -c -g -mmt -EL txe_mutex_info_get.c
mips-sde-elf-gcc -c -g -mmt -EL txe_mutex_prioritize.c
mips-sde-elf-gcc -c -g -mmt -EL txe_mutex_put.c
mips-sde-elf-gcc -c -g -mmt -EL txe_queue_create.c
mips-sde-elf-gcc -c -g -mmt -EL txe_queue_delete.c
mips-sde-elf-gcc -c -g -mmt -EL txe_queue_flush.c
mips-sde-elf-gcc -c -g -mmt -EL txe_queue_front_send.c
mips-sde-elf-gcc -c -g -mmt -EL txe_queue_info_get.c
mips-sde-elf-gcc -c -g -mmt -EL txe_queue_prioritize.c
mips-sde-elf-gcc -c -g -mmt -EL txe_queue_receive.c
mips-sde-elf-gcc -c -g -mmt -EL txe_queue_send.c
mips-sde-elf-gcc -c -g -mmt -EL txe_queue_send_notify.c
mips-sde-elf-gcc -c -g -mmt -EL txe_semaphore_ceiling_put.c
mips-sde-elf-gcc -c -g -mmt -EL txe_semaphore_create.c
mips-sde-elf-gcc -c -g -mmt -EL txe_semaphore_delete.c
mips-sde-elf-gcc -c -g -mmt -EL txe_semaphore_get.c
mips-sde-elf-gcc -c -g -mmt -EL txe_semaphore_info_get.c
mips-sde-elf-gcc -c -g -mmt -EL txe_semaphore_prioritize.c
mips-sde-elf-gcc -c -g -mmt -EL txe_semaphore_put.c
mips-sde-elf-gcc -c -g -mmt -EL txe_semaphore_put_notify.c
mips-sde-elf-gcc -c -g -mmt -EL txe_thread_create.c
mips-sde-elf-gcc -c -g -mmt -EL txe_thread_delete.c
mips-sde-elf-gcc -c -g -mmt -EL txe_thread_entry_exit_notify.c
mips-sde-elf-gcc -c -g -mmt -EL txe_thread_info_get.c
mips-sde-elf-gcc -c -g -mmt -EL txe_thread_preemption_change.c
mips-sde-elf-gcc -c -g -mmt -EL txe_thread_priority_change.c
mips-sde-elf-gcc -c -g -mmt -EL txe_thread_relinquish.c
mips-sde-elf-gcc -c -g -mmt -EL txe_thread_reset.c
mips-sde-elf-gcc -c -g -mmt -EL txe_thread_resume.c
mips-sde-elf-gcc -c -g -mmt -EL txe_thread_suspend.c
mips-sde-elf-gcc -c -g -mmt -EL txe_thread_terminate.c
mips-sde-elf-gcc -c -g -mmt -EL txe_thread_time_slice_change.c
mips-sde-elf-gcc -c -g -mmt -EL txe_thread_wait_abort.c
mips-sde-elf-gcc -c -g -mmt -EL txe_timer_activate.c
mips-sde-elf-gcc -c -g -mmt -EL txe_timer_change.c
mips-sde-elf-gcc -c -g -mmt -EL txe_timer_create.c
mips-sde-elf-gcc -c -g -mmt -EL txe_timer_deactivate.c
mips-sde-elf-gcc -c -g -mmt -EL txe_timer_delete.c
mips-sde-elf-gcc -c -g -mmt -EL txe_timer_info_get.c
mips-sde-elf-ar -r tx.a tx_initialize_low_level.o tx_thread_stack_build.o tx_thread_schedule.o tx_thread_system_return.o tx_thread_context_save.o tx_thread_context_restore.o tx_timer_interrupt.o tx_thread_interrupt_control.o
mips-sde-elf-ar -r tx.a tx_thread_smp_core_get.o tx_thread_smp_core_preempt.o tx_thread_smp_current_state_get.o tx_thread_smp_current_thread_get.o tx_thread_smp_initialize_wait.o
mips-sde-elf-ar -r tx.a tx_thread_smp_low_level_initialize.o tx_thread_smp_protect.o tx_thread_smp_time_get.o tx_thread_smp_unprotect.o
mips-sde-elf-ar -r tx.a tx_block_allocate.o tx_block_pool_cleanup.o tx_block_pool_create.o tx_block_pool_delete.o tx_block_pool_info_get.o
mips-sde-elf-ar -r tx.a tx_block_pool_initialize.o tx_block_pool_performance_info_get.o tx_block_pool_performance_system_info_get.o tx_block_pool_prioritize.o
mips-sde-elf-ar -r tx.a tx_block_release.o tx_byte_allocate.o tx_byte_pool_cleanup.o tx_byte_pool_create.o tx_byte_pool_delete.o tx_byte_pool_info_get.o
mips-sde-elf-ar -r tx.a tx_byte_pool_initialize.o tx_byte_pool_performance_info_get.o tx_byte_pool_performance_system_info_get.o tx_byte_pool_prioritize.o
mips-sde-elf-ar -r tx.a tx_byte_pool_search.o tx_byte_release.o tx_event_flags_cleanup.o tx_event_flags_create.o tx_event_flags_delete.o tx_event_flags_get.o
mips-sde-elf-ar -r tx.a tx_event_flags_info_get.o tx_event_flags_initialize.o tx_event_flags_performance_info_get.o tx_event_flags_performance_system_info_get.o
mips-sde-elf-ar -r tx.a tx_event_flags_set.o tx_event_flags_set_notify.o tx_initialize_high_level.o tx_initialize_kernel_enter.o tx_initialize_kernel_setup.o
mips-sde-elf-ar -r tx.a tx_mutex_cleanup.o tx_mutex_create.o tx_mutex_delete.o tx_mutex_get.o tx_mutex_info_get.o tx_mutex_initialize.o tx_mutex_performance_info_get.o
mips-sde-elf-ar -r tx.a tx_mutex_performance_system_info_get.o tx_mutex_prioritize.o tx_mutex_priority_change.o tx_mutex_put.o tx_queue_cleanup.o tx_queue_create.o
mips-sde-elf-ar -r tx.a tx_queue_delete.o tx_queue_flush.o tx_queue_front_send.o tx_queue_info_get.o tx_queue_initialize.o tx_queue_performance_info_get.o 
mips-sde-elf-ar -r tx.a tx_queue_performance_system_info_get.o tx_queue_prioritize.o tx_queue_receive.o tx_queue_send.o tx_queue_send_notify.o tx_semaphore_ceiling_put.o
mips-sde-elf-ar -r tx.a tx_semaphore_cleanup.o tx_semaphore_create.o tx_semaphore_delete.o tx_semaphore_get.o tx_semaphore_info_get.o tx_semaphore_initialize.o
mips-sde-elf-ar -r tx.a tx_semaphore_performance_info_get.o tx_semaphore_performance_system_info_get.o tx_semaphore_prioritize.o tx_semaphore_put.o tx_semaphore_put_notify.o
mips-sde-elf-ar -r tx.a tx_thread_create.o tx_thread_delete.o tx_thread_entry_exit_notify.o tx_thread_identify.o tx_thread_info_get.o tx_thread_initialize.o
mips-sde-elf-ar -r tx.a tx_thread_performance_info_get.o tx_thread_performance_system_info_get.o tx_thread_preemption_change.o tx_thread_priority_change.o tx_thread_relinquish.o
mips-sde-elf-ar -r tx.a tx_thread_reset.o tx_thread_resume.o tx_thread_shell_entry.o tx_thread_sleep.o tx_thread_stack_analyze.o tx_thread_stack_error_handler.o
mips-sde-elf-ar -r tx.a tx_thread_smp_core_exclude.o tx_thread_smp_core_exclude_get.o tx_thread_smp_current_state_set.o tx_thread_smp_debug_entry_insert.o
mips-sde-elf-ar -r tx.a tx_thread_smp_high_level_initialize.o tx_thread_smp_rebalance_execute_list.o tx_thread_smp_utilities.o
mips-sde-elf-ar -r tx.a tx_thread_stack_error_notify.o tx_thread_suspend.o tx_thread_system_preempt_check.o tx_thread_system_resume.o tx_thread_system_suspend.o
mips-sde-elf-ar -r tx.a tx_thread_terminate.o tx_thread_time_slice.o tx_thread_time_slice_change.o tx_thread_timeout.o tx_thread_wait_abort.o tx_time_get.o
mips-sde-elf-ar -r tx.a tx_time_set.o tx_timer_activate.o tx_timer_change.o tx_timer_create.o tx_timer_deactivate.o tx_timer_delete.o tx_timer_expiration_process.o
mips-sde-elf-ar -r tx.a tx_timer_info_get.o tx_timer_initialize.o tx_timer_performance_info_get.o tx_timer_performance_system_info_get.o tx_timer_system_activate.o
mips-sde-elf-ar -r tx.a tx_timer_system_deactivate.o tx_timer_thread_entry.o tx_trace_enable.o tx_trace_disable.o tx_trace_initialize.o tx_trace_interrupt_control.o 
mips-sde-elf-ar -r tx.a tx_trace_isr_enter_insert.o tx_trace_isr_exit_insert.o tx_trace_object_register.o tx_trace_object_unregister.o tx_trace_user_event_insert.o
mips-sde-elf-ar -r tx.a tx_trace_buffer_full_notify.o tx_trace_event_filter.o tx_trace_event_unfilter.o
mips-sde-elf-ar -r tx.a txe_block_allocate.o txe_block_pool_create.o txe_block_pool_delete.o txe_block_pool_info_get.o txe_block_pool_prioritize.o txe_block_release.o 
mips-sde-elf-ar -r tx.a txe_byte_allocate.o txe_byte_pool_create.o txe_byte_pool_delete.o txe_byte_pool_info_get.o txe_byte_pool_prioritize.o txe_byte_release.o 
mips-sde-elf-ar -r tx.a txe_event_flags_create.o txe_event_flags_delete.o txe_event_flags_get.o txe_event_flags_info_get.o txe_event_flags_set.o 
mips-sde-elf-ar -r tx.a txe_event_flags_set_notify.o txe_mutex_create.o txe_mutex_delete.o txe_mutex_get.o txe_mutex_info_get.o txe_mutex_prioritize.o
mips-sde-elf-ar -r tx.a txe_mutex_put.o txe_queue_create.o txe_queue_delete.o txe_queue_flush.o txe_queue_front_send.o txe_queue_info_get.o txe_queue_prioritize.o
mips-sde-elf-ar -r tx.a txe_queue_receive.o txe_queue_send.o txe_queue_send_notify.o txe_semaphore_ceiling_put.o txe_semaphore_create.o txe_semaphore_delete.o
mips-sde-elf-ar -r tx.a txe_semaphore_get.o txe_semaphore_info_get.o txe_semaphore_prioritize.o txe_semaphore_put.o txe_semaphore_put_notify.o txe_thread_create.o
mips-sde-elf-ar -r tx.a txe_thread_delete.o txe_thread_entry_exit_notify.o txe_thread_info_get.o txe_thread_preemption_change.o txe_thread_priority_change.o 
mips-sde-elf-ar -r tx.a txe_thread_relinquish.o txe_thread_reset.o txe_thread_resume.o txe_thread_suspend.o txe_thread_terminate.o txe_thread_time_slice_change.o
mips-sde-elf-ar -r tx.a txe_thread_wait_abort.o txe_timer_activate.o txe_timer_change.o txe_timer_create.o txe_timer_deactivate.o txe_timer_delete.o txe_timer_info_get.o
