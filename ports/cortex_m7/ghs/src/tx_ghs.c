/*
 *                ThreadX C/C++ Library Support
 *
 *      Copyright 1983-2019 Green Hills Software LLC.
 *
 *  This program is the property of Green Hills Software LLC.,
 *  its contents are proprietary information and no part of it
 *  is to be disclosed to anyone except employees of Green Hills
 *  Software LLC., or as agreed in writing signed by the President
 *  of Green Hills Software LLC.
 */

#include "tx_ghs.h"
#ifndef TX_DISABLE_ERROR_CHECKING
#define TX_DISABLE_ERROR_CHECKING
#endif
#include "tx_api.h"
#include <setjmp.h>
#include <string.h>

/* Allow these routines to access the following ThreadX global variables.  */
extern ULONG      _tx_thread_created_count;
extern TX_THREAD *_tx_thread_created_ptr;
extern TX_THREAD *_tx_thread_current_ptr;

#if defined(__ghs) && (__GHS_VERSION_NUMBER >= 500)
/* Thread-local storage routines for Green Hills releases 5.x and above. */
/*
  Thread-Local (Per-Thread) Library Data Retrieval
  ================================================

  __ghs_ThreadLocalStorage_specifier defines all library data items
  that the Green Hills libraries allow to be allocated per-thread.

  An implementation can choose which of these data items to allocate
  for each thread. For example, an implementation may choose to
  allocate an errno value for each thread, but not the strtok_saved_pos
  pointer. The application could then use strtok_r instead of strtok for
  correct operation.

  To add per-thread library data, define one of the
  TX_THREAD_EXTENSION_* macros in tx_port.h to include the data item
  or items in each thread control block TX_THREAD.

  If C++ with exceptions is being used, the __eh_globals entry must be
  allocated for each thread. This is typically done by default using
  TX_THREAD_EXTENSION_1 in tx_port.h.

  If __ghs_GetThreadLocalStorageItem is customized to return a
  per-thread errno value, you should also:

  * Customize the System Library for your project
  * Define the preprocessor symbol USE_THREAD_LOCAL_ERRNO in
    src/libsys/ind_errn.c

  If you customize the System Library, you should remove ind_thrd.c
  from the libsys.gpj subproject.
  
 */

/* Provide global __eh_globals value to support C++ exception handling
   outside a thread context. This name also forces this module to be
   included in the linked program instead of the ind_thrd.o module from
   the System Library libsys.a.
 */
static void *__eh_globals;

#pragma ghs startnomisra
void *__ghs_GetThreadLocalStorageItem(int specifier)
{
    void *ptlsitem = (void *)0;
    switch (specifier) {
	case (int)__ghs_TLS_Errno:
	    /* Set ptslsitem to the address of the per-thread errno value.
	       The per-thread errno value should have the type int.

	       If returning a per-thread errno value, follow the steps
	       above.

	       This item is used by numerous library functions.
	    */
	    break;
	case (int)__ghs_TLS_SignalHandlers:
	    /* Set ptslsitem to the address of the per-thread SignalHandlers
	       array. The per-thread SignalHandlers array should have the
	       array type as in the following declaration:
	       SignalHandler SignalHandlers[_SIGMAX];
	       The SignalHandler type and _SIGMAX constant are defined in
	       ind_thrd.h.

	       This item is used by the library functions signal() and
	       raise().
	    */
	    break;
	case (int)__ghs_TLS_asctime_buff:
	    /* Set ptslsitem to the address of the per-thread asctime_buff
	       array. The per-thread asctime_buff array should have the
	       array type as in the following declaration:
	       char asctime_buff[30];

	       This item is used by the library functions asctime() and
	       ctime(). The library provides asctime_r() and ctime_r(),
	       inherently thread-safe versions of these functions.
	    */
	    break;
	case (int)__ghs_TLS_tmpnam_space:
	    /* Set ptslsitem to the address of the per-thread tmpnam_space
	       array. The per-thread tmpnam_space array should have the
	       array type as in the following declaration:
	       char tmpnam_space[L_tmpnam];
	       The constant is defined in <stdio.h>

	       This item is used by the library function tmpnam() when
	       passed NULL. The library provides tmpnam_r(), an
	       inherently thread-safe version of tmpnam().
	    */
	    break;
	case (int)__ghs_TLS_strtok_saved_pos:
	    /* Set ptslsitem to the address of the per-thread
	       strtok_saved_pos pointer. The per-thread strtok_saved_pos
	       pointer should have the type "char *".

	       This item is used by the library function strtok().
	       The library provides strtok_r(), an inherently thread-safe
	       version of strtok().
	    */
	    break;
	case (int)__ghs_TLS_gmtime_temp:
	    /* Set ptslsitem to the address of the per-thread gmtime_temp
	       value. The per-thread gmtime_temp value should have the
	       type "struct tm" defined in time.h, included by indos.h.

	       This item is used by the library functions gmtime() and
	       localtime(). The library provides gmtime_r() and
	       localtime_r(), inherently thread-safe versions of these
	       functions.
	    */
	    break;
	case (int)__ghs_TLS___eh_globals:
	    /* Set ptslsitem to the address of the per-thread __eh_globals
	       value. The per-thread __eh_globals value should have the
	       type "void *".

	       This item is used by C++ exception handling.
	    */
	    if (_tx_thread_current_ptr)
		ptlsitem = (void *)&(_tx_thread_current_ptr->tx_thread_eh_globals);
	    else
		/* Use the global __eh_globals pointer.  */
		ptlsitem = (void *)&__eh_globals;
	    break;
    }
    return ptlsitem;
}
#pragma ghs endnomisra
#else
/* Thread-local storage routines for Green Hills releases 4.x and 3.x . */

