
/**************************************************************************/
/*   Copyright (c) Cadence Design Systems, Inc.                           */
/*                                                                        */
/* Permission is hereby granted, free of charge, to any person obtaining  */
/* a copy of this software and associated documentation files (the        */
/* "Software"), to deal in the Software without restriction, including    */
/* without limitation the rights to use, copy, modify, merge, publish,    */
/* distribute, sublicense, and/or sell copies of the Software, and to     */
/* permit persons to whom the Software is furnished to do so, subject to  */
/* the following conditions:                                              */
/*                                                                        */
/* The above copyright notice and this permission notice shall be         */
/* included in all copies or substantial portions of the Software.        */
/*                                                                        */
/* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,        */
/* EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF     */
/* MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. */
/* IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY   */
/* CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,   */
/* TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE      */
/* SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.                 */
/**************************************************************************/

/**************************************************************************/
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This file contains the interface functions to provide thread-safe   */
/*    operation of the C library. Both newlib and the Xtensa C Library    */
/*    are supported.                                                      */
/*                                                                        */
/*  RELEASE HISTORY                                                       */
/*                                                                        */
/*    DATE              NAME                      DESCRIPTION             */
/*                                                                        */
/*  12-31-2020     Cadence Design Systems   Initial Version 6.1.3         */
/*                                                                        */
/**************************************************************************/

#include "tx_api.h"     /* TX_THREAD_SAFE_CLIB may be defined by tx_port.h */
#include "tx_thread.h"
#include "tx_initialize.h"

/* Xtensa specific */
#include <xtensa/config/system.h>

#ifdef TX_THREAD_SAFE_CLIB    /* this file is only needed if using C lib */

#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>


#if XSHAL_CLIB == XTHAL_CLIB_NEWLIB

#include <malloc.h>

/* NOTE: should have been declared in reent.h... */
extern void _wrapup_reent(struct _reent * ptr);

/* Mutex used for all C library protection */
TX_MUTEX  clib_lock_mutex;


/**************************************************************************/
/*    __malloc_lock - called by the malloc() family of routines when they */
/*    need to lock the memory pool. A call to malloc() may call this      */
/*    function recursively.                                               */
/**************************************************************************/
void
__malloc_lock (struct _reent * ptr)
{
    if (_tx_thread_system_state != TX_INITIALIZE_IS_FINISHED) {
        return;
    }

    tx_mutex_get (&clib_lock_mutex, TX_WAIT_FOREVER);
}

/**************************************************************************/
/*    __malloc_unlock - called by the malloc() family of routines when    */
/*    need to unlock the memory pool.                                     */
/**************************************************************************/
void
__malloc_unlock (struct _reent * ptr)
{
    if (_tx_thread_system_state != TX_INITIALIZE_IS_FINISHED) {
        return;
    }

#ifndef THREADX_TESTSUITE   /* see THREADX_TESTSUITE comments below */
    tx_mutex_prioritize (&clib_lock_mutex);    /* is this at all necessary? */
#endif
    tx_mutex_put (&clib_lock_mutex);
}


/**************************************************************************/
/*    __env_lock - called by the setenv() family of routines when they    */
/*    need to modify the environment. A call to setenv() may call this    */
/*    function recursively.                                               */
/**************************************************************************/
void
__env_lock (struct _reent * ptr)
{
    if (_tx_thread_system_state != TX_INITIALIZE_IS_FINISHED) {
        return;
    }

    tx_mutex_get (&clib_lock_mutex, TX_WAIT_FOREVER);
}

/**************************************************************************/
/*    __env_unlock - called by the setenv() family of routines when they  */
/*    need to unlock the environment.                                     */
/**************************************************************************/
void
__env_unlock (struct _reent * ptr)
{
    if (_tx_thread_system_state != TX_INITIALIZE_IS_FINISHED) {
        return;
    }

    tx_mutex_prioritize (&clib_lock_mutex);
    tx_mutex_put (&clib_lock_mutex);
}

#endif /* XSHAL_CLIB == XTHAL_CLIB_NEWLIB */

#if XSHAL_CLIB == XTHAL_CLIB_XCLIB

#include <errno.h>
#include <sys/reent.h>
    
#define XT_NUM_CLIB_LOCKS      (_MAX_LOCK + FOPEN_MAX)

typedef TX_MUTEX * _Rmtx;

static TX_MUTEX xclib_locks[XT_NUM_CLIB_LOCKS];
static uint32_t lcnt;

/* Override this and set to nonzero to enable locking. */
int32_t _xclib_use_mt = 1;


