/* This test is designed to test error detection for simple memory block operations.  */
   
#include   <stdio.h>
#include   "tx_api.h"

static unsigned long   thread_0_counter =  0;
static TX_THREAD       thread_0;


static TX_BLOCK_POOL   pool_0;
static TX_BLOCK_POOL   pool_1;
static TX_BLOCK_POOL   pool_2;
static CHAR            *pointer;


/* Define thread prototypes.  */

static void    thread_0_entry(ULONG thread_input);

UINT        _txe_block_pool_create(TX_BLOCK_POOL *pool_ptr, CHAR *name_ptr, ULONG block_size,
                    VOID *pool_start, ULONG pool_size, UINT pool_control_block_size);


/* Prototype for test control return.  */

void  test_control_return(UINT status);


/* Define what the initial system looks like.  */

#ifdef CTEST
void test_application_define(void *first_unused_memory)
#else
void    threadx_block_memory_error_detection_application_define(void *first_unused_memory)
#endif
{

INT     status;

    /* Put first available memory address into a character pointer.  */
    pointer =  (CHAR *) first_unused_memory;

    /* Put system definition stuff in here, e.g. thread creates and other assorted
       create information.  */

    status =  tx_thread_create(&thread_0, "thread 0", thread_0_entry, 1,  
            pointer, TEST_STACK_SIZE_PRINTF, 
            17, 17, 100, TX_AUTO_START);
    pointer = pointer + TEST_STACK_SIZE_PRINTF;

    /* Create block pool 0.  */
    status =  tx_block_pool_create(&pool_0, "pool 0", 100, pointer, 320);
    pointer = pointer + 320;

#ifndef TX_DISABLE_ERROR_CHECKING   /* skip this test and pretend it passed */

    /* Create block pool again to get pool_ptr error.  */
    status =  tx_block_pool_create(&pool_0, "pool 0", 100, pointer, 320);
    if (status != TX_POOL_ERROR)
        return;

    /* Create block pool with NULL pointer.  */
    status =  tx_block_pool_create(TX_NULL, "pool 0", 100, pointer, 320);
    if (status != TX_POOL_ERROR)
    {

        printf("Running Block Memory Error Detection Test........................... ERROR #1\n");
        test_control_return(1);
    }

    /* Create block pool pointer if NULL start.  */
    status =  tx_block_pool_create(&pool_1, "pool 0", 100, NULL, 320);
    if (status != TX_PTR_ERROR)
    {

        printf("Running Block Memory Error Detection Test........................... ERROR #2\n");
        test_control_return(1);
    }

    /* Create block pool pointer with invalid size.  */
    status =  tx_block_pool_create(&pool_1, "pool 0", 100, pointer, 100);
    if (status != TX_SIZE_ERROR)
    {

        printf("Running Block Memory Error Detection Test........................... ERROR #3\n");
        test_control_return(1);
    }

    /* Attempt of allocate block with NULL dest.  */
    status =  tx_block_allocate(&pool_0, (VOID **) TX_NULL, TX_NO_WAIT);
    if (status != TX_PTR_ERROR)
    {

        printf("Running Block Memory Error Detection Test........................... ERROR #4\n");
        test_control_return(1);
    }

    /* Delete null pointer.  */
    status =  tx_block_pool_delete(TX_NULL);
    if (status != TX_POOL_ERROR)
    {

        printf("Running Block Memory Error Detection Test........................... ERROR #5\n");
        test_control_return(1);
    }

#endif  /* TX_DISABLE_ERROR_CHECKING */
}



/* Define the test threads.  */