/*
 * ThreadX C and C++ thread-safe library support routines.
 *
 * This implementation merely tries to guarantee thread safety within
 * individual C library calls such as malloc() and free(), but it does
 * not attempt to solve the problems associated with the following
 * multithreaded issues:
 *
 * 1. Use of errno.  This can be made thread-safe by adding errno
 *    to TX_THREAD_PORT_EXTENSION and using that within a modified
 *    version of libsys/ind_errno.c.
 *
 * 2. Thread safety ACROSS library calls.  Certain C library calls either
 *    return pointers to statically-allocated data structures or maintain
 *    state across calls.  These include strtok(), asctime(), gmtime(),
 *    tmpnam(NULL), signal().  To make such C library routines thread-safe
 *    would require adding a ThreadLocalStorage struct to the thread control
 *    block TX_THREAD.  Since relatively few applications make use of these
 *    library routines, the implementation provided here uses a single, global
 *    ThreadLocalStorage data structure rather than greatly increasing the size
 *    of the thread control block TX_THREAD.
 *
 *    The ThreadX global variable _tx_thread_current_ptr points to the
 *    current thread's control block TX_THREAD. If a ThreadLocalStorage struct
 *    called tx_tls is placed in TX_THREAD, the function GetThreadLocalStorage
 *    should be modified to return &(_tx_thread_current_ptr->tx_tls).
 */

static ThreadLocalStorage GlobalTLS;

ThreadLocalStorage *GetThreadLocalStorage()
{
    return &GlobalTLS;
}
#endif

/*
 * Use a global ThreadX mutex to implement thread safety within C and C++
 * library routines.
 *
 */
TX_MUTEX __ghLockMutex;

/*
 * Acquire general lock.  Blocks until the lock becomes available.
 * Use tx_mutex_get to implement __ghsLock
 */
void __ghsLock(void)
{
    tx_mutex_get(&__ghLockMutex, TX_WAIT_FOREVER);
}

/*
 * Release general lock
 * Use tx_mutex_put to implement __ghsUnlock
 */
void __ghsUnlock(void)
{
    tx_mutex_put(&__ghLockMutex);
}

/* ThreadX Initialization function prototype.  */
void _tx_initialize_kernel_setup(void);

void __gh_lock_init(void)
{
    /* Initialize the low-level portions of ThreadX. */
    _tx_initialize_kernel_setup();

    /* Create the global thread lock mutex.  */
    tx_mutex_create(&__ghLockMutex, "__ghLockMutex", TX_NO_INHERIT);
}

/*
  Saving State Across setjmp() Calls
  ==================================

  These routines can be used to save and restore arbitrary state
  across calls to setjmp() and longjmp().
*/
int __ghs_SaveSignalContext(jmp_buf jmpbuf)
{
    return 0;
}

