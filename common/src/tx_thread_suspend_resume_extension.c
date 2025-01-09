/***************************************************************************
 * Copyright (c) 2025 Microsoft Corporation 
 * 
 * This program and the accompanying materials are made available under the
 * terms of the MIT License which is available at
 * https://opensource.org/licenses/MIT.
 * 
 * SPDX-License-Identifier: MIT
 **************************************************************************/

/* Include necessary system files.  */

#include "tx_api.h"
#include "tx_trace.h"
#include "tx_thread.h"
#include "tx_initialize.h"

/* Test only, demo of user extension. If the below is not defined,
    no need to include this function at all - it will never be called */
#ifdef TX_ENABLE_THREAD_SYSTEM_RESUME_EXTENSION

UINT  _tx_thread_system_resume_extension(TX_THREAD *thread_ptr)
{
    UINT status = 0;
    if (!thread_ptr->resume_recorded)
    {
        thread_ptr->resume_recorded_at += 1;
    }

    /* Return completion status. */
    return(status);
}

#endif //TX_ENABLE_THREAD_SYSTEM_RESUME_EXTENSION
