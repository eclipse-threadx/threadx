/***************************************************************************
 * Copyright (c) 2026 Eclipse ThreadX contributors
 *
 * This program and the accompanying materials are made available under the
 * terms of the MIT License which is available at
 * https://opensource.org/licenses/MIT.
 *
 * SPDX-License-Identifier: MIT
 **************************************************************************/

#include <sys/lock.h>
#include "tx_api.h"
#include "tx_thread.h"
#include "rh850_board.h"
#include "rh850_cpu.h"

#define LIBC_LOCKS 4
#define LIBC_LOCKS_EXIT 97

struct __lock {
	TX_MUTEX mutex;
	volatile UINT ready;
};

struct __lock __lock___libc_recursive_mutex;

static struct __lock lock_pool[LIBC_LOCKS];
static UINT lock_used;

static int lock_usable(void)
{
	return (_tx_thread_current_ptr != TX_NULL) && (_tx_thread_system_state == 0U) &&
	       (RH850_STSR(RH850_CORE_MASK_SR) == RH850_CORE_MASK_NONE) &&
	       ((RH850_STSR(RH850_SR_PSW) & RH850_PSW_ID) == 0U);
}

static TX_MUTEX *lock_mutex(struct __lock *lock)
{
	if (!lock->ready) {
		TX_INTERRUPT_SAVE_AREA

		TX_DISABLE
		if (!lock->ready) {
			(void)tx_mutex_create(&lock->mutex, "libc", TX_INHERIT);
			lock->ready = 1;
		}
		TX_RESTORE
	}
	return &lock->mutex;
}

void __retarget_lock_init_recursive(_LOCK_T *lock)
{
	TX_INTERRUPT_SAVE_AREA

	TX_DISABLE
	if (lock_used == LIBC_LOCKS) {
		board_exit(LIBC_LOCKS_EXIT);
	}
	*lock = &lock_pool[lock_used++];
	TX_RESTORE
}

void __retarget_lock_init(_LOCK_T *lock)
{
	__retarget_lock_init_recursive(lock);
}

void __retarget_lock_close_recursive(_LOCK_T lock)
{
	(void)lock;
}

void __retarget_lock_close(_LOCK_T lock)
{
	(void)lock;
}

void __retarget_lock_acquire_recursive(_LOCK_T lock)
{
	if (lock_usable()) {
		(void)tx_mutex_get(lock_mutex(lock), TX_WAIT_FOREVER);
	}
}

void __retarget_lock_acquire(_LOCK_T lock)
{
	__retarget_lock_acquire_recursive(lock);
}

void __retarget_lock_release_recursive(_LOCK_T lock)
{
	if (lock_usable() && lock->ready && lock->mutex.tx_mutex_owner == _tx_thread_current_ptr) {
		(void)tx_mutex_put(&lock->mutex);
	}
}

void __retarget_lock_release(_LOCK_T lock)
{
	__retarget_lock_release_recursive(lock);
}