/* Restore arbitrary state across a longjmp() */
void __ghs_RestoreSignalContext(jmp_buf jmpbuf)
{
}

#if defined(__GHS_VERSION_NUMBER) && (__GHS_VERSION_NUMBER < 560)
/*
  C++ Exception Handling
  ======================

  These routines allow C++ exceptions to be used in multiple threads.
  The default implementation uses __ghs_GetThreadLocalStorageItem
  to return a thread-specific __eh_globals pointer.

*/

/* Must be called after __cpp_exception_init() is called to allocate
 * and initialize the per-thread exception handling structure */
void *__get_eh_globals(void)
{
#if defined(__ghs) && (__GHS_VERSION_NUMBER >= 500)
    return *(void **)__ghs_GetThreadLocalStorageItem(__ghs_TLS___eh_globals);
#else
    if (_tx_thread_current_ptr)

        /* Return thread-specific __eh_globals pointer.  */
        return _tx_thread_current_ptr->tx_thread_eh_globals;
    else
        /* Return the global __eh_globals pointer.  */
        return GlobalTLS.__eh_globals;
#endif
}
#endif

#if defined(__ghs) && (__GHS_VERSION_NUMBER >= 500)
#pragma weak __cpp_exception_init
extern void __cpp_exception_init(void **);
#pragma weak __cpp_exception_cleanup
extern void __cpp_exception_cleanup(void **);

/* __tx_cpp_exception_init retrieves the eh_globals field from
   thread-local storage and calls __cpp_exception_init.
 */
void __tx_cpp_exception_init(TX_THREAD *thread_ptr) {
    void **peh_globals;
    if(__cpp_exception_init) {
	if (thread_ptr)
	    peh_globals = &(thread_ptr->tx_thread_eh_globals);
	else
	    /* Use the global __eh_globals pointer.  */
	    peh_globals = &__eh_globals;
	__cpp_exception_init(peh_globals);
    }
}

/* __tx_cpp_exception_cleanup retrieves the eh_globals field from
   thread-local storage and calls __cpp_exception_cleanup.
 */
void __tx_cpp_exception_cleanup(TX_THREAD *thread_ptr) {
    void **peh_globals;
    if(__cpp_exception_cleanup) {
	if (thread_ptr)
	    peh_globals = &(thread_ptr->tx_thread_eh_globals);
	else
	    /* Use the global __eh_globals pointer.  */
	    peh_globals = &__eh_globals;
	__cpp_exception_cleanup(peh_globals);
    }
}

/* __ghs_cpp_exception_init is called from ind_crt1.o to initialize
   exceptions for the global context.
 */
void __ghs_cpp_exception_init() {
    __tx_cpp_exception_init((void *)0);
}

/* __ghs_cpp_exception_cleanup is called from ind_exit.o to clean up
   exceptions for the global context.
 */
void __ghs_cpp_exception_cleanup(TX_THREAD *thread_ptr) {
    __tx_cpp_exception_cleanup((void *)0);
}
#endif


/*
  File Locks
  ======================

  These routines can be customized to implement per-file locks to allow
  thread-safe I/O.

*/

/* Acquire lock for FILE *addr */
void __ghs_flock_file(void *addr)
{
    tx_mutex_get((TX_MUTEX *)addr, TX_WAIT_FOREVER);
}

/* Release lock for FILE *addr */
void __ghs_funlock_file(void *addr)
{
    tx_mutex_put((TX_MUTEX *)addr);
}

/* Non blocking acquire lock for FILE *addr.  May return -1 if */
/* not implemented. Returns 0 on success and nonzero otherwise. */
int __ghs_ftrylock_file(void *addr)
{
    return -1;
}

/* Calls to initialize local lock data structures before they */
/* are used. */
void __ghs_flock_create(void **addr)
{
    *addr = (void *)(&__ghLockMutex);
}
void __ghs_flock_destroy(void *addr) {}


