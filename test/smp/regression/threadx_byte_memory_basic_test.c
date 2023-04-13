/* This test is designed to test simple memory byte pool creation, deletion, and 
   allocates and releases.  */

#include   <stdio.h>
#include   "tx_api.h"

typedef struct BYTE_MEMORY_TEST_STRUCT
{
    ULONG           first;
    ULONG           second;
    TX_BYTE_POOL    pool;
    ULONG           first_middle;
    ULONG           second_middle;
    ULONG           pool_area[2048/sizeof(ULONG)];
    ULONG           next_to_last;
    ULONG           last;

} BYTE_MEMORY_TEST;

static  BYTE_MEMORY_TEST   byte_memory;


/* Define external reference to byte pool creation status.  */

extern UINT            test_byte_pool_create_init;



/* Define the ISR dispatch.  */

extern VOID    (*test_isr_dispatch)(void);

UINT        _txe_byte_pool_create(TX_BYTE_POOL *pool_ptr, CHAR *name_ptr, VOID *pool_start,
                    ULONG pool_size, UINT pool_control_block_size);


static unsigned long   thread_0_counter =  0;
static TX_THREAD       thread_0;


static TX_BYTE_POOL   pool_0;
static TX_BYTE_POOL   pool_1;
static TX_BYTE_POOL   pool_2;
static TX_BYTE_POOL   pool_3;

static TX_BYTE_POOL   pool_4;
static UCHAR          *search_ptr_1;
static UCHAR          *block_0;
static UCHAR          *block_1;
static UCHAR          *block_2;
static UCHAR          *block_3;
static UCHAR          *block_4;
static CHAR           *pool_4_memory;
static TX_TIMER        timer_0;


static unsigned long error =  0;
static unsigned long timer_executed =  0;
static unsigned long isr_executed =  0;


/* Define thread prototypes.  */

static void    thread_0_entry(ULONG thread_input);
UINT            _tx_byte_release(VOID *memory_ptr);


/* Prototype for test control return.  */

void  test_control_return(UINT status);


/* Define the timer for this test.  */

