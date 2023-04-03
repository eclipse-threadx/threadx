/* This test is designed to test the semaphore information services.  */

#include   <stdio.h>
#include   "tx_api.h"
#include   "tx_semaphore.h"


static unsigned long   thread_0_counter =  0;
static TX_THREAD       thread_0;

static TX_SEMAPHORE    semaphore_0;
static TX_SEMAPHORE    semaphore_1;
static TX_SEMAPHORE    semaphore_2;


/* Define thread prototypes.  */

static void    thread_0_entry(ULONG thread_input);


UINT        _tx_semaphore_performance_info_get(TX_SEMAPHORE *semaphore_ptr, ULONG *puts, ULONG *gets,
                    ULONG *suspensions, ULONG *timeouts);


/* Prototype for test control return.  */

void  test_control_return(UINT status);


/* Define what the initial system looks like.  */

#ifdef CTEST
void test_application_define(void *first_unused_memory)
#else
void    threadx_semaphore_information_application_define(void *first_unused_memory)
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
            16, 16, 100, TX_AUTO_START);
    pointer = pointer + TEST_STACK_SIZE_PRINTF;

    /* Check for status.  */
    if (status != TX_SUCCESS)
    {

        printf("Running Semaphore Information Test.................................. ERROR #1\n");
        test_control_return(1);
    }

    /* Create a semaphore with an initial count of 1.  */
    status =  tx_semaphore_create(&semaphore_0, "semaphore 0", 1);

    /* Check for status.  */
    if (status != TX_SUCCESS)
    {

        printf("Running Semaphore Information Test.................................. ERROR #2\n");
        test_control_return(1);
    }

    /* Create a semaphore with an initial count of 0.  */
    status =  tx_semaphore_create(&semaphore_1, "semaphore 1", 1);

    /* Check for status.  */
    if (status != TX_SUCCESS)
    {

        printf("Running Semaphore Information Test.................................. ERROR #3\n");
        test_control_return(1);
    }
}



/* Define the test threads.  */

