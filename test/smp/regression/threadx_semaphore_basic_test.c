/* This test is designed to test the semaphore create/delete and immediate  return gets and puts.  */

#include   <stdio.h>
#include   "tx_api.h"


typedef struct SEMAPHORE_MEMORY_TEST_STRUCT
{
    ULONG           first;
    ULONG           second;
    TX_SEMAPHORE    semaphore;
    ULONG           next_to_last;
    ULONG           last;

} SEMAPHORE_MEMORY_TEST;

static  SEMAPHORE_MEMORY_TEST   semaphore_memory;


/* Define the external symbol to obtain the status from a create call in initialization.  */

extern UINT  test_semaphore_from_init;


/* Define the ISR dispatch.  */

extern VOID    (*test_isr_dispatch)(void);


static unsigned long   thread_0_counter =  0;
static TX_THREAD       thread_0;
static TX_THREAD       thread_1;


static TX_TIMER        timer_0;


static unsigned long error =  0;
static unsigned long timer_executed =  0;
static unsigned long isr_executed =  0;


static TX_SEMAPHORE    semaphore_0;
static TX_SEMAPHORE    semaphore_1;
static TX_SEMAPHORE    semaphore_2;
static TX_SEMAPHORE    semaphore_3;


/* Define thread prototypes.  */

static void    thread_0_entry(ULONG thread_input);
static void    thread_1_entry(ULONG thread_input);

UINT        _txe_semaphore_create(TX_SEMAPHORE *semaphore_ptr, CHAR *name_ptr, ULONG initial_count, UINT semaphore_control_block_size);


/* Prototype for test control return.  */

void  test_control_return(UINT status);


/* Define the timer for this test.  */

static void    timer_entry(ULONG i)
{

#ifndef TX_DISABLE_ERROR_CHECKING

UINT    status;


    /* Determine if calling semaphore create from initialization was successful.  */
    if (test_semaphore_from_init != TX_SUCCESS)
    {
    
        /* Error!  */
        error++;
    }

    /* Attempt to create a semaphore from a timer.  */
    status =  tx_semaphore_create(&semaphore_2, "semaphore 2", 1);

    /* Check for status.  */
    if (status != TX_CALLER_ERROR)
    {

        /* Error!  */
        error++;
    }

    /* Attempt to delete a semaphore from a timer.  */
    status =  tx_semaphore_delete(&semaphore_0);

    /* Check for status.  */
    if (status != TX_CALLER_ERROR)
    {

        /* Error!  */
        error++;
    }

    /* Attempt to get a semaphore with suspension from a timer.  */
    status =  tx_semaphore_get(&semaphore_0, 100);

    /* Check for status.  */
    if (status != TX_WAIT_ERROR)
    {

        /* Error!  */
        error++;
    }


    timer_executed =  1;
#endif
}

/* Define the ISR dispatch routine.  */

static void    test_isr(void)
{

#ifndef TX_DISABLE_ERROR_CHECKING

UINT    status;


    /* Attempt to create a semaphore from an ISR.  */
    status =  tx_semaphore_create(&semaphore_2, "semaphore 2", 1);

    /* Check for status.  */
    if (status != TX_CALLER_ERROR)
    {

        /* Error!  */
        error++;
    }

    /* Attempt to delete a semaphore from an ISR.  */
    status =  tx_semaphore_delete(&semaphore_0);

    /* Check for status.  */
    if (status != TX_CALLER_ERROR)
    {

        /* Error!  */
        error++;
    }

    /* Attempt to get a semaphore with suspension from an ISR.  */
    status =  tx_semaphore_get(&semaphore_0, 100);

    /* Check for status.  */
    if (status != TX_WAIT_ERROR)
    {

        /* Error!  */
        error++;
    }

    isr_executed =  1;
#endif
}



/* Define what the initial system looks like.  */

#ifdef CTEST
void test_application_define(void *first_unused_memory)
#else
void    threadx_semaphore_basic_application_define(void *first_unused_memory)
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

    status +=  tx_thread_create(&thread_1, "thread 1", thread_1_entry, 1,  
            pointer, TEST_STACK_SIZE_PRINTF, 
            18, 18, 100, TX_DONT_START);
    pointer = pointer + TEST_STACK_SIZE_PRINTF;

    /* Check for status.  */
    if (status != TX_SUCCESS)
    {

        printf("Running Semaphore Basic Test........................................ ERROR #1\n");
        test_control_return(1);
    }

    /* Create a semaphore with an initial count of 1.  */
    status =  tx_semaphore_create(&semaphore_0, "semaphore 0", 1);

    /* Check for status.  */
    if (status != TX_SUCCESS)
    {

        printf("Running Semaphore Basic Test........................................ ERROR #2\n");
        test_control_return(1);
    }

    /* Create a semaphore with an initial count of 0.  */
    status =  tx_semaphore_create(&semaphore_1, "semaphore 1", 1);

    /* Check for status.  */
    if (status != TX_SUCCESS)
    {

        printf("Running Semaphore Basic Test........................................ ERROR #3\n");
        test_control_return(1);
    }
}



