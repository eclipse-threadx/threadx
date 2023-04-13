/* This test is designed to test the mutex information services.  */

#include   <stdio.h>
#include   "tx_api.h"
#include   "tx_mutex.h"


static unsigned long   thread_0_counter =  0;
static unsigned long   thread_1_counter =  0;
static TX_THREAD       thread_0;
static TX_THREAD       thread_1;

static TX_MUTEX        mutex_0;
static TX_MUTEX        mutex_1;
static TX_MUTEX        mutex_2;
static TX_MUTEX        mutex_3;
static TX_MUTEX        mutex_4;


/* Define thread prototypes.  */

static void    thread_0_entry(ULONG thread_input);
static void    thread_1_entry(ULONG thread_input);


/* Prototype for test control return.  */

void  test_control_return(UINT status);


UINT  _tx_mutex_performance_info_get(TX_MUTEX *mutex_ptr, ULONG *puts, ULONG *gets,
                    ULONG *suspensions, ULONG *timeouts, ULONG *inversions, ULONG *inheritances);


/* Define what the initial system looks like.  */

#ifdef CTEST
void test_application_define(void *first_unused_memory)
#else
void    threadx_mutex_information_application_define(void *first_unused_memory)
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
            16, 16, TX_NO_TIME_SLICE, TX_AUTO_START);
    pointer = pointer + TEST_STACK_SIZE_PRINTF;


    status =  tx_thread_create(&thread_1, "thread 1", thread_1_entry, 1,  
            pointer, TEST_STACK_SIZE_PRINTF, 
            16, 16, TX_NO_TIME_SLICE, TX_AUTO_START);
    pointer = pointer + TEST_STACK_SIZE_PRINTF;


    /* Check for status.  */
    if (status != TX_SUCCESS)
    {

        printf("Running Mutex Information Test...................................... ERROR #1\n");
        test_control_return(1);
    }

    /* Create a mutex.  */
    status =  tx_mutex_create(&mutex_0, "mutex 0", TX_NO_INHERIT);

    /* Check for status.  */
    if (status != TX_SUCCESS)
    {

        printf("Running Mutex Information Test...................................... ERROR #2\n");
        test_control_return(1);
    }

    /* Create another mutex.  */
    status =  tx_mutex_create(&mutex_1, "mutex 1", TX_NO_INHERIT);

    /* Check for status.  */
    if (status != TX_SUCCESS)
    {

        printf("Running Mutex Information Test...................................... ERROR #3\n");
        test_control_return(1);
    }

    /* Create another mutex.  */
    status =  tx_mutex_create(&mutex_2, "mutex 2", TX_INHERIT);

    /* Check for status.  */
    if (status != TX_SUCCESS)
    {

        printf("Running Mutex Information Test...................................... ERROR #4\n");
        test_control_return(1);
    }

    /* Create another mutex.  */
    status =  tx_mutex_create(&mutex_3, "mutex 3", TX_INHERIT);

    /* Check for status.  */
    if (status != TX_SUCCESS)
    {

        printf("Running Mutex Information Test...................................... ERROR #5\n");
        test_control_return(1);
    }
}



/* Define the test threads.  */

