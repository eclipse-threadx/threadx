/***************************************************************************
 * Copyright (c) 2024 Microsoft Corporation 
 * 
 * This program and the accompanying materials are made available under the
 * terms of the MIT License which is available at
 * https://opensource.org/licenses/MIT.
 * 
 * SPDX-License-Identifier: MIT
 **************************************************************************/

#ifndef TX_USER_H
#define TX_USER_H

/*  Define user-specified additional fields to be stored in TX_THREAD */
#define TX_THREAD_USER_EXTENSION       UCHAR resume_recorded; \
                                       ULONG resume_recorded_at; \
                                       UCHAR suspend_recorded; \
                                       ULONG suspend_recorded_at; 

/* This is one way to define the extension. Because it's a macro, it
   cannot handle static allocations internally, nor does it have a 
   proper calling context for tracing purposes. */
#define TX_THREAD_SYSTEM_SUSPEND_EXTENSION(thread_ptr) \
do { \
    if (!thread_ptr->suspend_recorded) \
    { \
        thread_ptr->suspend_recorded_at += 1; \
        thread_ptr->suspend_recorded = 1; \
    } \
} while(0)

#endif