static void    thread_0_entry(ULONG thread_input)
{

#ifndef TX_DISABLE_ERROR_CHECKING
UINT    status;
CHAR    *pointer_1;
CHAR    *pointer_2;
CHAR    *pointer_3;
CHAR    *pointer_4;
INT     i;
#endif

    /* Inform user.  */
    printf("Running Block Memory Error Detection Test........................... ");

#ifndef TX_DISABLE_ERROR_CHECKING   /* skip this test and pretend it passed */

    status =  tx_block_pool_create(&pool_1, "pool 1", 100, pointer, 320);
    pointer = pointer + 320;

    /* Attempt to create a pool with an invalid size.  */
    status =  _txe_block_pool_create(&pool_2, "pool 2", 100, pointer, 320, 777777);
    if (status != TX_POOL_ERROR)
    {

        /* Error!  */
        printf("ERROR #6\n");
        test_control_return(1);
    }

    /* Allocate with a NULL destination pointer.  */
    status =  tx_block_allocate(&pool_0, (VOID **) TX_NULL, TX_NO_WAIT);
    if (status != TX_PTR_ERROR)
    {

        /* Error!  */
        printf("ERROR #7\n");
        test_control_return(1);
    }

    /* Allocate with a NULL pool pointer.  */
    status =  tx_block_allocate(TX_NULL, (VOID **) TX_NULL, TX_NO_WAIT);
    if (status != TX_POOL_ERROR)
    {

        /* Error!  */
        printf("ERROR #8\n");
        test_control_return(1);
    }
    
    /* Allocate with bad pool pointer.  */
    pool_2.tx_block_pool_id =  0;
    status =  tx_block_allocate(&pool_2, (VOID **) TX_NULL, TX_NO_WAIT);
    if (status != TX_POOL_ERROR)
    {

        /* Error!  */
        printf("ERROR #9\n");
        test_control_return(1);
    }

    /* Release bad pool ptr. */
    status =  tx_block_release(TX_NULL);
    if (status != TX_PTR_ERROR)
    {

        /* Error!  */
        printf("ERROR #10\n");
        test_control_return(1);
    }

    /* Allocate first block from the pool.  */
    status = tx_block_allocate(&pool_0, (VOID **) &pointer_1, TX_NO_WAIT);

    /* Check status.  */
    if (status != TX_SUCCESS)
    {

        /* Error!  */
        printf("ERROR #11\n");
        test_control_return(1);
    }

    /* Set all the memory of the blocks.  */
    TX_MEMSET(pointer_1, (CHAR) 0xEF, 100);

    /* Allocate second block from the pool.  */
    status = tx_block_allocate(&pool_0, (VOID **) &pointer_2, TX_NO_WAIT);

    /* Check status.  */
    if (status != TX_SUCCESS)
    {

        /* Error!  */
        printf("ERROR #12\n");
        test_control_return(1);
    }

    /* Set all the memory of the blocks.  */
    TX_MEMSET(pointer_2, (CHAR) 0xEF, 100);

    /* Allocate third block from the pool.  */
    status = tx_block_allocate(&pool_0, (VOID **) &pointer_3, TX_NO_WAIT);

    /* Check status.  */
    if (status != TX_SUCCESS)
    {

        /* Error!  */
        printf("ERROR #13\n");
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

        /* Error!  */
        printf("ERROR #14\n");
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

        /* Error!  */
        printf("ERROR #15\n");
        test_control_return(1);
    }

    /* Release the second block.  */
    status =  tx_block_release(pointer_2);

    /* Check for status.  */
    if (status != TX_SUCCESS)
    {

        /* Error!  */
        printf("ERROR #16\n");
        test_control_return(1);
    }

    /* Release the third block.  */
    status =  tx_block_release(pointer_3);

    /* Check for status.  */
    if (status != TX_SUCCESS)
    {

        /* Error!  */
        printf("ERROR #17\n");
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

        /* Error!  */
        printf("ERROR #18\n");
        test_control_return(1);
    }

    /* Set all the memory of the blocks.  */
    TX_MEMSET(pointer_1, (CHAR) 0xEF, 100);

    /* Allocate second block from the pool.  */
    status = tx_block_allocate(&pool_0, (VOID **) &pointer_2, TX_NO_WAIT);

    /* Check status.  */
    if (status != TX_SUCCESS)
    {

        /* Error!  */
        printf("ERROR #19\n");
        test_control_return(1);
    }

    /* Set all the memory of the blocks.  */
    TX_MEMSET(pointer_2, (CHAR) 0xEF, 100);

    /* Allocate third block from the pool.  */
    status = tx_block_allocate(&pool_0, (VOID **) &pointer_3, TX_NO_WAIT);

    /* Check status.  */
    if (status != TX_SUCCESS)
    {

        /* Error!  */
        printf("ERROR #20\n");
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

        /* Error!  */
        printf("ERROR #21\n");
        test_control_return(1);
    }

    /* Delete both block pools.  */
    status =  tx_block_pool_delete(&pool_0);
    if (status != TX_SUCCESS)
    {

        /* Error!  */
        printf("ERROR #22\n");
        test_control_return(1);
    }

    status =  tx_block_pool_delete(&pool_1);
    if (status != TX_SUCCESS)
    {

        /* Error!  */
        printf("ERROR #23\n");
        test_control_return(1);
    }

    /* Attempt to delete again.  */
    status =  tx_block_pool_delete(&pool_1);
    if (status != TX_POOL_ERROR)
    {

        /* Error!  */
        printf("ERROR #24\n");
        test_control_return(1);
    }

    thread_0_counter++;

#endif  /* TX_DISABLE_ERROR_CHECKING */
    
    /* All is good!  */
    printf("SUCCESS!\n");
    test_control_return(0);
}