static void    thread_0_entry(ULONG thread_input)
{

UINT            status;
CHAR            *name;
ULONG           current_value;
TX_THREAD       *first_suspended;
ULONG           suspended_count;
TX_SEMAPHORE    *next_semaphore;
ULONG           puts;
ULONG           gets;
ULONG           suspensions;
ULONG           timeouts;


    /* Inform user.  */
    printf("Running Semaphore Information Test.................................. ");

    /* Increment thread 0 counter.  */
    thread_0_counter++;

#ifndef TX_DISABLE_ERROR_CHECKING

    /* Attempt to get semaphore information from a non-semaphore.  */
    status =   tx_semaphore_info_get(TX_NULL, TX_NULL, TX_NULL, TX_NULL, TX_NULL, TX_NULL);

    /* Check status.  */
    if (status != TX_SEMAPHORE_ERROR)
    {

        /* Semaphore error.  */
        printf("ERROR #4\n");
        test_control_return(1);
    }

    /* Attempt to get semaphore information from a non-created semaphore.  */
    semaphore_2.tx_semaphore_id =  0;
    status =   tx_semaphore_info_get(&semaphore_2, TX_NULL, TX_NULL, TX_NULL, TX_NULL, TX_NULL);

    /* Check status.  */
    if (status != TX_SEMAPHORE_ERROR)
    {

        /* Semaphore error.  */
        printf("ERROR #5\n");
        test_control_return(1);
    }
#endif

    /* Attempt to get from semaphore with an instance.  Should be successful! */
    status =  tx_semaphore_get(&semaphore_0, TX_NO_WAIT);

    /* Check status.  */
    if (status != TX_SUCCESS)
    {

        /* Semaphore error.  */
        printf("ERROR #6\n");
        test_control_return(1);
    }

    /* Attempt to get from semaphore without an instance.  Should be unsuccessful.  */
    status =  tx_semaphore_get(&semaphore_0, TX_NO_WAIT);

    /* Check status.  */
    if (status != TX_NO_INSTANCE)
    {

        /* Semaphore error.  */
        printf("ERROR #7\n");
        test_control_return(1);
    }

    /* Put to semaphore that has an instance already.  Should now be 2!  */
    status =  tx_semaphore_put(&semaphore_1);

    /* Check status.  */
    if ((status != TX_SUCCESS) || (semaphore_1.tx_semaphore_count != 2))
    {

        /* Semaphore error.  */
        printf("ERROR #8\n");
        test_control_return(1);
    }


    /* Attempt to get from semaphore with an instance.  Should be successful! */
    status =  tx_semaphore_get(&semaphore_1, TX_NO_WAIT);

    /* Check status.  */
    if (status != TX_SUCCESS)
    {

        /* Semaphore error.  */
        printf("ERROR #9\n");
        test_control_return(1);
    }

    /* Attempt to get from semaphore with an instance.  Should be successful.  */
    status =  tx_semaphore_get(&semaphore_1, TX_NO_WAIT);
    
    /* Check status.  */
    if (status != TX_SUCCESS)
    {

        /* Semaphore error.  */
        printf("ERROR #10\n");
        test_control_return(1);
    }

    /* Get semaphore information.  */
    status =   tx_semaphore_info_get(&semaphore_0, TX_NULL, TX_NULL, TX_NULL, TX_NULL, TX_NULL);
    status +=  tx_semaphore_info_get(&semaphore_0, &name, &current_value, &first_suspended, &suspended_count, &next_semaphore);
    
    /* Check status.  */
    if ((status != TX_SUCCESS) || (current_value != semaphore_0.tx_semaphore_count) || 
        (first_suspended != semaphore_0.tx_semaphore_suspension_list) || (suspended_count != semaphore_0.tx_semaphore_suspended_count) || 
        (next_semaphore != semaphore_0.tx_semaphore_created_next))
    {

        /* Semaphore error.  */
        printf("ERROR #11\n");
        test_control_return(1);
    }

#ifdef TX_SEMAPHORE_ENABLE_PERFORMANCE_INFO

    /* Get semaphore performance information with NULL pointer.  */
    status =  _tx_semaphore_performance_info_get(TX_NULL, TX_NULL, TX_NULL, TX_NULL, TX_NULL);

    /* Check for error.  */
    if (status != TX_PTR_ERROR)
    {

        /* Semaphore error.  */
        printf("ERROR #12\n");
        test_control_return(1);
    }

    /* Get semaphore performance information.  */
    status =  tx_semaphore_performance_info_get(&semaphore_0, TX_NULL, TX_NULL, TX_NULL, TX_NULL);
    status += tx_semaphore_performance_info_get(&semaphore_0, &puts, &gets, &suspensions, &timeouts);
    
    /* Check status.  */
    if ((status != TX_SUCCESS) || (puts != semaphore_0.tx_semaphore_performance_put_count) || (gets != semaphore_0.tx_semaphore_performance_get_count) || 
        (suspensions != semaphore_0.tx_semaphore_performance_suspension_count) || (timeouts != semaphore_0.tx_semaphore_performance_timeout_count))
    {

        /* Semaphore error.  */
        printf("ERROR #13\n");
        test_control_return(1);
    }
    
    /* Get semaphore system performance information.  */
    status =  tx_semaphore_performance_system_info_get(TX_NULL, TX_NULL, TX_NULL, TX_NULL);
    status += tx_semaphore_performance_system_info_get(&puts, &gets, &suspensions, &timeouts);
    
    /* Check status.  */
    if ((status != TX_SUCCESS) || (puts != _tx_semaphore_performance_put_count) || (gets != _tx_semaphore_performance_get_count) || 
        (suspensions != _tx_semaphore_performance_suspension_count) || (timeouts != _tx_semaphore_performance_timeout_count))
    {

        /* Semaphore error.  */
        printf("ERROR #14\n");
        test_control_return(1);
    }

#else

    /* Get semaphore performance information.  */
    status =  tx_semaphore_performance_info_get(&semaphore_0, &puts, &gets, &suspensions, &timeouts);

    /* Check for error.  */
    if (status != TX_FEATURE_NOT_ENABLED)
    {

        /* Semaphore error.  */
        printf("ERROR #15\n");
        test_control_return(1);
    }

    /* Get semaphore performance information.  */
    status =  tx_semaphore_performance_info_get(TX_NULL, &puts, &gets, &suspensions, &timeouts);

    /* Check for error.  */
    if (status != TX_FEATURE_NOT_ENABLED)
    {

        /* Semaphore error.  */
        printf("ERROR #16\n");
        test_control_return(1);
    }

    /* Get semaphore performance information.  */
    status =  tx_semaphore_performance_info_get(TX_NULL, TX_NULL, &gets, &suspensions, &timeouts);

    /* Check for error.  */
    if (status != TX_FEATURE_NOT_ENABLED)
    {

        /* Semaphore error.  */
        printf("ERROR #17\n");
        test_control_return(1);
    }

    /* Get semaphore performance information.  */
    status =  tx_semaphore_performance_info_get(TX_NULL, TX_NULL, TX_NULL, &suspensions, &timeouts);

    /* Check for error.  */
    if (status != TX_FEATURE_NOT_ENABLED)
    {

        /* Semaphore error.  */
        printf("ERROR #18\n");
        test_control_return(1);
    }

    /* Get semaphore performance information.  */
    status =  tx_semaphore_performance_info_get(TX_NULL, TX_NULL, TX_NULL, TX_NULL, &timeouts);

    /* Check for error.  */
    if (status != TX_FEATURE_NOT_ENABLED)
    {

        /* Semaphore error.  */
        printf("ERROR #19\n");
        test_control_return(1);
    }

    /* Get semaphore performance information.  */
    status =  tx_semaphore_performance_info_get(TX_NULL, TX_NULL, TX_NULL, TX_NULL, TX_NULL);

    /* Check for error.  */
    if (status != TX_FEATURE_NOT_ENABLED)
    {

        /* Semaphore error.  */
        printf("ERROR #20\n");
        test_control_return(1);
    }

    /* Get semaphore system performance information.  */
    status =  tx_semaphore_performance_system_info_get(&puts, &gets, &suspensions, &timeouts);

    /* Check for error.  */
    if (status != TX_FEATURE_NOT_ENABLED)
    {

        /* Semaphore error.  */
        printf("ERROR #21\n");
        test_control_return(1);
    }

    /* Get semaphore system performance information.  */
    status =  tx_semaphore_performance_system_info_get(TX_NULL, &gets, &suspensions, &timeouts);

    /* Check for error.  */
    if (status != TX_FEATURE_NOT_ENABLED)
    {

        /* Semaphore error.  */
        printf("ERROR #22\n");
        test_control_return(1);
    }

    /* Get semaphore system performance information.  */
    status =  tx_semaphore_performance_system_info_get(TX_NULL, TX_NULL, &suspensions, &timeouts);

    /* Check for error.  */
    if (status != TX_FEATURE_NOT_ENABLED)
    {

        /* Semaphore error.  */
        printf("ERROR #23\n");
        test_control_return(1);
    }

    /* Get semaphore system performance information.  */
    status =  tx_semaphore_performance_system_info_get(TX_NULL, TX_NULL, TX_NULL, &timeouts);

    /* Check for error.  */
    if (status != TX_FEATURE_NOT_ENABLED)
    {

        /* Semaphore error.  */
        printf("ERROR #24\n");
        test_control_return(1);
    }

    /* Get semaphore system performance information.  */
    status =  tx_semaphore_performance_system_info_get(TX_NULL, TX_NULL, TX_NULL, TX_NULL);

    /* Check for error.  */
    if (status != TX_FEATURE_NOT_ENABLED)
    {

        /* Semaphore error.  */
        printf("ERROR #25\n");
        test_control_return(1);
    }
#endif

    /* Delete semaphores.  */
    status =  tx_semaphore_delete(&semaphore_0);

    /* Check status.  */
    if (status != TX_SUCCESS)
    {

        /* Semaphore error.  */
        printf("ERROR #26\n");
        test_control_return(1);
    }

    status =  tx_semaphore_delete(&semaphore_1);
    /* Check status.  */
    if (status != TX_SUCCESS)
    {

        /* Semaphore error.  */
        printf("ERROR #27\n");
        test_control_return(1);
    }
    else
    {

        /* Successful test.  */
        printf("SUCCESS!\n");
        test_control_return(0);
    }
}
