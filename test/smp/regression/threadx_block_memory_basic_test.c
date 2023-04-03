/* This test is designed to test simple memory block pool creation, deletion, and 
   allocates and releases.  */

#include   <stdio.h>
#include   "tx_api.h"

typedef struct BLOCK_MEMORY_TEST_STRUCT
{
    ULONG           first;
    ULONG           second;
    TX_BLOCK_POOL   pool;
    ULONG           first_middle;
    ULONG           second_middle;
    ULONG           pool_area[2048/sizeof(ULONG)];
    ULONG           next_to_last;
    ULONG           last;

} BLOCK_MEMORY_TEST;

static  BLOCK_MEMORY_TEST   block_memory;


/* Define external reference for status of block pool create from initialization.  */

extern UINT            test_block_pool_create_init;


/* Define the ISR dispatch.  */

extern VOID    (*test_isr_dispatch)(void);


static TX_THREAD       thread_0;
static TX_THREAD       thread_1;


static TX_BLOCK_POOL   pool_0;
static TX_BLOCK_POOL   pool_1;
static TX_BLOCK_POOL   pool_2;
static TX_BLOCK_POOL   pool_3;

static TX_TIMER        timer_0;


static unsigned long error =  0;
static unsigned long timer_executed =  0;
static unsigned long isr_executed =  0;


/* Define thread prototypes.  */

static void    thread_0_entry(ULONG thread_input);
static void    thread_1_entry(ULONG thread_input);


/* Prototype for test control return.  */

void  test_control_return(UINT status);


/* Prototype direct call to block pool core service.  */

UINT  _tx_block_pool_create(TX_BLOCK_POOL *pool_ptr, CHAR *name_ptr, ULONG block_size,
                    VOID *pool_start, ULONG pool_size);



/* Define the timer for this test.  */

static void    timer_entry(ULONG i)
{

#ifndef TX_DISABLE_ERROR_CHECKING

UINT    status;
CHAR    *pointer;


    /* Determine if calling block pool create from initialization was successful.  */
    if (test_block_pool_create_init != TX_SUCCESS)
    {
    
        /* Error!  */
        error++;
    }

    /* Attempt to create a block pool from a timer.  */
    pointer =  (CHAR *) 0x30000;
    status =  tx_block_pool_create(&pool_3, "pool 3", 100, pointer, 320);
    
        /* Check status.  */
    if (status != TX_CALLER_ERROR)
    {

        /* Error!  */
        error++;
    }

    /* Attempt to delete a block pool.  */
    status =  tx_block_pool_delete(&pool_0);
    
        /* Check status.  */
    if (status != TX_CALLER_ERROR)
    {

        /* Error!  */
        error++;
    }
    
    timer_executed =  1;

    /* Attempt to allocate a block with suspension from a timer.  */
    status =  tx_block_allocate(&pool_0, (void **) &pointer, 10);

    /* Check status.  */
    if (status != TX_WAIT_ERROR)
    {

        /* Error!  */
        error++;
    }

#endif
}

/* Define the ISR dispatch routine.  */

static void    test_isr(void)
{

#ifndef TX_DISABLE_ERROR_CHECKING

CHAR    *pointer;
UINT    status;


    /* Attempt to Allocate block from the pool.  */
    status = tx_block_allocate(&pool_0, (VOID **) &pointer, TX_WAIT_FOREVER);

    /* Check status.  */
    if (status != TX_WAIT_ERROR)
    {

        /* Error!  */
        error++;
    }

    /* Attempt to create a block pool from an ISR.  */
    status =  tx_block_pool_create(&pool_3, "pool 3", 100, (void *) 0x100000, 320);

    /* Check status.  */
    if (status != TX_CALLER_ERROR)
    {

        /* Error!  */
        error++;
    }

    /* Attempt to delete a pool from an ISR.  */
    status =  tx_block_pool_delete(&pool_0);

    if (status != TX_CALLER_ERROR)
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
void    threadx_block_memory_basic_application_define(void *first_unused_memory)
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
            17, 17, 100, TX_AUTO_START);
    pointer = pointer + TEST_STACK_SIZE_PRINTF;

    /* Check status.  */
    if (status != TX_SUCCESS)
    {

        printf("Running Block Memory Basic Functionality Test....................... ERROR #1\n");
        test_control_return(1);
    }

    status =  tx_thread_create(&thread_1, "thread 1", thread_1_entry, 1,  
            pointer, TEST_STACK_SIZE_PRINTF, 
            18, 18, 100, TX_DONT_START);
    pointer = pointer + TEST_STACK_SIZE_PRINTF;

    /* Check status.  */
    if (status != TX_SUCCESS)
    {

        printf("Running Block Memory Basic Functionality Test....................... ERROR #2\n");
        test_control_return(1);
    }

    /* Create block pools 0 and 1.  */
    status =  tx_block_pool_create(&pool_0, "pool 0", 100, pointer, 320);
    pointer = pointer + 320;

    /* Check status.  */
    if (status != TX_SUCCESS)
    {

        printf("Running Block Memory Basic Functionality Test....................... ERROR #3\n");
        test_control_return(1);
    }

    status =  tx_block_pool_create(&pool_1, "pool 1", 100, pointer, 320);
    pointer = pointer + 320;

    /* Check status.  */
    if (status != TX_SUCCESS)
    {

        printf("Running Block Memory Basic Functionality Test....................... ERROR #4\n");
        test_control_return(1);
    }

    /* Check the no-blocks path.  */
    status =  _tx_block_pool_create(&pool_2, "pool 2", 100, pointer, 50);
    pointer = pointer + 320;

    /* Check status.  */
    if (status != TX_SIZE_ERROR)
    {

        printf("Running Block Memory Basic Functionality Test....................... ERROR #5\n");
        test_control_return(1);
    }
}



