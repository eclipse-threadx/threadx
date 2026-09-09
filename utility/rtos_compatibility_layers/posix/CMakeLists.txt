# POSIX compatibility layer for ThreadX.
#
# This target is EXCLUDE_FROM_ALL: it is not part of the default build and is
# only produced when something asks for it, either by name or by linking it.
#
# The include directory is deliberately PUBLIC. The layer ships replacements for
# errno.h, fcntl.h, pthread.h, sched.h, signal.h and time.h, and application code
# has to see those rather than a toolchain's own copies. That is also why the
# layer cannot be used on the hosted simulation ports, where the C library
# already supplies them; the top-level CMakeLists.txt skips it there.

add_library(posix-threadx EXCLUDE_FROM_ALL)

target_include_directories(posix-threadx
    PUBLIC
    ${CMAKE_CURRENT_LIST_DIR}
)

target_sources(posix-threadx
    PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}/px_abs_time_to_rel_ticks.c
    ${CMAKE_CURRENT_LIST_DIR}/px_clock_getres.c
    ${CMAKE_CURRENT_LIST_DIR}/px_clock_gettime.c
    ${CMAKE_CURRENT_LIST_DIR}/px_clock_settime.c
    ${CMAKE_CURRENT_LIST_DIR}/px_cond_broadcast.c
    ${CMAKE_CURRENT_LIST_DIR}/px_cond_destroy.c
    ${CMAKE_CURRENT_LIST_DIR}/px_cond_init.c
    ${CMAKE_CURRENT_LIST_DIR}/px_cond_signal.c
    ${CMAKE_CURRENT_LIST_DIR}/px_cond_timedwait.c
    ${CMAKE_CURRENT_LIST_DIR}/px_cond_wait.c
    ${CMAKE_CURRENT_LIST_DIR}/px_error.c
    ${CMAKE_CURRENT_LIST_DIR}/px_in_thread_context.c
    ${CMAKE_CURRENT_LIST_DIR}/px_internal_signal_dispatch.c
    ${CMAKE_CURRENT_LIST_DIR}/px_memory_allocate.c
    ${CMAKE_CURRENT_LIST_DIR}/px_memory_release.c
    ${CMAKE_CURRENT_LIST_DIR}/px_mq_arrange_msg.c
    ${CMAKE_CURRENT_LIST_DIR}/px_mq_attr_init.c
    ${CMAKE_CURRENT_LIST_DIR}/px_mq_close.c
    ${CMAKE_CURRENT_LIST_DIR}/px_mq_create.c
    ${CMAKE_CURRENT_LIST_DIR}/px_mq_find_queue.c
    ${CMAKE_CURRENT_LIST_DIR}/px_mq_get_new_queue.c
    ${CMAKE_CURRENT_LIST_DIR}/px_mq_get_queue_desc.c
    ${CMAKE_CURRENT_LIST_DIR}/px_mq_open.c
    ${CMAKE_CURRENT_LIST_DIR}/px_mq_priority_search.c
    ${CMAKE_CURRENT_LIST_DIR}/px_mq_putback_queue.c
    ${CMAKE_CURRENT_LIST_DIR}/px_mq_queue_delete.c
    ${CMAKE_CURRENT_LIST_DIR}/px_mq_queue_init.c
    ${CMAKE_CURRENT_LIST_DIR}/px_mq_receive.c
    ${CMAKE_CURRENT_LIST_DIR}/px_mq_reset_queue.c
    ${CMAKE_CURRENT_LIST_DIR}/px_mq_send.c
    ${CMAKE_CURRENT_LIST_DIR}/px_mq_unlink.c
    ${CMAKE_CURRENT_LIST_DIR}/px_mx_attr_destroy.c
    ${CMAKE_CURRENT_LIST_DIR}/px_mx_attr_getprotocol.c
    ${CMAKE_CURRENT_LIST_DIR}/px_mx_attr_getpshared.c
    ${CMAKE_CURRENT_LIST_DIR}/px_mx_attr_gettype.c
    ${CMAKE_CURRENT_LIST_DIR}/px_mx_attr_initi.c
    ${CMAKE_CURRENT_LIST_DIR}/px_mx_attr_setprotocol.c
    ${CMAKE_CURRENT_LIST_DIR}/px_mx_attr_setpshared.c
    ${CMAKE_CURRENT_LIST_DIR}/px_mx_attr_settype.c
    ${CMAKE_CURRENT_LIST_DIR}/px_mx_destroy.c
    ${CMAKE_CURRENT_LIST_DIR}/px_mx_init.c
    ${CMAKE_CURRENT_LIST_DIR}/px_mx_lock.c
    ${CMAKE_CURRENT_LIST_DIR}/px_mx_set_default_mutexattr.c
    ${CMAKE_CURRENT_LIST_DIR}/px_mx_timedlock.c
    ${CMAKE_CURRENT_LIST_DIR}/px_mx_trylock.c
    ${CMAKE_CURRENT_LIST_DIR}/px_mx_unlock.c
    ${CMAKE_CURRENT_LIST_DIR}/px_nanosleep.c
    ${CMAKE_CURRENT_LIST_DIR}/px_pth_attr_destroy.c
    ${CMAKE_CURRENT_LIST_DIR}/px_pth_attr_getdetachstate.c
    ${CMAKE_CURRENT_LIST_DIR}/px_pth_attr_getinheritsched.c
    ${CMAKE_CURRENT_LIST_DIR}/px_pth_attr_getschedparam.c
    ${CMAKE_CURRENT_LIST_DIR}/px_pth_attr_getschedpolicy.c
    ${CMAKE_CURRENT_LIST_DIR}/px_pth_attr_getstack.c
    ${CMAKE_CURRENT_LIST_DIR}/px_pth_attr_getstackaddr.c
    ${CMAKE_CURRENT_LIST_DIR}/px_pth_attr_getstacksize.c
    ${CMAKE_CURRENT_LIST_DIR}/px_pth_attr_init.c
    ${CMAKE_CURRENT_LIST_DIR}/px_pth_attr_setdetachstate.c
    ${CMAKE_CURRENT_LIST_DIR}/px_pth_attr_setinheritsched.c
    ${CMAKE_CURRENT_LIST_DIR}/px_pth_attr_setschedparam.c
    ${CMAKE_CURRENT_LIST_DIR}/px_pth_attr_setschedpolicyl.c
    ${CMAKE_CURRENT_LIST_DIR}/px_pth_attr_setstack.c
    ${CMAKE_CURRENT_LIST_DIR}/px_pth_attr_setstackaddr.c
    ${CMAKE_CURRENT_LIST_DIR}/px_pth_attr_setstacksize.c
    ${CMAKE_CURRENT_LIST_DIR}/px_pth_cancel.c
    ${CMAKE_CURRENT_LIST_DIR}/px_pth_create.c
    ${CMAKE_CURRENT_LIST_DIR}/px_pth_detach.c
    ${CMAKE_CURRENT_LIST_DIR}/px_pth_equal.c
    ${CMAKE_CURRENT_LIST_DIR}/px_pth_exit.c
    ${CMAKE_CURRENT_LIST_DIR}/px_pth_getcanceltype.c
    ${CMAKE_CURRENT_LIST_DIR}/px_pth_getschedparam.c
    ${CMAKE_CURRENT_LIST_DIR}/px_pth_init.c
    ${CMAKE_CURRENT_LIST_DIR}/px_pth_join.c
    ${CMAKE_CURRENT_LIST_DIR}/px_pth_kill.c
    ${CMAKE_CURRENT_LIST_DIR}/px_pth_once.c
    ${CMAKE_CURRENT_LIST_DIR}/px_pth_self.c
    ${CMAKE_CURRENT_LIST_DIR}/px_pth_set_default_pthread_attr.c
    ${CMAKE_CURRENT_LIST_DIR}/px_pth_setcancelstate.c
    ${CMAKE_CURRENT_LIST_DIR}/px_pth_setcanceltype.c
    ${CMAKE_CURRENT_LIST_DIR}/px_pth_setschedparam.c
    ${CMAKE_CURRENT_LIST_DIR}/px_pth_sigmask.c
    ${CMAKE_CURRENT_LIST_DIR}/px_pth_testcancel.c
    ${CMAKE_CURRENT_LIST_DIR}/px_pth_yield.c
    ${CMAKE_CURRENT_LIST_DIR}/px_px_initialize.c
    ${CMAKE_CURRENT_LIST_DIR}/px_sched_get_prio.c
    ${CMAKE_CURRENT_LIST_DIR}/px_sched_yield.c
    ${CMAKE_CURRENT_LIST_DIR}/px_sem_close.c
    ${CMAKE_CURRENT_LIST_DIR}/px_sem_destroy.c
    ${CMAKE_CURRENT_LIST_DIR}/px_sem_find_sem.c
    ${CMAKE_CURRENT_LIST_DIR}/px_sem_get_new_sem.c
    ${CMAKE_CURRENT_LIST_DIR}/px_sem_getvalue.c
    ${CMAKE_CURRENT_LIST_DIR}/px_sem_init.c
    ${CMAKE_CURRENT_LIST_DIR}/px_sem_open.c
    ${CMAKE_CURRENT_LIST_DIR}/px_sem_post.c
    ${CMAKE_CURRENT_LIST_DIR}/px_sem_reset.c
    ${CMAKE_CURRENT_LIST_DIR}/px_sem_set_sem_name.c
    ${CMAKE_CURRENT_LIST_DIR}/px_sem_trywait.c
    ${CMAKE_CURRENT_LIST_DIR}/px_sem_unlink.c
    ${CMAKE_CURRENT_LIST_DIR}/px_sem_wait.c
    ${CMAKE_CURRENT_LIST_DIR}/px_sig_addset.c
    ${CMAKE_CURRENT_LIST_DIR}/px_sig_delset.c
    ${CMAKE_CURRENT_LIST_DIR}/px_sig_emptyset.c
    ${CMAKE_CURRENT_LIST_DIR}/px_sig_fillset.c
    ${CMAKE_CURRENT_LIST_DIR}/px_sig_signal.c
    ${CMAKE_CURRENT_LIST_DIR}/px_sig_wait.c
    ${CMAKE_CURRENT_LIST_DIR}/px_sleep.c
    ${CMAKE_CURRENT_LIST_DIR}/px_system_manager.c
)

target_link_libraries(posix-threadx PUBLIC threadx)
