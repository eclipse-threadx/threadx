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
/*  Xtensa overlay manager OS hooks for ThreadX. XEA2 only.               */
/*                                                                        */
/*  RELEASE HISTORY                                                       */
/*                                                                        */
/*    DATE              NAME                      DESCRIPTION             */
/*                                                                        */
/*  12-31-2020     Cadence Design Systems   Initial Version 6.1.3         */
/*                                                                        */
/**************************************************************************/


#ifdef XT_USE_OVLY

#include <xtensa/overlay.h>
#include "tx_api.h"


/* Required to work around a bug in the overlay header. */
#ifdef XT_DISABLE_OVERLAYS
#undef  xt_overlay_fatal_error
#define xt_overlay_fatal_error(id)    
#endif


/* Mutex object that controls access to the overlay. Currently only one
 * overlay region is supported so one mutex suffices.
 */
static TX_MUTEX xt_overlay_mutex;


/**************************************************************************/
/*  This function should be overridden to provide OS specific init such   */
/*  as the creation of a mutex lock that can be used for overlay locking. */
/*  Typically this mutex would be set up with priority inheritance. See   */
/* overlay manager documentation for more details.                        */
/**************************************************************************/
void
xt_overlay_init_os(void)
{
    /* Create the mutex for overlay access. Priority inheritance is
     * required.
     */
    UINT status = 
        tx_mutex_create (&xt_overlay_mutex, "xt_overlay_lock", TX_INHERIT);

    if (status != TX_SUCCESS) {
        xt_overlay_fatal_error (-1);
    }
}


/**************************************************************************/
/* This function locks access to shared overlay resources, typically      */
/* by acquiring a mutex.                                                  */
/**************************************************************************/
void
xt_overlay_lock(void)
{
    UINT status = tx_mutex_get (&xt_overlay_mutex, TX_WAIT_FOREVER);

    if (status != TX_SUCCESS) {
        xt_overlay_fatal_error (-1);
    }
}


/**************************************************************************/
/* This function releases access to shared overlay resources, typically   */
/* by unlocking a mutex.                                                  */
/**************************************************************************/
void
xt_overlay_unlock(void)
{
    UINT status = tx_mutex_put (&xt_overlay_mutex);

    if (status != TX_SUCCESS) {
        xt_overlay_fatal_error (-1);
    }
}

#endif /* XT_USE_OVLY */