/*
 * ThreadX Peak Stack Checking support routines.
 *
 * All of these routines are called by MULTI's ThreadX-aware debugging
 * package to determine the peak stack use for one thread or for all threads.
 *
 * These routines are included in this file in order to guarantee that they will
 * be available while debugging with MULTI.  These routines are not referenced by
 * any other part of the ThreadX system.
 *
 * _txs_thread_stack_check:         return the peak stack usage for a thread.
 *
 * _txs_thread_stack_check_2:       store the peak stack usage for all threads
 *                                  in the tx_thread_stack_size field of each thread
 *                                  control block, TX_THREAD.  This routine takes
 *                                  advantage of the redundancy within the TX_THREAD
 *                                  structure since tx_thread_stack_size can be computed
 *                                  from the tx_thread_stack_start and tx_thread_stack_end
 *                                  fields of TX_THREAD.
 *
 * _txs_thread_stack_check_2_fixup: clean up from the _txs_thread_stack_check_2
 *                                  call by computing the stack size for each
 *                                  thread and storing the result in the
 *                                  tx_thread_stack_size field of each thread control
 *                                  block TX_THREAD.
 *
 * These three routines do not support architectures such as i960 or StarCore
 * where the stack grows up instead of down.
 *
 */
#ifndef TX_DISABLE_STACK_CHECKING

ULONG _txs_thread_stack_check(TX_THREAD *thread_ptr)
{
    CHAR      *cp;                  /* Pointer inside thread's stack.  */

    /* Search through the thread's stack to find the highest address modified.  */
    for ( cp = (CHAR *)thread_ptr->tx_thread_stack_start;
        cp <= (CHAR *)thread_ptr->tx_thread_stack_end; ++cp ) {

        /* Check if this byte in the stack contains something other than TX_STACK_FILL.  */
        if (*cp != (char)TX_STACK_FILL) {

            /* Assume cp points to the locating marking the peak stack use.
               Return the number of bytes from cp up to and including the
               end of the stack.  */
            return (((ULONG)thread_ptr->tx_thread_stack_end) - (ULONG)cp + 1);
        }
    }
    return thread_ptr->tx_thread_stack_size;
}


int _txs_thread_stack_check_2(void) {
    CHAR *      cp;                  /* Pointer inside thread's stack.  */
    TX_THREAD * tp;                  /* Pointer to each thread.  */

    /* If no threads are created, return immediately.  */
    if (!_tx_thread_created_count)
        return 0;

    /* Start iterating through the threads in the system.  Assume that we always
       have at least one thread (the system timer thread) in the system.  */
    tp = _tx_thread_created_ptr;

    do {

        /* Search through the thread's stack to find the highest address modified.  */
        for ( cp = (CHAR *)tp->tx_thread_stack_start; cp <= (CHAR *)tp->tx_thread_stack_end;
        ++cp ) {

            /* Check if this byte in the stack contains something other than TX_STACK_FILL.  */
            if (*cp != (char)TX_STACK_FILL) {

                /* Assume cp points to the locating marking the peak stack use.
                   Store the number of bytes from cp up to and including the
                   end of the stack in the tx_thread_stack_size field.  */
                tp->tx_thread_stack_size = ((ULONG)tp->tx_thread_stack_end) - (ULONG)cp + 1;
                break;
            }

        }

        /* Continue with the next thread.  */
        tp = tp->tx_thread_created_next;

    /* Loop until we point to the first thread again.  */
    } while ( tp != _tx_thread_created_ptr );

    return 0;
}

int _txs_thread_stack_check_2_fixup(void) {
    TX_THREAD * tp;                  /* Pointer to each thread.  */

    /* If no threads are created, return immediately.  */
    if (!_tx_thread_created_count)
        return 0;

    /* Start iterating through the threads in the system.  Assume that we always
       have at least one thread (the system timer thread) in the system.  */
    tp = _tx_thread_created_ptr;

    do {

        /* Compute the tx_thread_stack_size field by using the tx_thread_stack_end and
           tx_thread_stack_start fields.  */
        tp->tx_thread_stack_size = (ULONG)tp->tx_thread_stack_end-(ULONG)tp->tx_thread_stack_start+1;

        /* Continue with the next thread.  */
        tp = tp->tx_thread_created_next;

    /* Loop until we point to the first thread again.  */
    } while ( tp != _tx_thread_created_ptr );

    return 0;
}

#endif /* TX_DISABLE_STACK_CHECKING  */
