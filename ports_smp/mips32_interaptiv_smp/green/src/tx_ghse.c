/*
 *              ThreadX C++ Library Support
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
#define TX_DISABLE_ERROR_CHECKING
#include "tx_api.h"

/*
  C++ Exception Handling
  ======================

  These routines allow C++ exceptions to be used in multiple threads.
  The default implementation uses __ghs_GetThreadLocalStorageItem
  to return a thread-specific __eh_globals pointer.

*/

#if defined(__ghs) && (__GHS_VERSION_NUMBER >= 560)
#ifdef _WIN32
/* Windows uses a different linker, so include a stub routine, never called,
   to pull in __cpp_exception_init and __cpp_exception_cleanup */
extern void __cpp_exception_init(void **);
extern void __cpp_exception_cleanup(void **);
void __tx_win32_pull_in_exceptions(void) {
    __cpp_exception_init(0);
    __cpp_exception_cleanup(0);
}
#else
#pragma ghs reference __cpp_exception_init
#pragma ghs reference __cpp_exception_cleanup
#endif

/* Must be called after __cpp_exception_init() is called to allocate
 * and initialize the per-thread exception handling structure */
void *__get_eh_globals(void)
{
    return *(void **)__ghs_GetThreadLocalStorageItem(__ghs_TLS___eh_globals);
}
#endif
