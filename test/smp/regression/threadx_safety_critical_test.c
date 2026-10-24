/***************************************************************************/
/* Copyright (c) 2024 Microsoft Corporation                                */
/* Copyright (c) 2026 Eclipse ThreadX contributors                         */
/*                                                                         */
/* This program and the accompanying materials are made available under    */
/* the terms of the MIT License which is available at                      */
/* https://opensource.org/licenses/MIT.                                    */
/*                                                                         */
/* SPDX-License-Identifier: MIT                                            */
/***************************************************************************/

/* This test is designed to test safety-critical build verification and
   exception handling in ThreadX SMP.  */

#include   <stdio.h>
#include   <string.h>

#ifndef TX_SAFETY_CRITICAL_BUILD_CHECK
#define TX_SAFETY_CRITICAL_BUILD_CHECK
#endif

#ifndef TX_SAFETY_CRITICAL_EXCEPTION_ENABLE
#define TX_SAFETY_CRITICAL_EXCEPTION_ENABLE
#endif

#include   "tx_api.h"

static unsigned long   thread_0_counter =  0;
static TX_THREAD       thread_0;

static unsigned long   exception_handler_invoked = 0;
static CHAR            *last_exception_file = TX_NULL;
static INT             last_exception_line = 0;
static UINT            last_exception_status = 0;

/* Define custom exception handler for test verification.  */
VOID  _tx_safety_critical_exception_handler(CHAR *file_name, INT line_number, UINT status)
{
    exception_handler_invoked++;
    last_exception_file = file_name;
    last_exception_line = line_number;
    last_exception_status = status;
}

/* Prototype for test control return.  */
void  test_control_return(UINT status);

/* Define thread prototypes.  */
static void    thread_0_entry(ULONG thread_input);

/* Define what the initial system looks like.  */
#ifdef CTEST
void test_application_define(void *first_unused_memory)
#else
void threadx_safety_critical_application_define(void *first_unused_memory)
#endif
{
UINT    status;
CHAR    *pointer;

    /* Put first available memory address into a character pointer.  */
    pointer =  (CHAR *) first_unused_memory;

    /* Create test thread.  */
    status =  tx_thread_create(&thread_0, "thread 0", thread_0_entry, 1,
            pointer, TEST_STACK_SIZE_PRINTF,
            16, 16, 3, TX_AUTO_START);

    /* Check for status.  */
    if (status != TX_SUCCESS)
    {
        printf("Running Safety Critical Test........................................ ERROR #1\n");
        test_control_return(1);
    }
}

/* Define the test thread.  */
static void    thread_0_entry(ULONG thread_input)
{
TX_PARAMETER_NOT_USED(thread_input);

    /* Inform user.  */
    printf("Running Safety Critical Test........................................ ");

    /* Increment thread 0 counter.  */
    thread_0_counter++;

    /* Verify build-time configurations are satisfied.  */
#ifndef TX_MAX_PRIORITIES
    printf("ERROR #2\n");
    test_control_return(1);
    return;
#else
    if ((TX_MAX_PRIORITIES % 32) != 0)
    {
        printf("ERROR #3\n");
        test_control_return(1);
        return;
    }
#endif

    /* Sleep for 1 tick to verify scheduling operates normally.  */
    tx_thread_sleep(1);

    /* Verify that no exception has occurred so far.  */
    if (exception_handler_invoked != 0)
    {
        printf("ERROR #4\n");
        test_control_return(1);
        return;
    }

    /* Test runtime safety-critical exception trap dispatch.  */
    TX_SAFETY_CRITICAL_EXCEPTION(__FILE__, __LINE__, 0x5A5A);

    /* Verify that the exception handler was called exactly once.  */
    if (exception_handler_invoked != 1)
    {
        printf("ERROR #5\n");
        test_control_return(1);
        return;
    }

    /* Verify the recorded status value.  */
    if (last_exception_status != 0x5A5A)
    {
        printf("ERROR #6\n");
        test_control_return(1);
        return;
    }

    /* Verify that file name and line number were passed correctly.  */
    if (last_exception_file == TX_NULL || last_exception_line == 0)
    {
        printf("ERROR #7\n");
        test_control_return(1);
        return;
    }

    /* Successful test completion.  */
    printf("SUCCESS!\n");
    test_control_return(0);
}