static void    thread_0_entry(ULONG thread_input)
{

UINT        status;
CHAR        *name;
ULONG       count;
TX_THREAD   *owner;
TX_THREAD   *first_suspended;
ULONG       suspended_count;
TX_MUTEX    *next_mutex;
ULONG       puts;
ULONG       gets;
ULONG       suspensions;
ULONG       timeouts;
ULONG       inversions;
ULONG       inheritances;


    /* Inform user.  */
    printf("Running Mutex Information Test...................................... ");

    /* Increment thread 0 counter.  */
    thread_0_counter++;

    /* Attempt to get from mutex that is available.  Should be successful!  */
    status =  tx_mutex_get(&mutex_0, TX_NO_WAIT);

    /* Check status.  */
    if (status != TX_SUCCESS)
    {

        /* Mutex error.  */
        printf("ERROR #6\n");
        test_control_return(1);
    }

    /* Attempt to get the same mutex again.  Should be successful!  */
    status =  tx_mutex_get(&mutex_0, TX_NO_WAIT);

    /* Check status.  */
    if (status != TX_SUCCESS)
    {

        /* Mutex error.  */
        printf("ERROR #7\n");
        test_control_return(1);
    }

    /* Put the mutex. */
    status =  tx_mutex_put(&mutex_0);

    /* Check status.  */
    if ((status != TX_SUCCESS) || (mutex_0.tx_mutex_ownership_count != 1))
    {

        /* Mutex error.  */
        printf("ERROR #8\n");
        test_control_return(1);
    }

    /* Put the mutex again.  Should be successful! */
    status =  tx_mutex_put(&mutex_0);

    /* Check status.  */
    if (status != TX_SUCCESS)
    {

        /* Mutex error.  */
        printf("ERROR #9\n");
        test_control_return(1);
    }

    /* Relinquish to allow other thread to get the mutex.  */
    tx_thread_relinquish();

    /* Attempt to get the mutex.  Should be unsuccessful.  */
    status =  tx_mutex_get(&mutex_1, TX_NO_WAIT);
    
    /* Check status.  */
    if (status != TX_NOT_AVAILABLE)
    {

        /* Mutex error.  */
        printf("ERROR #10\n");
        test_control_return(1);
    }

    /* Relinquish again so that the other thread can release it.  */
    tx_thread_relinquish();

    /* Delete mutex.  */
    status =  tx_mutex_delete(&mutex_0);

    /* Check status.  */
    if (status != TX_SUCCESS)
    {

        /* Mutex error.  */
        printf("ERROR #11\n");
        test_control_return(1);
    }

    status =  tx_mutex_delete(&mutex_1);
    
    /* Check status.  */
    if (status != TX_SUCCESS)
    {

        /* Mutex error.  */
        printf("ERROR #12\n");
        test_control_return(1);
    }

    /* Attempt to get a priority inheritance mutex.  */
    status =  tx_mutex_get(&mutex_2, TX_NO_WAIT);

    /* Check status.  */
    if (status != TX_SUCCESS)
    {

        /* Mutex error.  */
        printf("ERROR #13\n");
        test_control_return(1);
    }

    /* Attempt to get another priority inheritance mutex.  */
    status =  tx_mutex_get(&mutex_3, TX_NO_WAIT);

    /* Check status.  */
    if (status != TX_SUCCESS)
    {

        /* Mutex error.  */
        printf("ERROR #14\n");
        test_control_return(1);
    }

#ifndef TX_DISABLE_ERROR_CHECKING

    /* Attempt to get mutex info with a NULL pointer.  */
    status =  tx_mutex_info_get(TX_NULL, TX_NULL, TX_NULL, TX_NULL, TX_NULL, TX_NULL, TX_NULL);

    /* Check status.  */
    if (status != TX_MUTEX_ERROR)
    {

        /* Mutex error.  */
        printf("ERROR #15\n");
        test_control_return(1);
    }

    /* Attempt to get mutex info from a non-created mutex.  */
    mutex_4.tx_mutex_id =  0;
    status =  tx_mutex_info_get(&mutex_4, TX_NULL, TX_NULL, TX_NULL, TX_NULL, TX_NULL, TX_NULL);

    /* Check status.  */
    if (status != TX_MUTEX_ERROR)
    {

        /* Mutex error.  */
        printf("ERROR #16\n");
        test_control_return(1);
    }
#endif

    /* Get mutex information.  */
    status =  tx_mutex_info_get(&mutex_2, TX_NULL, TX_NULL, TX_NULL, TX_NULL, TX_NULL, TX_NULL);
    status += tx_mutex_info_get(&mutex_2, &name, &count, &owner, &first_suspended, &suspended_count, &next_mutex);

    /* Check status.  */
    if ((status != TX_SUCCESS) || (count != mutex_2.tx_mutex_ownership_count) || (owner != mutex_2.tx_mutex_owner) || 
        (first_suspended != mutex_2.tx_mutex_suspension_list) || (suspended_count != mutex_2.tx_mutex_suspended_count) || (next_mutex != mutex_2.tx_mutex_created_next))
    {

        /* Mutex error.  */
        printf("ERROR #17\n");
        test_control_return(1);
    }

#ifdef TX_MUTEX_ENABLE_PERFORMANCE_INFO

    /* Call with NULL pointer.   */
    status =  _tx_mutex_performance_info_get(TX_NULL, TX_NULL, TX_NULL, TX_NULL, TX_NULL, TX_NULL, TX_NULL);

    /* Check status.  */
    if (status != TX_PTR_ERROR)
    {

        /* Mutex error.  */
        printf("ERROR #18\n");
        test_control_return(1);
    }

    /* Now get the performance inforamtion.  */
    status =  tx_mutex_performance_info_get(&mutex_2, &puts, &gets, &suspensions, &timeouts, &inversions, &inheritances);
    
    /* Check status.  */
    if ((status != TX_SUCCESS) || (puts != mutex_2.tx_mutex_performance_put_count) || (gets != mutex_2.tx_mutex_performance_get_count) ||
        (suspensions != mutex_2.tx_mutex_performance_suspension_count) || (timeouts != mutex_2.tx_mutex_performance_timeout_count) ||
        (inversions != mutex_2.tx_mutex_performance_priority_inversion_count) || (inheritances != mutex_2.tx_mutex_performance__priority_inheritance_count))
    {

        /* Mutex error.  */
        printf("ERROR #19\n");
        test_control_return(1);
    }
    
    /* Now get the system performance inforamtion.  */
    status =  tx_mutex_performance_system_info_get(&puts, &gets, &suspensions, &timeouts, &inversions, &inheritances);
    
    /* Check status.  */
    if ((status != TX_SUCCESS) || (puts != _tx_mutex_performance_put_count) || (gets != _tx_mutex_performance_get_count) ||
        (suspensions != _tx_mutex_performance_suspension_count) || (timeouts != _tx_mutex_performance_timeout_count) ||
        (inversions != _tx_mutex_performance_priority_inversion_count) || (inheritances != _tx_mutex_performance__priority_inheritance_count))
    {

        /* Mutex error.  */
        printf("ERROR #20\n");
        test_control_return(1);
    }

#else

    /* Now get the performance inforamtion.  */
    status =  tx_mutex_performance_info_get(&mutex_2, &puts, &gets, &suspensions, &timeouts, &inversions, &inheritances);

    /* Check status.  */
    if (status != TX_FEATURE_NOT_ENABLED)
    {

        /* Mutex error.  */
        printf("ERROR #21\n");
        test_control_return(1);
    }

    /* Now get the performance inforamtion.  */
    status =  tx_mutex_performance_info_get(TX_NULL, &puts, &gets, &suspensions, &timeouts, &inversions, &inheritances);

    /* Check status.  */
    if (status != TX_FEATURE_NOT_ENABLED)
    {

        /* Mutex error.  */
        printf("ERROR #22\n");
        test_control_return(1);
    }

    /* Now get the performance inforamtion.  */
    status =  tx_mutex_performance_info_get(TX_NULL, TX_NULL, &gets, &suspensions, &timeouts, &inversions, &inheritances);

    /* Check status.  */
    if (status != TX_FEATURE_NOT_ENABLED)
    {

        /* Mutex error.  */
        printf("ERROR #23\n");
        test_control_return(1);
    }

    /* Now get the performance inforamtion.  */
    status =  tx_mutex_performance_info_get(TX_NULL, TX_NULL, TX_NULL, &suspensions, &timeouts, &inversions, &inheritances);

    /* Check status.  */
    if (status != TX_FEATURE_NOT_ENABLED)
    {

        /* Mutex error.  */
        printf("ERROR #24\n");
        test_control_return(1);
    }

    /* Now get the performance inforamtion.  */
    status =  tx_mutex_performance_info_get(TX_NULL, TX_NULL, TX_NULL, TX_NULL, &timeouts, &inversions, &inheritances);

    /* Check status.  */
    if (status != TX_FEATURE_NOT_ENABLED)
    {

        /* Mutex error.  */
        printf("ERROR #25\n");
        test_control_return(1);
    }

    /* Now get the performance inforamtion.  */
    status =  tx_mutex_performance_info_get(TX_NULL, TX_NULL, TX_NULL, TX_NULL, TX_NULL, &inversions, &inheritances);

    /* Check status.  */
    if (status != TX_FEATURE_NOT_ENABLED)
    {

        /* Mutex error.  */
        printf("ERROR #26\n");
        test_control_return(1);
    }

    /* Now get the performance inforamtion.  */
    status =  tx_mutex_performance_info_get(TX_NULL, TX_NULL, TX_NULL, TX_NULL, TX_NULL, TX_NULL, &inheritances);

    /* Check status.  */
    if (status != TX_FEATURE_NOT_ENABLED)
    {

        /* Mutex error.  */
        printf("ERROR #27\n");
        test_control_return(1);
    }

    /* Now get the performance inforamtion.  */
    status =  tx_mutex_performance_info_get(TX_NULL, TX_NULL, TX_NULL, TX_NULL, TX_NULL, TX_NULL, TX_NULL);

    /* Check status.  */
    if (status != TX_FEATURE_NOT_ENABLED)
    {

        /* Mutex error.  */
        printf("ERROR #28\n");
        test_control_return(1);
    }

    /* Now get the system performance inforamtion.  */
    status =  tx_mutex_performance_system_info_get(&puts, &gets, &suspensions, &timeouts, &inversions, &inheritances);

    /* Check status.  */
    if (status != TX_FEATURE_NOT_ENABLED)
    {

        /* Mutex error.  */
        printf("ERROR #29\n");
        test_control_return(1);
    }

    /* Now get the system performance inforamtion.  */
    status =  tx_mutex_performance_system_info_get(TX_NULL, &gets, &suspensions, &timeouts, &inversions, &inheritances);

    /* Check status.  */
    if (status != TX_FEATURE_NOT_ENABLED)
    {

        /* Mutex error.  */
        printf("ERROR #30\n");
        test_control_return(1);
    }

    /* Now get the system performance inforamtion.  */
    status =  tx_mutex_performance_system_info_get(TX_NULL, TX_NULL, &suspensions, &timeouts, &inversions, &inheritances);

    /* Check status.  */
    if (status != TX_FEATURE_NOT_ENABLED)
    {

        /* Mutex error.  */
        printf("ERROR #31\n");
        test_control_return(1);
    }

    /* Now get the system performance inforamtion.  */
    status =  tx_mutex_performance_system_info_get(TX_NULL, TX_NULL, TX_NULL, &timeouts, &inversions, &inheritances);

    /* Check status.  */
    if (status != TX_FEATURE_NOT_ENABLED)
    {

        /* Mutex error.  */
        printf("ERROR #32\n");
        test_control_return(1);
    }

    /* Now get the system performance inforamtion.  */
    status =  tx_mutex_performance_system_info_get(TX_NULL, TX_NULL, TX_NULL, TX_NULL, &inversions, &inheritances);

    /* Check status.  */
    if (status != TX_FEATURE_NOT_ENABLED)
    {

        /* Mutex error.  */
        printf("ERROR #33\n");
        test_control_return(1);
    }

    /* Now get the system performance inforamtion.  */
    status =  tx_mutex_performance_system_info_get(TX_NULL, TX_NULL, TX_NULL, TX_NULL, TX_NULL, &inheritances);

    /* Check status.  */
    if (status != TX_FEATURE_NOT_ENABLED)
    {

        /* Mutex error.  */
        printf("ERROR #34\n");
        test_control_return(1);
    }

    /* Now get the system performance inforamtion.  */
    status =  tx_mutex_performance_system_info_get(TX_NULL, TX_NULL, TX_NULL, TX_NULL, TX_NULL, TX_NULL);

    /* Check status.  */
    if (status != TX_FEATURE_NOT_ENABLED)
    {

        /* Mutex error.  */
        printf("ERROR #35\n");
        test_control_return(1);
    }

#endif

    /* Delete mutex.  */
    status =  tx_mutex_delete(&mutex_2);

    /* Check status.  */
    if (status != TX_SUCCESS)
    {

        /* Mutex error.  */
        printf("ERROR #36\n");
        test_control_return(1);
    }

    status =  tx_mutex_delete(&mutex_3);
    
    /* Check status.  */
    if (status != TX_SUCCESS)
    {

        /* Mutex error.  */
        printf("ERROR #37\n");
        test_control_return(1);
    }
    else
    {

        /* Successful test.  */
        printf("SUCCESS!\n");
        test_control_return(0);
    }
}

static void    thread_1_entry(ULONG thread_input)
{

UINT    status;

    /* Increment thread 1 counter.  */
    thread_1_counter++;

    /* Attempt to get from mutex that is available.  Should be successful!  */
    status =  tx_mutex_get(&mutex_1, TX_NO_WAIT);

    /* Check status.  */
    if (status != TX_SUCCESS)
    {

        /* Mutex error.  */
        printf("ERROR #38\n");
        test_control_return(1);
    }

    /* Let other thread run again.  */
    tx_thread_relinquish();

    /* Release mutex!  */
    status =  tx_mutex_put(&mutex_1);

    /* Check status.  */
    if (status != TX_SUCCESS)
    {

        /* Mutex error.  */
        printf("ERROR #39\n");
        test_control_return(1);
    }
}