/* Define the test threads.  */

static void    thread_0_entry(ULONG thread_input)
{

UINT    status;
CHAR    *pointer_1;
CHAR    *pointer_2;
CHAR    *pointer_3;
CHAR    *pointer_4;
INT      i;
unsigned long fake_block[20];


    /* Inform user.  */
    printf("Running Block Memory Basic Functionality Test....................... ");

    /* Perform block memory test.  */
    block_memory.first =        0x11223344;
    block_memory.second =       0x55667788;
    block_memory.first_middle = 0x21314151;
    block_memory.second_middle= 0x61718191;
    block_memory.next_to_last = 0x99aabbcc;
    block_memory.last =         0xddeeff00;
    
    /* Create the block pool.  */
    status =  tx_block_pool_create(&block_memory.pool, "pool memory", 16, &block_memory.pool_area[0], (2048*sizeof(ULONG))/sizeof(ULONG));
    tx_block_pool_delete(&block_memory.pool);

    /* Check for status.  */
    if ((status != TX_SUCCESS) ||
        (block_memory.first != 0x11223344) ||
        (block_memory.second != 0x55667788) ||
        (block_memory.first_middle != 0x21314151) ||
        (block_memory.second_middle != 0x61718191) ||
        (block_memory.next_to_last != 0x99aabbcc) ||
        (block_memory.last != 0xddeeff00))
    {

        /* Block memory error.  */
        printf("ERROR #6\n");
        test_control_return(1);
    }

#ifndef TX_DISABLE_ERROR_CHECKING

    /* Try to release a non-block.  */
    fake_block[0] =  0;
    fake_block[1] =  0;
    status =  tx_block_release(&fake_block[2]);
    
    /* Check status.  */
    if (status != TX_PTR_ERROR)
    {

        /* Block memory error.  */
        printf("ERROR #7\n");
        test_control_return(1);
    }

    /* Try to release a block that points to a non-pool.  */
    fake_block[0] =  0;
    fake_block[1] =  (unsigned long) &fake_block[0];
    status =  tx_block_release(&fake_block[2]);
    
    /* Check status.  */
    if (status != TX_PTR_ERROR)
    {

        /* Block memory error.  */
        printf("ERROR #8\n");
        test_control_return(1);
    }
#endif
   
    /* Allocate first block from the pool.  */
    status = tx_block_allocate(&pool_0, (VOID **) &pointer_1, TX_NO_WAIT);

    /* Check status.  */
    if (status != TX_SUCCESS)
    {

        /* Block memory error.  */
        printf("ERROR #9\n");
        test_control_return(1);
    }

    /* Set all the memory of the blocks.  */
    TX_MEMSET(pointer_1, (CHAR) 0xEF, 100);

    /* Allocate second block from the pool.  */
    status = tx_block_allocate(&pool_0, (VOID **) &pointer_2, TX_NO_WAIT);

    /* Check status.  */
    if (status != TX_SUCCESS)
    {

        /* Block memory error.  */
        printf("ERROR #10\n");
        test_control_return(1);
    }

    /* Set all the memory of the blocks.  */
    TX_MEMSET(pointer_2, (CHAR) 0xEF, 100);

    /* Allocate third block from the pool.  */
    status = tx_block_allocate(&pool_0, (VOID **) &pointer_3, TX_NO_WAIT);

    /* Check status.  */
    if (status != TX_SUCCESS)
    {

        /* Block memory error.  */
        printf("ERROR #11\n");
        test_control_return(1);
    }

    /* Set all the memory of the blocks.  */
    TX_MEMSET(pointer_3, (CHAR) 0xEF, 100);

    /* Attempt to allocate fourth block from the pool.  This should fail because
       there should be no more blocks in the pool.  */
    status = tx_block_allocate(&pool_0, (VOID **) &pointer_4, TX_NO_WAIT);

    /* Check status.  */
    if (status != TX_NO_MEMORY)
    {

        /* Block memory error.  */
        printf("ERROR ##12\n");
        test_control_return(1);
    }

    /* Set the memory of all of the allocated blocks.  */
    for (i =0; i < 100; i++)
    {
        pointer_1[i] = (CHAR) 0xFF;
        pointer_2[i] = (CHAR) 0xFF;
        pointer_3[i] = (CHAR) 0xFF;
    }

    /* Now release each of the blocks. */
    status =  tx_block_release(pointer_1);

    /* Check for status.  */
    if (status != TX_SUCCESS)
    {

        /* Block memory error.  */
        printf("ERROR #13\n");
        test_control_return(1);
    }

    /* Release the second block.  */
    status =  tx_block_release(pointer_2);

    /* Check for status.  */
    if (status != TX_SUCCESS)
    {

        /* Block memory error.  */
        printf("ERROR #14\n");
        test_control_return(1);
    }

    /* Release the third block.  */
    status =  tx_block_release(pointer_3);

    /* Check for status.  */
    if (status != TX_SUCCESS)
    {

        /* Block memory error.  */
        printf("ERROR #15\n");
        test_control_return(1);
    }

    /* Allocate each block again to make sure everything still
       works.  The block addresses should come out in reverse
       order, because a released block is placed at the head of
       the list.  */

    /* Allocate first block from the pool.  */
    status = tx_block_allocate(&pool_0, (VOID **) &pointer_1, TX_NO_WAIT);

    /* Check status.  */
    if (status != TX_SUCCESS)
    {

        /* Block memory error.  */
        printf("ERROR #16\n");
        test_control_return(1);
    }

    /* Set all the memory of the blocks.  */
    TX_MEMSET(pointer_1, (CHAR) 0xEF, 100);

    /* Allocate second block from the pool.  */
    status = tx_block_allocate(&pool_0, (VOID **) &pointer_2, TX_NO_WAIT);

    /* Check status.  */
    if (status != TX_SUCCESS)
    {

        /* Block memory error.  */
        printf("ERROR #17\n");
        test_control_return(1);
    }

    /* Set all the memory of the blocks.  */
    TX_MEMSET(pointer_2, (CHAR) 0xEF, 100);

    /* Allocate third block from the pool.  */
    status = tx_block_allocate(&pool_0, (VOID **) &pointer_3, TX_NO_WAIT);

    /* Check status.  */
    if (status != TX_SUCCESS)
    {

        /* Block memory error.  */
        printf("ERROR #18\n");
        test_control_return(1);
    }

    /* Set all the memory of the blocks.  */
    TX_MEMSET(pointer_3, (CHAR) 0xEF, 100);

    /* Attempt to allocate fourth block from the pool.  This should fail because
       there should be no more blocks in the pool.  */
    status = tx_block_allocate(&pool_0, (VOID **) &pointer_4, TX_NO_WAIT);

    /* Check status.  */
    if (status != TX_NO_MEMORY)
    {

        /* Block memory error.  */
        printf("ERROR #19\n");
        test_control_return(1);
    }

#ifndef TX_DISABLE_ERROR_CHECKING

    /* Create a timer for the test.  */
    tx_timer_create(&timer_0, "timer 0", timer_entry, 0, 1, 1, TX_AUTO_ACTIVATE);

    /* Setup the ISR.  */
    test_isr_dispatch =  test_isr;

    /* Sleep for a bit...  */
    tx_thread_sleep(3);
    
    /* Now resume the background thread.  */
    tx_thread_resume(&thread_1);
    
    /* Sleep for a bit...  */
    tx_thread_sleep(3);

    /* Clear the ISR.  */
    test_isr_dispatch =  TX_NULL;

    /* Test for error.  */
    if ((error) || (timer_executed != 1) || (isr_executed != 1))
    {
    
        /* Block memory error.  */
        printf("ERROR #20\n");
        test_control_return(1);
    }

#endif

    /* Delete both block pools.  */
    status =  tx_block_pool_delete(&pool_0);

    /* Check status.  */
    if (status != TX_SUCCESS)
    {

        /* Block memory error.  */
        printf("ERROR #21\n");
        test_control_return(1);
    }

    status =  tx_block_pool_delete(&pool_1);

    /* Check status.  */
    if ((status != TX_SUCCESS) || (error))
    {

        /* Block memory error.  */
        printf("ERROR #22\n");
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