static void    timer_entry(ULONG i)
{

#ifndef TX_DISABLE_ERROR_CHECKING

UINT    status;
CHAR    *pointer;


    /* Determine if calling byte pool create from initialization was successful.  */
    if (test_byte_pool_create_init != TX_SUCCESS)
    {
    
        /* Error!  */
        error++;
    }

    /* Attempt to create a byte pool from a timer.  */
    pointer =  (CHAR *) 0x30000;
    status =  tx_byte_pool_create(&pool_2, "pool 2", pointer, 108);
    
        /* Check status.  */
    if (status != TX_CALLER_ERROR)
    {

        /* Error!  */
        error++;
    }

    /* Attempt to create a byte pool with an invalid size.  */
    status =  _txe_byte_pool_create(&pool_3, "pool 3", pointer,
                    108, (sizeof(TX_BYTE_POOL)+1));

       /* Check status.  */
    if (status != TX_POOL_ERROR)
    {

        /* Error!  */
        error++;
    }

    /* Allocate memory from the pool.  */
    status = tx_byte_allocate(&pool_0, (VOID **) &pointer, 24, TX_NO_WAIT);

    /* Check status.  */
    if (status != TX_CALLER_ERROR)
    {

        /* Error!  */
        error++;
    }

    /* Try to delete a byte pool from a timer.  */
    status =  tx_byte_pool_delete(&pool_0);

    /* Check status.  */
    if (status != TX_CALLER_ERROR)
    {

        /* Error!  */
        error++;
    }

    /* Attempt to release byte memory from timer.  */
    pointer = (CHAR *) 0x30000;
    status =  tx_byte_release(pointer);
    
    /* Check for error status!  */
    if (status != TX_CALLER_ERROR)
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

CHAR    *pointer;
UINT    status;


    /* Allocate memory from the pool.  */
    status = tx_byte_allocate(&pool_0, (VOID **) &pointer, 24, 100);

    /* Check status.  */
    if (status != TX_WAIT_ERROR)
    {

        /* Error!  */
        error++;
    }

    /* Allocate memory from the pool.  */
    status = tx_byte_allocate(&pool_0, (VOID **) &pointer, 24, TX_NO_WAIT);

    /* Check status.  */
    if (status != TX_CALLER_ERROR)
    {

        /* Error!  */
        error++;
    }


    /* Attempt to create a byte pool from an ISR.  */
    status =  tx_byte_pool_create(&pool_2, "pool 0", (void *) 0x100000, 108);

    /* Check status.  */
    if (status != TX_CALLER_ERROR)
    {

        /* Error!  */
        error++;
    }

    /* Attempt to deleate a pool from an ISR.  */
    status =  tx_byte_pool_delete(&pool_0);

    if (status != TX_CALLER_ERROR)
    {

        /* Error!  */
        error++;
    }

    /* Attempt to release byte memory from ISR.  */
    pointer = (CHAR *) 0x30000;
    status =  tx_byte_release(pointer);
    
    /* Check for error status!  */
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
void    threadx_byte_memory_basic_application_define(void *first_unused_memory)
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

        printf("Running Byte Memory Basic Test...................................... ERROR #1\n");
        test_control_return(1);
    }

    /* Create byte pools 0 and 1.  */
    status =  tx_byte_pool_create(&pool_0, "pool 0", pointer, 108);
    pointer = pointer + 108;

    /* Check status.  */
    if (status != TX_SUCCESS)
    {

        printf("Running Byte Memory Basic Test...................................... ERROR #2\n");
        test_control_return(1);
    }

    status =  tx_byte_pool_create(&pool_1, "pool 1", pointer, 200);
    pointer = pointer + 200;

    /* Check status.  */
    if (status != TX_SUCCESS)
    {

        printf("Running Byte Memory Basic Test...................................... ERROR #3\n");
        test_control_return(1);
    }

    /* Test for search pointer issue on wrapped seach with prior block to search pointer merged.  */
    status =  tx_byte_pool_create(&pool_4, "pool 4", pointer, 300);
    pool_4_memory =  pointer;
    pointer = pointer + 300;

    /* Check status.  */
    if (status != TX_SUCCESS)
    {

        printf("Running Byte Memory Basic Test...................................... ERROR #3a\n");
        test_control_return(1);
    }
  
    /* Allocate first block.  */
    status +=  tx_byte_allocate(&pool_4, (VOID **) &block_0, 80, TX_NO_WAIT);
    
    /* Save next search pointer.  */
    search_ptr_1 =  pool_4.tx_byte_pool_search;
    
    /* Clear the allocatged memory.  */
    TX_MEMSET(block_0, 0, 80);
    
    /* Allocate another block.  */
    status +=  tx_byte_allocate(&pool_4, (VOID **) &block_1, 80, TX_NO_WAIT);
    
    /* Clear the allocated block.  */
    TX_MEMSET(block_1, 0, 80);
    
    /* Allocate the third and final block.  */
    status +=  tx_byte_allocate(&pool_4, (VOID **) &block_2, 80, TX_NO_WAIT);
      
    /* Clear the allocated block.  */
    TX_MEMSET(block_2, 0, 80);

    /* Release the first block.  */
    status +=  tx_byte_release(block_0);
    
    /* Release the second block.  */
    status +=  tx_byte_release(block_1);
    
    /* Manually move the search pointer to create the case where the search wraps and a merge happens on the search pointer
       necessitating its update.  */
    pool_4.tx_byte_pool_search  = search_ptr_1;         /* Point to the middle block.  */

    /* Allocate a larger block that will wrap the search and require moving as well as an update of the search pointer.  */
    status +=  tx_byte_allocate(&pool_4, (VOID **) &block_3, 120, TX_NO_WAIT);
    
    /* Clear the newly allocated block.  */    
    TX_MEMSET(block_3, 0, 120);
    
    /* At this point, verify the search pointer was properly updated in the previous allocation.  */
    status +=  tx_byte_allocate(&pool_4, (VOID **) &block_4, 40, TX_NO_WAIT);   /* Should fail since search pointer is now invalid!  */

    /* Check status.  */
    if (status != TX_SUCCESS)
    {

        printf("Running Byte Memory Basic Test...................................... ERROR #3b\n");
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
INT     i;
ULONG   array[20];
UCHAR   *save_search;


    /* Inform user.  */
    printf("Running Byte Memory Basic Test...................................... ");

    /* Perform byte memory test.  */
    byte_memory.first =        0x11223344;
    byte_memory.second =       0x55667788;
    byte_memory.first_middle = 0x21314151;
    byte_memory.second_middle= 0x61718191;
    byte_memory.next_to_last = 0x99aabbcc;
    byte_memory.last =         0xddeeff00;
    
    /* Create the byte pool.  */
    status =  tx_byte_pool_create(&byte_memory.pool, "pool memory", &byte_memory.pool_area[0], (2048*sizeof(ULONG))/sizeof(ULONG));
    tx_byte_pool_delete(&byte_memory.pool);

    /* Check for status.  */
    if ((status != TX_SUCCESS) ||
        (byte_memory.first != 0x11223344) ||
        (byte_memory.second != 0x55667788) ||
        (byte_memory.first_middle != 0x21314151) ||
        (byte_memory.second_middle != 0x61718191) ||
        (byte_memory.next_to_last != 0x99aabbcc) ||
        (byte_memory.last != 0xddeeff00))
    {

        /* Byte memory error.  */
        printf("ERROR #4\n");
        test_control_return(1);
    }

    /* Increment the run counter.  */
    thread_0_counter++;

#ifndef TX_DISABLE_ERROR_CHECKING

    /* Try to create a NULL pool.  */
    pointer_1 = (CHAR *) 0x30000;
    status =  tx_byte_pool_create(TX_NULL, "pool 0", pointer_1, 108);

    /* Check status.  */
    if (status != TX_POOL_ERROR)
    {

        /* Byte memory error.  */
        printf("ERROR #5\n");
        test_control_return(1);
    }

    /* Try to create the same pool.  */
    status =  tx_byte_pool_create(&pool_0, "pool 0", pointer_1, 108);

    /* Check status.  */
    if (status != TX_POOL_ERROR)
    {

        /* Byte memory error.  */
        printf("ERROR #6\n");
        test_control_return(1);
    }

    /* Try to create a pool with a NULL start address.  */
    status =  tx_byte_pool_create(&pool_2, "pool 2", TX_NULL, 108);

    /* Check status.  */
    if (status != TX_PTR_ERROR)
    {

        /* Byte memory error.  */
        printf("ERROR #7\n");
        test_control_return(1);
    }

    /* Try to create a pool with a a bad size.  */
    status =  tx_byte_pool_create(&pool_2, "pool 2", pointer_1, 1);

    /* Check status.  */
    if (status != TX_SIZE_ERROR)
    {

        /* Byte memory error.  */
        printf("ERROR #8\n");
        test_control_return(1);
    }

    /* Try to delete a byte pool with a NULL pointer.  */
    status =  tx_byte_pool_delete(TX_NULL);

    /* Check status.  */
    if (status != TX_POOL_ERROR)
    {

        /* Byte memory error.  */
        printf("ERROR #9\n");
        test_control_return(1);
    }

    /* Try to delete a byte pool that hasn't been created.  */
    pool_2.tx_byte_pool_id =  0;
    status =  tx_byte_pool_delete(&pool_2);

    /* Check status.  */
    if (status != TX_POOL_ERROR)
    {

        /* Byte memory error.  */
        printf("ERROR #10\n");
        test_control_return(1);
    }

    /* Test NULL pool pointer.  */
    status = tx_byte_allocate(TX_NULL, (VOID **) &pointer_1, 24, TX_NO_WAIT);

    /* Check status.  */
    if (status != TX_POOL_ERROR)
    {

        /* Byte memory error.  */
        printf("ERROR #11\n");
        test_control_return(1);
    }
    
    /* Test non-created pool pointer.  */
    pool_2.tx_byte_pool_id =  0;
    status = tx_byte_allocate(&pool_2, (VOID **) &pointer_1, 24, TX_NO_WAIT);

    /* Check status.  */
    if (status != TX_POOL_ERROR)
    {

        /* Byte memory error.  */
        printf("ERROR #12\n");
        test_control_return(1);
    }

    /* Test NULL memory pointer.  */
    status = tx_byte_allocate(&pool_0, (VOID **) TX_NULL, 24, TX_NO_WAIT);

    /* Check status.  */
    if (status != TX_PTR_ERROR)
    {

        /* Byte memory error.  */
        printf("ERROR #13\n");
        test_control_return(1);
    }

    /* Test extreme size.  */
    status = tx_byte_allocate(&pool_0, (VOID **) &pointer_1, 240000, TX_NO_WAIT);

    /* Check status.  */
    if (status != TX_SIZE_ERROR)
    {

        /* Byte memory error.  */
        printf("ERROR #14\n");
        test_control_return(1);
    }

    /* Test size of 0.  */
    status = tx_byte_allocate(&pool_0, (VOID **) &pointer_1, 0, TX_NO_WAIT);

    /* Check status.  */
    if (status != TX_SIZE_ERROR)
    {

        /* Byte memory error.  */
        printf("ERROR #15\n");
        test_control_return(1);
    }
#endif

    /* Test NULL pointer release.  */
    status =  tx_byte_release(TX_NULL);
    
    /* Check for error status!  */
    if (status != TX_PTR_ERROR)
    {
    
        /* Byte memory error.  */
        printf("ERROR #16\n");
        test_control_return(1);
    }    

    /* Allocate memory from the pool.  */
    status = tx_byte_allocate(&pool_0, (VOID **) &pointer_1, 24, TX_NO_WAIT);

    /* Check status.  */
    if (status != TX_SUCCESS)
    {

        /* Byte memory error.  */
        printf("ERROR #17\n");
        test_control_return(1);
    }

    /* Allocate memory from the pool.  */
    status = tx_byte_allocate(&pool_0, (VOID **) &pointer_2, 24, TX_NO_WAIT);

    /* Check status.  */
    if (status != TX_SUCCESS)
    {

        /* Byte memory error.  */
        printf("ERROR #18\n");
        test_control_return(1);
    }

    /* Allocate memory from the pool.  */
    status = tx_byte_allocate(&pool_0, (VOID **) &pointer_3, 24, TX_NO_WAIT);

    /* Check status.  */
    if (status != TX_SUCCESS)
    {

        /* Byte memory error.  */
        printf("ERROR #19\n");
        test_control_return(1);
    }

    /* Attempt to allocate fourth area from the pool.  This should fail because
       there should be not enough bytes in the pool.  */
    status = tx_byte_allocate(&pool_0, (VOID **) &pointer_4, 24, TX_NO_WAIT);

    /* Check status.  */
    if (status != TX_NO_MEMORY)
    {

        /* Byte memory error.  */
        printf("ERROR #20\n");
        test_control_return(1);
    }

    /* Set the memory of all of the allocated memory.  */
    for (i =0; i < 24; i++)
    {
        pointer_1[i] = (CHAR) 0xFF;
        pointer_2[i] = (CHAR) 0xFF;
        pointer_3[i] = (CHAR) 0xFF;
    }

    /* Test the byte release with a bad block pointer.  */
    status =  _tx_byte_release(TX_NULL);
    
    /* Check for error status!  */
    if (status != TX_PTR_ERROR)
    {
    
        /* Byte memory error.  */
        printf("ERROR #21\n");
        test_control_return(1);
    }    

    /* Test another bad block release... no pool pointer!  */
    array[0] =  0;
    array[1] =  0;
    array[2] =  0;
    status =  _tx_byte_release(&array[2]);
    
    /* Check for error status!  */
    if (status != TX_PTR_ERROR)
    {
    
        /* Byte memory error.  */
        printf("ERROR #22\n");
        test_control_return(1);
    }    

    /* Test another bad block release.... pool pointer is not a valid pool!  */
    array[0] =  0;
    array[1] =  (ULONG) &array[3];
    array[2] =  0;
    array[3] =  0;
    status =  _tx_byte_release(&array[2]);
    
    /* Check for error status!  */
    if (status != TX_PTR_ERROR)
    {
    
        /* Byte memory error.  */
        printf("ERROR #22\n");
        test_control_return(1);
    }    
    
    /* Now release each of the blocks. */
    status =  tx_byte_release(pointer_1);

    /* Check for status.  */
    if (status != TX_SUCCESS)
    {

        /* Byte memory error.  */
        printf("ERROR #23\n");
        test_control_return(1);
    }

    /* Now release the same block again. */
    status =  tx_byte_release(pointer_1);

    /* Check for status.  */
    if (status != TX_PTR_ERROR)
    {

        /* Byte memory error.  */
        printf("ERROR #24\n");
        test_control_return(1);
    }

    /* Release the second memory area.  */
    status =  tx_byte_release(pointer_2);

    /* Check for status.  */
    if (status != TX_SUCCESS)
    {

        /* Byte memory error.  */
        printf("ERROR #25\n");
        test_control_return(1);
    }

    /* Release the third memory area.  */
    status =  tx_byte_release(pointer_3);

    /* Check for status.  */
    if (status != TX_SUCCESS)
    {

        /* Byte memory error.  */
        printf("ERROR #26\n");
        test_control_return(1);
    }

    /* Allocate each block again to make sure everything still
       works.  */

    /* Allocate memory from the pool.  */
    status = tx_byte_allocate(&pool_0, (VOID **) &pointer_1, 24, TX_NO_WAIT);

    /* Check status.  */
    if (status != TX_SUCCESS)
    {

        /* Byte memory error.  */
        printf("ERROR #27\n");
        test_control_return(1);
    }

    /* Allocate second memory area from the pool.  */
    status = tx_byte_allocate(&pool_0, (VOID **) &pointer_2, 24, TX_NO_WAIT);

    /* Check status.  */
    if (status != TX_SUCCESS)
    {

        /* Byte memory error.  */
        printf("ERROR #28\n");
        test_control_return(1);
    }

    /* Allocate third memory area from the pool.  */
    status = tx_byte_allocate(&pool_0, (VOID **) &pointer_3, 24, TX_NO_WAIT);

    /* Check status.  */
    if (status != TX_SUCCESS)
    {

        /* Byte memory error.  */
        printf("ERROR #29\n");
        test_control_return(1);
    }

    /* Attempt to allocate fourth memory area from the pool.  This should fail because
       there should be not enough bytes in the pool.  */
    status = tx_byte_allocate(&pool_0, (VOID **) &pointer_4, 24, TX_NO_WAIT);

    /* Check status.  */
    if (status != TX_NO_MEMORY)
    {

        /* Byte memory error.  */
        printf("ERROR #30\n");
        test_control_return(1);
    }

    /* Now release each of the blocks. */
    status =  tx_byte_release(pointer_1);

    /* Check for status.  */
    if (status != TX_SUCCESS)
    {

        /* Byte memory error.  */
        printf("ERROR #31\n");
        test_control_return(1);
    }

    /* Release the second memory area.  */
    status =  tx_byte_release(pointer_2);

    /* Check for status.  */
    if (status != TX_SUCCESS)
    {

        /* Byte memory error.  */
        printf("ERROR #32\n");
        test_control_return(1);
    }

    /* Release the third memory area.  */
    status =  tx_byte_release(pointer_3);

    /* Check for status.  */
    if (status != TX_SUCCESS)
    {

        /* Byte memory error.  */
        printf("ERROR #33\n");
        test_control_return(1);
    }

    /* Now allocate a block that should cause all of the blocks to merge 
       together.  */
    status = tx_byte_allocate(&pool_0, (VOID **) &pointer_3, 88, TX_NO_WAIT);

    /* Check status.  */
    if (status != TX_SUCCESS)
    {

        /* Byte memory error.  */
        printf("ERROR #34\n");
        test_control_return(1);
    }

    /* Attempt to allocate fourth memory area from the pool.  This should fail because
       there should be not enough bytes in the pool.  */
    status = tx_byte_allocate(&pool_0, (VOID **) &pointer_4, 24, TX_NO_WAIT);

    /* Check status.  */
    if (status != TX_NO_MEMORY)
    {

        /* Byte memory error.  */
        printf("ERROR #35\n");
        test_control_return(1);
    }

    /* Release the block.  */
    status =  tx_byte_release(pointer_3);

    /* Check status.  */
    if (status != TX_SUCCESS)
    {

        /* Byte memory error.  */
        printf("ERROR #36\n");
        test_control_return(1);
    }
    
    /* Now ensure the search pointer update in the byte search algorithm is updated.  */
    
    /* Allocate memory from the pool.  */
    status = tx_byte_allocate(&pool_0, (VOID **) &pointer_1, 24, TX_NO_WAIT);

    /* Check status.  */
    if (status != TX_SUCCESS)
    {

        /* Byte memory error.  */
        printf("ERROR #37\n");
        test_control_return(1);
    }

    /* Allocate second memory area from the pool.  */
    status = tx_byte_allocate(&pool_0, (VOID **) &pointer_2, 24, TX_NO_WAIT);

    /* Check status.  */
    if (status != TX_SUCCESS)
    {

        /* Byte memory error.  */
        printf("ERROR #38\n");
        test_control_return(1);
    }

    /* Allocate third memory area from the pool.  */
    status = tx_byte_allocate(&pool_0, (VOID **) &pointer_3, 24, TX_NO_WAIT);

    /* Check status.  */
    if (status != TX_SUCCESS)
    {

        /* Byte memory error.  */
        printf("ERROR #39\n");
        test_control_return(1);
    }
    
    /* Release the middle block.  */
    status =  tx_byte_release(pointer_2);    
    
    /* Check status.  */
    if (status != TX_SUCCESS)
    {

        /* Byte memory error.  */
        printf("ERROR #40\n");
        test_control_return(1);
    }

    /* Move the search pointer to the third block to exercise that code in the byte search algorithm.  */
    pool_0.tx_byte_pool_search =  (UCHAR *) pointer_3-8;

    /* Now allocate the block again.  */
    status = tx_byte_allocate(&pool_0, (VOID **) &pointer_2, 24, TX_NO_WAIT);

    /* Check status.  */
    if (status != TX_SUCCESS)
    {

        /* Byte memory error.  */
        printf("ERROR #41\n");
        test_control_return(1);
    }

    /* Now release the blocks are test the merge with the update of the search pointer.  */
    status =   tx_byte_release(pointer_3);
    status +=  tx_byte_release(pointer_2);
    status +=  tx_byte_release(pointer_1);
    
    /* Move the search pointer to the third block to exercise that code in the byte search algorithm.  */
    pool_0.tx_byte_pool_search =  (UCHAR *) pointer_3-8;

    /* Allocate a large block to force the search pointer update.  */
    status = tx_byte_allocate(&pool_0, (VOID **) &pointer_3, 88, TX_NO_WAIT);

    /* Check status.  */
    if (status != TX_SUCCESS)
    {

        /* Byte memory error.  */
        printf("ERROR #42\n");
        test_control_return(1);
    }
    
    
#ifndef TX_DISABLE_ERROR_CHECKING

    /* Create a timer for the test.  */
    tx_timer_create(&timer_0, "timer 0", timer_entry, 0, 1, 1, TX_AUTO_ACTIVATE);

    /* Setup the ISR.  */
    test_isr_dispatch =  test_isr;

    /* Sleep for a bit...  */
    tx_thread_sleep(3);

    /* Clear the ISR.  */
    test_isr_dispatch =  TX_NULL;

    /* Test for error.  */
    if ((error) || (timer_executed != 1) || (isr_executed != 1))
    {
    
        /* Byte memory error.  */
        printf("ERROR #43\n");
        test_control_return(1);
    }

#endif
    
    /* Delete both byte pools.  */
    status =  tx_byte_pool_delete(&pool_0);

    /* Check status.  */
    if (status != TX_SUCCESS)
    {

        /* Byte memory error.  */
        printf("ERROR #44\n");
        test_control_return(1);
    }

    status =  tx_byte_pool_delete(&pool_1);

    /* Check status.  */
    if (status != TX_SUCCESS)
    {

        /* Byte memory error.  */
        printf("ERROR #45\n");
        test_control_return(1);
    }

    /* Delete pool 4.  */
    status =  tx_byte_pool_delete(&pool_4);
    
    /* Check status.  */
    if (status != TX_SUCCESS)
    {

        /* Byte memory error.  */
        printf("ERROR #46\n");
        test_control_return(1);
    }
    
    /* Create pool 4.  */
    status =  tx_byte_pool_create(&pool_4, "pool 4", pool_4_memory, 300);
    
    /* Check status.  */
    if (status != TX_SUCCESS)
    {

        /* Byte memory error.  */
        printf("ERROR #47\n");
        test_control_return(1);
    }

    /* Allocate three blocks...  */
    status =   tx_byte_allocate(&pool_4, (VOID **) &pointer_1, 84, TX_NO_WAIT);
    status +=  tx_byte_allocate(&pool_4, (VOID **) &pointer_2, 84, TX_NO_WAIT);
    status +=  tx_byte_allocate(&pool_4, (VOID **) &pointer_3, 84, TX_NO_WAIT);

    /* Check status.  */
    if (status != TX_SUCCESS)
    {

        /* Byte memory error.  */
        printf("ERROR #48\n");
        test_control_return(1);
    }
    
    /* At this point, there should be three allocated blocks and the reserved block at the end.  */
    
    /* Now release all the blocks in reverse order. This should leave the search pointer at the last block.  */
    status =  tx_byte_release(pointer_3);
    save_search =  pool_4.tx_byte_pool_search;
    status += tx_byte_release(pointer_2);
    status += tx_byte_release(pointer_1);
    
    /* Move the search pointer back to the last block.  */
    pool_4.tx_byte_pool_search =  save_search;

    /* Check status.  */
    if (status != TX_SUCCESS)
    {

        /* Byte memory error.  */
        printf("ERROR #49\n");
        test_control_return(1);
    }   

    /* Now attempt to allocate a block that requires a merge, which should exercise the branch in byte search that does not 
       result in a search pointer change.  */
    status =  tx_byte_allocate(&pool_4, (VOID **) &pointer_1, 168, TX_NO_WAIT);
    
    /* Check status.  */
    if (status != TX_SUCCESS)
    {

        /* Byte memory error.  */
        printf("ERROR #50\n");
        test_control_return(1);
    }      
    
    /* Release the last block.  */
    status =  tx_byte_release(pointer_1);
    
    /* Allocate all the blocks.  */
    status =  tx_byte_allocate(&pool_4, (VOID **) &pointer_1, 84, TX_NO_WAIT);
    status += tx_byte_allocate(&pool_4, (VOID **) &pointer_2, 84, TX_NO_WAIT);
    status += tx_byte_allocate(&pool_4, (VOID **) &pointer_3, 84, TX_NO_WAIT);
    
    /* Release all of the blocks in order.  */
    status += tx_byte_release(pointer_1);
    status += tx_byte_release(pointer_2);
    status += tx_byte_release(pointer_3);
    
    /* Check status.  */
    if (status != TX_SUCCESS)
    {

        /* Byte memory error.  */
        printf("ERROR #50\n");
        test_control_return(1);
    }      

    /* Now setup a special test to exercise the examine blocks equal to 0 path in the byte pool search.  */
    pool_4.tx_byte_pool_search =     save_search;
    pool_4.tx_byte_pool_fragments =  (UINT) (-1);

    /* Call byte allocate to execise the examine blocks equal to 0 path on non-merge block condition.  */
    status =  tx_byte_allocate(&pool_4, (VOID **) &pointer_1, 168, TX_NO_WAIT);
    
    /* Check status.  */
    if (status != TX_NO_MEMORY)
    {

        /* Byte memory error.  */
        printf("ERROR #51\n");
        test_control_return(1);
    }      
    
    /* Successful test.  */
    printf("SUCCESS!\n");
    test_control_return(0);
}