/**************************************************************************/
/*    _Mtxinit - initialize a lock. Called once for each lock.            */
/**************************************************************************/
void
_Mtxinit (_Rmtx * mtx)
{
    TX_MUTEX * lock;

    if (lcnt >= XT_NUM_CLIB_LOCKS) {
        /* Fatal error */
    }

    lock = &(xclib_locks[lcnt]);
    lcnt++;

    /* See notes for newlib case below. */
#ifdef THREADX_TESTSUITE
    tx_mutex_create (lock, "Clib lock", 0);
#else
    tx_mutex_create (lock, "Clib lock", TX_INHERIT);
#endif

    *mtx = lock;
}

/**************************************************************************/
/*    _Mtxdst - destroy a lock. Called once for each lock.                */
/**************************************************************************/
void
_Mtxdst (_Rmtx * mtx)
{
    if ((mtx) && (*mtx)) {
        tx_mutex_delete (*mtx);
    }
}

/**************************************************************************/
/*    _Mtxlock - acquire lock.                                            */
/**************************************************************************/
void
_Mtxlock (_Rmtx * mtx)
{
    if ((mtx) && (*mtx)) {
        tx_mutex_get (*mtx, TX_WAIT_FOREVER);
    }
}

/**************************************************************************/
/*    _Mtxunlock - release a lock.                                        */
/**************************************************************************/
void
_Mtxunlock (_Rmtx * mtx)
{
    if ((mtx) && (*mtx)) {
        tx_mutex_put (*mtx);
    }
}

#endif /* XSHAL_CLIB == XTHAL_CLIB_XCLIB */


/**************************************************************************/
/*    _sbrk_r - heap allocator. This function is called when the memory   */
/*    allocator needs a new chunk of memory.                              */
/*    The bounds of the heap area are global variables so that customer   */
/*    startup code can easily override them if needed.                    */
/*                                                                        */
/*    _tx_clib_heap_start    is the start of memory assigned to the heap  */
/*                           or 0 (NULL) if no memory is assigned (in     */
/*                           that case all calls to malloc will fail).    */
/*                                                                        */
/*   _tx_clib_heap_end       is the end of memory assigned to the heap    */
/*                           or 0 (NULL) if no memory is assigned. If a   */
/*                           nonzero start value is set then a nonzero    */
/*                           end value must be set.                       */
/**************************************************************************/

char * _tx_clib_heap_start = NULL;
char * _tx_clib_heap_end   = NULL;

void *
_sbrk_r (struct _reent * reent, int32_t incr)
{
    static char * heap_ptr;
    char * new_heap_ptr;
    char * alloc_ptr;

    /* The heap is bound by _tx_clib_heap_{start,end}. */
    if (heap_ptr == NULL) {
        heap_ptr = _tx_clib_heap_start;
    }

    new_heap_ptr = heap_ptr + incr;
    if ((heap_ptr == NULL) ||                   /* no heap        */
        (new_heap_ptr >= _tx_clib_heap_end) ||  /* heap exhausted */
        (new_heap_ptr < heap_ptr)) {            /* wraparound     */
        reent->_errno = ENOMEM;
        return (void *) -1;
    }

    alloc_ptr = heap_ptr;
    heap_ptr = new_heap_ptr;
    return (void *) alloc_ptr;
}


/**************************************************************************/
/*    _tx_clib_init - initialize C library thread safety support.         */
/*    Called by _tx_initialize_low_level().                               */
/**************************************************************************/
void
_tx_clib_init (void)
{
#if XSHAL_CLIB == XTHAL_CLIB_NEWLIB
#ifdef THREADX_TESTSUITE
  /*  Priority inheritance causes printf() (which calls malloc()
      which calls __malloc_unlock() which calls tx_mutex_put()
      which calls _tx_mutex_priority_change() if TX_INHERIT is set)
      which causes the task to suspend and resume which sometimes
      changes execution order in the very sensitive testsuite
      and makes it fail.  So, for the testsuite, don't request
      priority inheritance (it doesn't need it in any case).  */
    tx_mutex_create (&clib_lock_mutex, "Clib lock", 0);
#else
    tx_mutex_create (&clib_lock_mutex, "Clib lock", TX_INHERIT);
#endif
#endif /* NEWLIB */

#if XSHAL_CLIB == XTHAL_CLIB_XCLIB
    /* Nothing. */
#endif /* XCLIB */
}


