/***************************************************************************
 * Copyright (c) 2024 Microsoft Corporation 
 * Copyright (c) 2026 Eclipse ThreadX contributors
 * 
 * This program and the accompanying materials are made available under the
 * terms of the MIT License which is available at
 * https://opensource.org/licenses/MIT.
 * 
 * SPDX-License-Identifier: MIT
 **************************************************************************/
// Portions of this file were generated with AI assistance.

/* This test is designed to test a simple sleep for 100 ticks.  */

#include   <stdio.h>
#include   "tx_api.h"

/* Define the ISR dispatch.  */

extern VOID    (*test_isr_dispatch)(void);



static TX_THREAD       thread_0;

static int             error = 0;

static int             isr_count =  0;


/* Define thread prototypes.  */

static void    thread_0_entry(ULONG thread_input);


/* Prototype for test control return.  */

void  test_control_return(UINT status);



/* Define the ISR dispatch routine.  */

static void    test_isr(void)
{

UINT    status;

    /* Increment the ISR count.  */
    isr_count++;
    
    /* Call sleep from ISR to check for error!  */
    status =  tx_thread_sleep(100);

    /* Check status.  */
    if (status != TX_CALLER_ERROR)
    {


        error =  1;
    }
    
    /* End the ISR.  */
    test_isr_dispatch =  TX_NULL;
}


/* Define what the initial system looks like.  */

#ifdef CTEST
void test_application_define(void *first_unused_memory)
#else
void    threadx_thread_sleep_for_100ticks_application_define(void *first_unused_memory)
#endif
{

UINT    status;
CHAR    *pointer;

    /* Put first available memory address into a character pointer.  */
    pointer =  (CHAR *) first_unused_memory;


    /* Put system definition stuff in here, e.g. thread creates and other assorted
       create information.  */

    status =  tx_thread_create(&thread_0, "thread 0", thread_0_entry, 1,  
            pointer, TEST_STACK_SIZE_PRINTF, 
            16, 16, 3, TX_AUTO_START);

    /* Check for status.  */
    if (status != TX_SUCCESS)
    {

        printf("Running Thread Sleep for 100 Ticks Test............................. ERROR #1\n");
        test_control_return(1);
    }

}



/* Define the test threads.  */

static void    thread_0_entry(ULONG thread_input)
{

UINT    status;

    /* Inform user.  */
    printf("Running Thread Sleep for 100 Ticks Test............................. ");

    /* Call sleep with an expiration of 0 and test error code.  */
    status =  tx_thread_sleep(0);
    
    /* Check error code.  */
    if (status != TX_SUCCESS)
    {
    
        /* Thread Simple Sleep error.  */
        printf("ERROR #3\n");
        test_control_return(1);
    }

    /* Setup the test ISR.  */
    test_isr_dispatch =  test_isr;

    /* Clear the tick count. */
    tx_time_set(0);


    /* Sleep for 100 ticks.  */
    status =  tx_thread_sleep(100);

    /* Determine if the sleep was accurate.  */
    if ((status != TX_SUCCESS) || (tx_time_get() < 100) ||
        (tx_time_get() > 101))
    {

        /* Thread Simple Sleep error.  */
        printf("ERROR #5\n");
        test_control_return(1);
    }

    /* Check to make sure the ISR happened and the proper return value was present.  */
    if ((isr_count == 0) || (error))
    {
    
        /* Thread Simple Sleep error.  */
        printf("ERROR #6\n");
        test_control_return(1);
    }
    else      
    {

        /* Successful Simple Sleep test.  */
        printf("SUCCESS!\n");
        test_control_return(0);
    }
}