/* Define the test threads.  */

static void    thread_0_entry(ULONG thread_input)
{

UINT    status;

    /* Inform user.  */
    printf("Running Semaphore Basic Test........................................ ");

    /* Perform semaphore memory test.  */
    semaphore_memory.first =        0x11223344;
    semaphore_memory.second =       0x55667788;
    semaphore_memory.next_to_last = 0x99aabbcc;
    semaphore_memory.last =         0xddeeff00;
    
    /* Create the semaphore.  */
    status =  tx_semaphore_create(&semaphore_memory.semaphore, "semaphore memory", 0);
    tx_semaphore_delete(&semaphore_memory.semaphore);
    
    /* Check for status.  */
    if ((status != TX_SUCCESS) ||
        (semaphore_memory.first != 0x11223344) ||
        (semaphore_memory.second != 0x55667788) ||
        (semaphore_memory.next_to_last != 0x99aabbcc) ||
        (semaphore_memory.last != 0xddeeff00))
    {

        /* Semaphore error.  */
        printf("ERROR #4\n");
        test_control_return(1);
    }

    /* Increment thread 0 counter.  */
    thread_0_counter++;

#ifndef TX_DISABLE_ERROR_CHECKING

    /* Attempt to create a non-semaphore.  */
    status =  tx_semaphore_create(TX_NULL, "semaphore 0", 1);

    /* Check for status.  */
    if (status != TX_SEMAPHORE_ERROR)
    {

        /* Semaphore error.  */
        printf("ERROR #5\n");
        test_control_return(1);
    }

    /* Attempt to create a semaphore with a bad sized block.  */
    status =  _txe_semaphore_create(&semaphore_3, "semaphore 3", 1, (sizeof(TX_SEMAPHORE)+1));

    /* Check for status.  */
    if (status != TX_SEMAPHORE_ERROR)
    {

        /* Semaphore error.  */
        printf("ERROR #6\n");
        test_control_return(1);
    }

    /* Attempt to create an already created semaphore.  */
    status =  tx_semaphore_create(&semaphore_0, "semaphore 0", 1);

    /* Check for status.  */
    if (status != TX_SEMAPHORE_ERROR)
    {

        /* Semaphore error.  */
        printf("ERROR #7\n");
        test_control_return(1);
    }

    /* Attempt to delete a non-semaphore.  */
    status =  tx_semaphore_delete(TX_NULL);

    /* Check for status.  */
    if (status != TX_SEMAPHORE_ERROR)
    {

        /* Semaphore error.  */
        printf("ERROR #8\n");
        test_control_return(1);
    }

    /* Attempt to delete a non-created semaphore.  */
    semaphore_2.tx_semaphore_id =  0;
    status =  tx_semaphore_delete(&semaphore_2);

    /* Check for status.  */
    if (status != TX_SEMAPHORE_ERROR)
    {

        /* Semaphore error.  */
        printf("ERROR #9\n");
        test_control_return(1);
    }

    /* Attempt to get a non-semaphore.  */
    status =  tx_semaphore_get(TX_NULL, TX_NO_WAIT);

    /* Check for status.  */
    if (status != TX_SEMAPHORE_ERROR)
    {

        /* Semaphore error.  */
        printf("ERROR #10\n");
        test_control_return(1);
    }

    /* Attempt to get a non-created semaphore.  */
    semaphore_2.tx_semaphore_id =  0;
    status =  tx_semaphore_get(&semaphore_2, TX_NO_WAIT);

    /* Check for status.  */
    if (status != TX_SEMAPHORE_ERROR)
    {

        /* Semaphore error.  */
        printf("ERROR #11\n");
        test_control_return(1);
    }

    /* Attempt to put a non-semaphore.  */
    status =  tx_semaphore_put(TX_NULL);

    /* Check for status.  */
    if (status != TX_SEMAPHORE_ERROR)
    {

        /* Semaphore error.  */
        printf("ERROR #12\n");
        test_control_return(1);
    }

    /* Attempt to put a non-created semaphore.  */
    semaphore_2.tx_semaphore_id =  0;
    status =  tx_semaphore_put(&semaphore_2);

    /* Check for status.  */
    if (status != TX_SEMAPHORE_ERROR)
    {

        /* Semaphore error.  */
        printf("ERROR #13\n");
        test_control_return(1);
    }

    /* Attempt to ceiling put a non-semaphore.  */
    status =  tx_semaphore_ceiling_put(TX_NULL, 0);

    /* Check for status.  */
    if (status != TX_SEMAPHORE_ERROR)
    {

        /* Semaphore error.  */
        printf("ERROR #14\n");
        test_control_return(1);
    }

    /* Attempt to ceiling put a non-created semaphore.  */
    semaphore_2.tx_semaphore_id =  0;
    status =  tx_semaphore_ceiling_put(&semaphore_2, 0);

    /* Check for status.  */
    if (status != TX_SEMAPHORE_ERROR)
    {

        /* Semaphore error.  */
        printf("ERROR #15\n");
        test_control_return(1);
    }

    /* Attempt to ceiling put with an invalid ceiling of 0.  */
    status =  tx_semaphore_ceiling_put(&semaphore_0, 0);

    /* Check for status.  */
    if (status != TX_INVALID_CEILING)
    {

        /* Semaphore error.  */
        printf("ERROR #16\n");
        test_control_return(1);
    }
#endif

    /* Attempt to get from semaphore with an instance.  Should be successful! */
    status =  tx_semaphore_get(&semaphore_0, TX_NO_WAIT);

    /* Check status.  */
    if (status != TX_SUCCESS)
    {

        /* Semaphore error.  */
        printf("ERROR #17\n");
        test_control_return(1);
    }

    /* Attempt to get from semaphore without an instance.  Should be unsuccessful.  */
    status =  tx_semaphore_get(&semaphore_0, TX_NO_WAIT);

    /* Check status.  */
    if (status != TX_NO_INSTANCE)
    {

        /* Semaphore error.  */
        printf("ERROR #18\n");
        test_control_return(1);
    }

    /* Put to semaphore that has an instance already.  Should now be 2!  */
    status =  tx_semaphore_put(&semaphore_1);

    /* Check status.  */
    if ((status != TX_SUCCESS) || (semaphore_1.tx_semaphore_count != 2))
    {

        /* Semaphore error.  */
        printf("ERROR #19\n");
        test_control_return(1);
    }


    /* Attempt to get from semaphore with an instance.  Should be successful! */
    status =  tx_semaphore_get(&semaphore_1, TX_NO_WAIT);

    /* Check status.  */
    if (status != TX_SUCCESS)
    {

        /* Semaphore error.  */
        printf("ERROR #20\n");
        test_control_return(1);
    }

    /* Attempt to get from semaphore with an instance.  Should be successful.  */
    status =  tx_semaphore_get(&semaphore_1, TX_NO_WAIT);
    
    /* Check status.  */
    if (status != TX_SUCCESS)
    {

        /* Semaphore error.  */
        printf("ERROR #21\n");
        test_control_return(1);
    }

#ifndef TX_DISABLE_ERROR_CHECKING

    /* Create a timer for the test.  */
    tx_timer_create(&timer_0, "timer 0", timer_entry, 0, 1, 1, TX_AUTO_ACTIVATE);

    /* Setup the ISR.  */
    test_isr_dispatch =  test_isr;

    /* Sleep for a bit...  */
    tx_thread_sleep(3);

    /* Resume the thread 1 so we can take an interrupt on top of it.  */
    tx_thread_resume(&thread_1);

    /* Sleep for a bit...  */
    tx_thread_sleep(3);

    /* Clear the ISR.  */
    test_isr_dispatch =  TX_NULL;

    /* Test for error.  */
    if ((error) || (timer_executed != 1) || (isr_executed != 1))
    {
    
        /* Semaphore error.  */
        printf("ERROR #22\n");
        test_control_return(1);
    }

#endif

    /* Delete semaphores.  */
    status =  tx_semaphore_delete(&semaphore_0);

    /* Check status.  */
    if (status != TX_SUCCESS)
    {

        /* Semaphore error.  */
        printf("ERROR #23\n");
        test_control_return(1);
    }

    status =  tx_semaphore_delete(&semaphore_1);
    /* Check status.  */
    if (status != TX_SUCCESS)
    {

        /* Semaphore error.  */
        printf("ERROR #24\n");
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

    while(1)
    {
    
        tx_thread_relinquish();
    }
}