/**************************************************************************/
/*    _tx_clib_reent_init - initialize C library thread reent structure.  */
/*    Called by tx_thread_create() to init per-thread C library state.    */
/**************************************************************************/
void
_tx_clib_reent_init (TX_THREAD * thread_ptr)
{
    if (thread_ptr == NULL) {
        /* Should never happen */
        return;
    }

#if XSHAL_CLIB == XTHAL_CLIB_NEWLIB
    struct _reent * reent = &(thread_ptr->tx_thread_clib_reent);

    memset (reent, 0, sizeof(struct _reent));
    _REENT_INIT_PTR (reent);
    thread_ptr->tx_thread_clib_ptr = reent;
#endif

#if XSHAL_CLIB == XTHAL_CLIB_XCLIB
    thread_ptr->tx_thread_clib_ptr = &(thread_ptr->tx_thread_clib_reent);
    _init_reent (thread_ptr->tx_thread_clib_ptr);
#endif
}


/**************************************************************************/
/*    _tx_clib_reent_cleanup - clean up C library thread reent structure. */
/*    Called by tx_thread_delete() to clean up per-thread C library state */
/*    and free any allocated memory (partial = 0).                        */
/*    Called by tx_thread_shell_entry and tx_thread_terminate to perform  */
/*    "atexit" processing and clean up stdio, but leaving the rest of the */
/*    structure intact so the thread can be restarted (partial = 1).      */
/**************************************************************************/
void
_tx_clib_reent_cleanup (TX_THREAD * thread_ptr, int32_t partial)
{
#if XSHAL_CLIB == XTHAL_CLIB_NEWLIB
    struct _reent * reent = &(thread_ptr->tx_thread_clib_reent);
    FILE * fp = &(reent->__sf[0]);
    int32_t i;

  /* Avoid closing stdin,stdout,stderr so other threads can still use them. */
    for (i = 0; i < 3; i++) {
        fp->_close = NULL;
        fp++;
    }

    if (partial != 0) {
        /* Perform "atexit" processing and clean up stdio. */
        _wrapup_reent (reent);
    }
    else {
        /* Free all the heap memory allocated in the reent structure.
           ThreadX requires that the thread has either exited or been
           terminated before it can be deleted so we can assume that
           _wrapup_reent has already been called for this thread. */
        _reclaim_reent (reent);
    }
#endif

#if XSHAL_CLIB == XTHAL_CLIB_XCLIB
  /* Unused, keep compiler happy */
  (void) partial;

  /* File handle table is global; no allocated memory in struct. */
  thread_ptr->tx_thread_clib_ptr = 0;
#endif
}


/**************************************************************************/
/*    _xt_wrapper - thread wrapper to handle C library init/cleanup.      */
/*    If C library thread safety is enabled, every thread is invoked      */
/*    via this wrapper in order to handle thread context setup/cleanup.   */
/**************************************************************************/
void
_xt_thread_wrapper (ULONG arg)
{
    TX_INTERRUPT_SAVE_AREA
    TX_THREAD * thread_ptr = _tx_thread_current_ptr;

    /* No use for this parameter */
    (void) arg;

    /* Init the C library thread context */
    _tx_clib_reent_init (thread_ptr);

    /* Disable interrupts around the global context ptr update */
    TX_DISABLE

#if XSHAL_CLIB == XTHAL_CLIB_NEWLIB
    _impure_ptr = thread_ptr->tx_thread_clib_ptr;
#endif
#if XSHAL_CLIB == XTHAL_CLIB_XCLIB
    _reent_ptr = thread_ptr->tx_thread_clib_ptr;
#endif

    TX_RESTORE

    /* Call actual thread entry point */
    (thread_ptr->tx_real_thread_entry)(thread_ptr->tx_thread_entry_parameter);

    /* Clean up C library thread context */
    _tx_clib_reent_cleanup(thread_ptr, 1);
    _tx_clib_reent_cleanup(thread_ptr, 0);
}


/**************************************************************************/
/*    _tx_clib_thread_setup - Xtensa-specific thread setup actions.       */
/*    This function will be called only if thread safe C library usage    */
/*    is enabled. It inserts the wrapper as the thread entry point and    */
/*    saves the actual entry point for later use.                         */
/**************************************************************************/
void
_tx_clib_thread_setup (TX_THREAD * thread_ptr)
{
    thread_ptr->tx_real_thread_entry = thread_ptr->tx_thread_entry;
    thread_ptr->tx_thread_entry      = &(_xt_thread_wrapper);
}

#endif /* TX_THREAD_SAFE_CLIB */

