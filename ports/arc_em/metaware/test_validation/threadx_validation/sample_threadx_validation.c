/* This is a small demo of the high-performance ThreadX kernel.  It includes examples of eight
   threads of different priorities, using a message queue, semaphore, mutex, event flags group, 
   byte pool, and block pool.  */

#include "tx_api.h"

#define TX_TEST_STACK_SIZE         (1024 * 2)
#define TX_TEST_QUEUE_SIZE         (16)
#define TX_TEST_BYTE_POOL_SIZE     (1024 * 4)
#define TX_TEST_BLOCK_POOL_SIZE    (1024 * 4)


/* Define the ThreadX object control blocks...  */

TX_THREAD               tx_test_thread_0;
TX_THREAD               tx_test_thread_1;
TX_THREAD               tx_test_thread_2;
TX_THREAD               tx_test_thread_3;
TX_THREAD               tx_test_thread_4;
TX_THREAD               tx_test_thread_5;
TX_THREAD               tx_test_thread_6;
TX_THREAD               tx_test_thread_7;
TX_QUEUE                tx_test_queue_0;
TX_SEMAPHORE            tx_test_semaphore_0;
TX_MUTEX                tx_test_mutex_0;
TX_EVENT_FLAGS_GROUP    tx_test_event_flags_0;
TX_BYTE_POOL            tx_test_byte_pool_0;
TX_BLOCK_POOL           tx_test_block_pool_0;


/* Define the counters used in the demo application...  */

ULONG					tx_test_error_counter;
ULONG                   tx_test_thread_0_counter;
ULONG                   tx_test_thread_1_counter;
ULONG                   tx_test_thread_2_counter;


/* Define thread prototypes.  */

void    tx_test_thread_0_entry(ULONG thread_input);
void    tx_test_thread_1_entry(ULONG thread_input);
void    tx_test_thread_2_entry(ULONG thread_input);
void    tx_test_thread_3_and_4_entry(ULONG thread_input);
void    tx_test_thread_5_entry(ULONG thread_input);
void    tx_test_thread_6_and_7_entry(ULONG thread_input);


/* Define the thread stacks.  */

ULONG	tx_test_thread_0_stack[TX_TEST_STACK_SIZE / sizeof(ULONG)];
ULONG	tx_test_thread_1_stack[TX_TEST_STACK_SIZE / sizeof(ULONG)];
ULONG	tx_test_thread_2_stack[TX_TEST_STACK_SIZE / sizeof(ULONG)];
ULONG	tx_test_thread_3_stack[TX_TEST_STACK_SIZE / sizeof(ULONG)];
ULONG	tx_test_thread_4_stack[TX_TEST_STACK_SIZE / sizeof(ULONG)];
ULONG	tx_test_thread_5_stack[TX_TEST_STACK_SIZE / sizeof(ULONG)];
ULONG	tx_test_thread_6_stack[TX_TEST_STACK_SIZE / sizeof(ULONG)];
ULONG	tx_test_thread_7_stack[TX_TEST_STACK_SIZE / sizeof(ULONG)];


/* Define other buffers used by the test code.  */

ULONG 	tx_test_queue_0_buffer[TX_TEST_QUEUE_SIZE];
ULONG 	tx_test_byte_pool_0_buffer[TX_TEST_BYTE_POOL_SIZE / sizeof(ULONG)];
ULONG 	tx_test_block_pool_0_buffer[TX_TEST_BLOCK_POOL_SIZE / sizeof(ULONG)];


/* Define test function prototypes.  */

void    tx_test_sleep();
void    tx_test_busy_loop();
void    tx_test_byte_alloc_and_free();

int     tx_test_sum_many_params(
			void (*f)(),
			int p1, int p2, int p3, int p4, int p5, int p6, int p7, int p8,
			int p9, int p10, int p11, int p12, int p13, int p14, int p15, int p16,
			int p17, int p18, int p19, int p20, int p21, int p22, int p23, int p24,
			int p25, int p26, int p27, int p28, int p29, int p30, int p31, int p32,
			int p33, int p34, int p35, int p36, int p37, int p38, int p39, int p40,
			int p41, int p42, int p43, int p44, int p45, int p46, int p47, int p48,
			int p49, int p50, int p51, int p52, int p53, int p54, int p55, int p56,
			int p57, int p58, int p59, int p60, int p61, int p62, int p63, int p64,
			int p65, int p66, int p67, int p68, int p69, int p70, int p71, int p72);

int     tx_test_xor_many_params(
			void (*f)(),
			int p1, int p2, int p3, int p4, int p5, int p6, int p7, int p8,
			int p9, int p10, int p11, int p12, int p13, int p14, int p15, int p16,
			int p17, int p18, int p19, int p20, int p21, int p22, int p23, int p24,
			int p25, int p26, int p27, int p28, int p29, int p30, int p31, int p32,
			int p33, int p34, int p35, int p36, int p37, int p38, int p39, int p40,
			int p41, int p42, int p43, int p44, int p45, int p46, int p47, int p48,
			int p49, int p50, int p51, int p52, int p53, int p54, int p55, int p56,
			int p57, int p58, int p59, int p60, int p61, int p62, int p63, int p64,
			int p65, int p66, int p67, int p68, int p69, int p70, int p71, int p72);


/* Define the error handler. */

void tx_test_error_handler()
{
	tx_test_error_counter++;

	for (;;)
	{
		/* Stay here forever */
	}
}

/* Define main entry point.  */

int main()
{

    /* Enter the ThreadX kernel.  */
    tx_kernel_enter();

    return(0);
}


/* Define what the initial system looks like.  */

void    tx_application_define(void *first_unused_memory)
{

UINT	status = TX_SUCCESS;


    /* Create the ThreadX test thread 0.  */
    status = tx_thread_create(
    		&tx_test_thread_0, "ThreadX test thread 0",
			tx_test_thread_0_entry, 0,
			tx_test_thread_0_stack,
			TX_TEST_STACK_SIZE,
            5, 5,
			TX_NO_TIME_SLICE,
			TX_AUTO_START);
    if (status != TX_SUCCESS) tx_test_error_handler();

    /* Create the ThreadX test thread 1.  */
    status = tx_thread_create(
    		&tx_test_thread_1, "ThreadX test thread 1",
			tx_test_thread_1_entry, 1,
    		tx_test_thread_1_stack,
			TX_TEST_STACK_SIZE,
            7, 7,
			3, /* time slice */
			TX_AUTO_START);
    if (status != TX_SUCCESS) tx_test_error_handler();

    /* Create the ThreadX test thread 2.  */
    status = tx_thread_create(
    		&tx_test_thread_2, "ThreadX test thread 2",
			tx_test_thread_2_entry, 2,
    		tx_test_thread_2_stack,
			TX_TEST_STACK_SIZE,
            7, 5,
			4, /* time slice */
			TX_AUTO_START);
    if (status != TX_SUCCESS) tx_test_error_handler();

    /* Create the ThreadX test thread 3.  */
    status = tx_thread_create(
    		&tx_test_thread_3, "ThreadX test thread 3",
			tx_test_thread_3_and_4_entry, 3,
    		tx_test_thread_3_stack,
			TX_TEST_STACK_SIZE,
            7, 5,
			4, /* time slice */
			TX_AUTO_START);
    if (status != TX_SUCCESS) tx_test_error_handler();

    /* Create the ThreadX test thread 4.  */
    status = tx_thread_create(
    		&tx_test_thread_4, "ThreadX test thread 4",
			tx_test_thread_3_and_4_entry, 4,
    		tx_test_thread_4_stack,
			TX_TEST_STACK_SIZE,
            7, 7,
			4, /* time slice */
			TX_AUTO_START);
    if (status != TX_SUCCESS) tx_test_error_handler();

    /* Create the ThreadX test thread 5.  */
    status = tx_thread_create(
    		&tx_test_thread_5, "ThreadX test thread 5",
			tx_test_thread_5_entry, 5,
    		tx_test_thread_5_stack,
			TX_TEST_STACK_SIZE,
            7, 7,
			4, /* time slice */
			TX_AUTO_START);
    if (status != TX_SUCCESS) tx_test_error_handler();

    /* Create the ThreadX test thread 6.  */
    status = tx_thread_create(
    		&tx_test_thread_6, "ThreadX test thread 6",
			tx_test_thread_6_and_7_entry, 6,
    		tx_test_thread_6_stack,
			TX_TEST_STACK_SIZE,
            7, 7,
			4, /* time slice */
			TX_AUTO_START);
    if (status != TX_SUCCESS) tx_test_error_handler();

    /* Create the ThreadX test thread 2.  */
    status = tx_thread_create(
    		&tx_test_thread_7, "ThreadX test thread 7",
			tx_test_thread_6_and_7_entry, 7,
    		tx_test_thread_7_stack,
			TX_TEST_STACK_SIZE,
            7, 7,
			4, /* time slice */
			TX_AUTO_START);
    if (status != TX_SUCCESS) tx_test_error_handler();


    /* Create the message queue shared by threads 1 and 2.  */
    status = tx_queue_create(&tx_test_queue_0, "ThreadX test queue 0", TX_1_ULONG, tx_test_queue_0_buffer, TX_TEST_QUEUE_SIZE*sizeof(ULONG));
    if (status != TX_SUCCESS) tx_test_error_handler();

    /* Create the semaphore used for testing.  */
    status = tx_semaphore_create(&tx_test_semaphore_0, "ThreadX test semaphore 0", 1);
    if (status != TX_SUCCESS) tx_test_error_handler();

    /* Create the event flags group used by threads 1 and 5.  */
    status = tx_event_flags_create(&tx_test_event_flags_0, "ThreadX test event flags 0");
    if (status != TX_SUCCESS) tx_test_error_handler();

    /* Create the mutex used for testing without priority inheritance.  */
    status = tx_mutex_create(&tx_test_mutex_0, "ThreadX test mutex 0", TX_NO_INHERIT);
    if (status != TX_SUCCESS) tx_test_error_handler();

    /* Create a byte memory pool for testing.  */
    status = tx_byte_pool_create(&tx_test_byte_pool_0, "ThreadX test byte pool 0", tx_test_byte_pool_0_buffer, TX_TEST_BYTE_POOL_SIZE);
    if (status != TX_SUCCESS) tx_test_error_handler();

    /* Quick test of the byte pool during initialization.  */
    {
		CHAR *pointer = TX_NULL;

		/* Allocate a block and release the block memory.  */
		status = tx_byte_allocate(&tx_test_byte_pool_0, (VOID **) &pointer, 123, TX_NO_WAIT);
	    if (status != TX_SUCCESS) tx_test_error_handler();

		/* Release the block back to the pool.  */
		status = tx_byte_release(pointer);
	    if (status != TX_SUCCESS) tx_test_error_handler();
    }

    /* Create a block memory pool to allocate a message buffer from.  */
    status = tx_block_pool_create(&tx_test_block_pool_0, "ThreadX test block pool 0", 128, tx_test_block_pool_0_buffer, TX_TEST_BLOCK_POOL_SIZE);
    if (status != TX_SUCCESS) tx_test_error_handler();

    /* Quick test of the block pool during initialization.  */
    {
		CHAR *pointer = TX_NULL;

		/* Allocate a block and release the block memory.  */
		status = tx_block_allocate(&tx_test_block_pool_0, (VOID **) &pointer, TX_NO_WAIT);
	    if (status != TX_SUCCESS) tx_test_error_handler();

		/* Release the block back to the pool.  */
		status = tx_block_release(pointer);
	    if (status != TX_SUCCESS) tx_test_error_handler();
    }
}


/* Define the test functions.  */

void    tx_test_sleep()
{
	tx_thread_sleep(10);
}

void    tx_test_busy_loop()
{
	unsigned i;
	static volatile unsigned x = 0;
	for (i = 0; i < (1024 * 4); ++i)
	{
		x++;
	}
}

void    tx_test_byte_alloc_and_free()
{
	UINT status;

    /* Quick test of the byte pool during initialization.  */
    {
		CHAR *pointer = TX_NULL;

		/* Allocate a block and release the block memory.  */
		status = tx_byte_allocate(&tx_test_byte_pool_0, (VOID **) &pointer, 123, TX_NO_WAIT);
	    if (status != TX_SUCCESS) tx_test_error_handler();

		/* Release the block back to the pool.  */
		status = tx_byte_release(pointer);
	    if (status != TX_SUCCESS) tx_test_error_handler();
    }

}

int     tx_test_sum_many_params(
			void (*f)(),
			int p1, int p2, int p3, int p4, int p5, int p6, int p7, int p8,
			int p9, int p10, int p11, int p12, int p13, int p14, int p15, int p16,
			int p17, int p18, int p19, int p20, int p21, int p22, int p23, int p24,
			int p25, int p26, int p27, int p28, int p29, int p30, int p31, int p32,
			int p33, int p34, int p35, int p36, int p37, int p38, int p39, int p40,
			int p41, int p42, int p43, int p44, int p45, int p46, int p47, int p48,
			int p49, int p50, int p51, int p52, int p53, int p54, int p55, int p56,
			int p57, int p58, int p59, int p60, int p61, int p62, int p63, int p64,
			int p65, int p66, int p67, int p68, int p69, int p70, int p71, int p72)
{
	volatile int a,b;

	a =
			p1 + p2 + p3 + p4 + p5 + p6 + p7 + p8 +
			p9 + p10 + p11 + p12 + p13 + p14 + p15 + p16 +
			p17 + p18 + p19 + p20 + p21 + p22 + p23 + p24 +
			p25 + p26 + p27 + p28 + p29 + p30 + p31 + p32 +
			p33 + p34 + p35 + p36 + p37 + p38 + p39 + p40 +
			p41 + p42 + p43 + p44 + p45 + p46 + p47 + p48 +
			p49 + p50 + p51 + p52 + p53 + p54 + p55 + p56 +
			p57 + p58 + p59 + p60 + p61 + p62 + p63 + p64 +
			p65 + p66 + p67 + p68 + p69 + p70 + p71 + p72;

	f();

	b =
			p1 + p2 + p3 + p4 + p5 + p6 + p7 + p8 +
			p9 + p10 + p11 + p12 + p13 + p14 + p15 + p16 +
			p17 + p18 + p19 + p20 + p21 + p22 + p23 + p24 +
			p25 + p26 + p27 + p28 + p29 + p30 + p31 + p32 +
			p33 + p34 + p35 + p36 + p37 + p38 + p39 + p40 +
			p41 + p42 + p43 + p44 + p45 + p46 + p47 + p48 +
			p49 + p50 + p51 + p52 + p53 + p54 + p55 + p56 +
			p57 + p58 + p59 + p60 + p61 + p62 + p63 + p64 +
			p65 + p66 + p67 + p68 + p69 + p70 + p71 + p72;

	if (a != b)
	{
		tx_test_error_handler();
	}

	return a;
}

int     tx_test_xor_many_params(
			void (*f)(),
			int p1, int p2, int p3, int p4, int p5, int p6, int p7, int p8,
			int p9, int p10, int p11, int p12, int p13, int p14, int p15, int p16,
			int p17, int p18, int p19, int p20, int p21, int p22, int p23, int p24,
			int p25, int p26, int p27, int p28, int p29, int p30, int p31, int p32,
			int p33, int p34, int p35, int p36, int p37, int p38, int p39, int p40,
			int p41, int p42, int p43, int p44, int p45, int p46, int p47, int p48,
			int p49, int p50, int p51, int p52, int p53, int p54, int p55, int p56,
			int p57, int p58, int p59, int p60, int p61, int p62, int p63, int p64,
			int p65, int p66, int p67, int p68, int p69, int p70, int p71, int p72)
{
	volatile int a,b;

	a =
			p1 ^ p2 ^ p3 ^ p4 ^ p5 ^ p6 ^ p7 ^ p8 ^
			p9 ^ p10 ^ p11 ^ p12 ^ p13 ^ p14 ^ p15 ^ p16 ^
			p17 ^ p18 ^ p19 ^ p20 ^ p21 ^ p22 ^ p23 ^ p24 ^
			p25 ^ p26 ^ p27 ^ p28 ^ p29 ^ p30 ^ p31 ^ p32 ^
			p33 ^ p34 ^ p35 ^ p36 ^ p37 ^ p38 ^ p39 ^ p40 ^
			p41 ^ p42 ^ p43 ^ p44 ^ p45 ^ p46 ^ p47 ^ p48 ^
			p49 ^ p50 ^ p51 ^ p52 ^ p53 ^ p54 ^ p55 ^ p56 ^
			p57 ^ p58 ^ p59 ^ p60 ^ p61 ^ p62 ^ p63 ^ p64 ^
			p65 ^ p66 ^ p67 ^ p68 ^ p69 ^ p70 ^ p71 ^ p72;

	f();

	b =
			p1 ^ p2 ^ p3 ^ p4 ^ p5 ^ p6 ^ p7 ^ p8 ^
			p9 ^ p10 ^ p11 ^ p12 ^ p13 ^ p14 ^ p15 ^ p16 ^
			p17 ^ p18 ^ p19 ^ p20 ^ p21 ^ p22 ^ p23 ^ p24 ^
			p25 ^ p26 ^ p27 ^ p28 ^ p29 ^ p30 ^ p31 ^ p32 ^
			p33 ^ p34 ^ p35 ^ p36 ^ p37 ^ p38 ^ p39 ^ p40 ^
			p41 ^ p42 ^ p43 ^ p44 ^ p45 ^ p46 ^ p47 ^ p48 ^
			p49 ^ p50 ^ p51 ^ p52 ^ p53 ^ p54 ^ p55 ^ p56 ^
			p57 ^ p58 ^ p59 ^ p60 ^ p61 ^ p62 ^ p63 ^ p64 ^
			p65 ^ p66 ^ p67 ^ p68 ^ p69 ^ p70 ^ p71 ^ p72;

	if (a != b)
	{
		tx_test_error_handler();
	}

	return a;
}



/* Define the test threads.  */

void    tx_test_thread_0_entry(ULONG thread_input)
{

UINT    status;


    /* This thread simply sits in while-forever-sleep loop.  */
    while(1)
    {
    	volatile int a, b, c, d;

		int p1; int p2; int p3; int p4; int p5; int p6; int p7; int p8;
		int p9; int p10; int p11; int p12; int p13; int p14; int p15; int p16;
		int p17; int p18; int p19; int p20; int p21; int p22; int p23; int p24;
		int p25; int p26; int p27; int p28; int p29; int p30; int p31; int p32;
		int p33; int p34; int p35; int p36; int p37; int p38; int p39; int p40;
		int p41; int p42; int p43; int p44; int p45; int p46; int p47; int p48;
		int p49; int p50; int p51; int p52; int p53; int p54; int p55; int p56;
		int p57; int p58; int p59; int p60; int p61; int p62; int p63; int p64;
		int p65; int p66; int p67; int p68; int p69; int p70; int p71; int p72;

		p1 = 1; p2 = 2; p3 = 3; p4 = 4; p5 = 5; p6 = 6; p7 = 7; p8 = 8;
		p9 = 9; p10 = 10; p11 = 11; p12 = 12; p13 = 13; p14 = 14; p15 = 15; p16 = 16;
		p17 = 17; p18 = 18; p19 = 19; p20 = 20; p21 = 21; p22 = 22; p23 = 23; p24 = 24;
		p25 = 25; p26 = 26; p27 = 27; p28 = 28; p29 = 29; p30 = 30; p31 = 31; p32 = 32;
		p33 = 33; p34 = 34; p35 = 35; p36 = 36; p37 = 37; p38 = 38; p39 = 39; p40 = 40;
		p41 = 41; p42 = 42; p43 = 43; p44 = 44; p45 = 45; p46 = 46; p47 = 47; p48 = 48;
		p49 = 49; p50 = 50; p51 = 51; p52 = 52; p53 = 53; p54 = 54; p55 = 55; p56 = 56;
		p57 = 57; p58 = 58; p59 = 59; p60 = 60; p61 = 61; p62 = 62; p63 = 63; p64 = 64;
		p65 = 65; p66 = 66; p67 = 67; p68 = 68; p69 = 69; p70 = 70; p71 = 71; p72 = 72;

    	a = tx_test_sum_many_params(
    			tx_test_sleep,
				1, 2, 3, 4, 5, 6, 7, 8,
				9, 10, 11, 12, 13, 14, 15, 16,
				17, 18, 19, 20, 21, 22, 23, 24,
				25, 26, 27, 28, 29, 30, 31, 32,
				33, 34, 35, 36, 37, 38, 39, 40,
				41, 42, 43, 44, 45, 46, 47, 48,
				49, 50, 51, 52, 53, 54, 55, 56,
				57, 58, 59, 60, 61, 62, 63, 64,
				65, 66, 67, 68, 69, 70, 71, 72);

    	b = tx_test_xor_many_params(
    			tx_test_sleep,
				1, 2, 3, 4, 5, 6, 7, 8,
				9, 10, 11, 12, 13, 14, 15, 16,
				17, 18, 19, 20, 21, 22, 23, 24,
				25, 26, 27, 28, 29, 30, 31, 32,
				33, 34, 35, 36, 37, 38, 39, 40,
				41, 42, 43, 44, 45, 46, 47, 48,
				49, 50, 51, 52, 53, 54, 55, 56,
				57, 58, 59, 60, 61, 62, 63, 64,
				65, 66, 67, 68, 69, 70, 71, 72);

        /* Increment the thread counter.  */
        tx_test_thread_0_counter++;

        /* Sleep for 10 ticks.  */
        tx_thread_sleep(10);

    	c = tx_test_sum_many_params(
    			tx_test_sleep,
				p1, p2, p3, p4, p5, p6, p7, p8,
				p9, p10, p11, p12, p13, p14, p15, p16,
				p17, p18, p19, p20, p21, p22, p23, p24,
				p25, p26, p27, p28, p29, p30, p31, p32,
				p33, p34, p35, p36, p37, p38, p39, p40,
				p41, p42, p43, p44, p45, p46, p47, p48,
				p49, p50, p51, p52, p53, p54, p55, p56,
				p57, p58, p59, p60, p61, p62, p63, p64,
				p65, p66, p67, p68, p69, p70, p71, p72);

    	d = tx_test_xor_many_params(
    			tx_test_sleep,
				p1, p2, p3, p4, p5, p6, p7, p8,
				p9, p10, p11, p12, p13, p14, p15, p16,
				p17, p18, p19, p20, p21, p22, p23, p24,
				p25, p26, p27, p28, p29, p30, p31, p32,
				p33, p34, p35, p36, p37, p38, p39, p40,
				p41, p42, p43, p44, p45, p46, p47, p48,
				p49, p50, p51, p52, p53, p54, p55, p56,
				p57, p58, p59, p60, p61, p62, p63, p64,
				p65, p66, p67, p68, p69, p70, p71, p72);

    	if (a != c)
    	{
    		tx_test_error_handler();
    	}

    	if (b != d)
    	{
    		tx_test_error_handler();
    	}

        /* Set event flag 0 to wakeup thread 5.  */
        status =  tx_event_flags_set(&tx_test_event_flags_0, 0x1, TX_OR);

        /* Check status.  */
        if (status != TX_SUCCESS)
            break;
    }
}


void    tx_test_thread_1_entry(ULONG thread_input)
{

UINT    status;


    /* This thread simply sends messages to a queue shared by thread 2.  */
    while(1)
    {

        /* Increment the thread counter.  */
    	tx_test_thread_1_counter++;

        /* Send message to queue 0.  */
        status =  tx_queue_send(&tx_test_queue_0, &tx_test_thread_1_counter, TX_WAIT_FOREVER);

        /* Check completion status.  */
        if (status != TX_SUCCESS)
            break;
    }
}


void    tx_test_thread_2_entry(ULONG thread_input)
{

ULONG   received_message;
UINT    status;

    /* This thread retrieves messages placed on the queue by thread 1.  */
    while(1)
    {

        /* Increment the thread counter.  */
    	tx_test_thread_2_counter++;

        /* Retrieve a message from the queue.  */
        status = tx_queue_receive(&tx_test_queue_0, &received_message, TX_WAIT_FOREVER);

        /* Check completion status and make sure the message is what we 
           expected.  */
        if ((status != TX_SUCCESS) || (received_message != tx_test_thread_1_counter))
            break;
    }
}


void    tx_test_thread_3_and_4_entry(ULONG thread_input)
{

UINT    status;


    /* This function is executed from thread 3 and thread 4.  As the loop
       below shows, these function compete for ownership of semaphore_0.  */
    while(1)
    {

        /* Get the semaphore with suspension.  */
        status =  tx_semaphore_get(&tx_test_semaphore_0, TX_WAIT_FOREVER);

        /* Check status.  */
        if (status != TX_SUCCESS)
            break;

        /* Sleep for 2 ticks to hold the semaphore.  */
        tx_thread_sleep(2);

        /* Release the semaphore.  */
        status =  tx_semaphore_put(&tx_test_semaphore_0);

        /* Check status.  */
        if (status != TX_SUCCESS)
            break;
    }
}


void    tx_test_thread_5_entry(ULONG thread_input)
{

UINT    status;
ULONG   actual_flags;


    /* This thread simply waits for an event in a forever loop.  */
    while(1)
    {

        /* Wait for event flag 0.  */
        status =  tx_event_flags_get(&tx_test_event_flags_0, 0x1, TX_OR_CLEAR,
                                                &actual_flags, TX_WAIT_FOREVER);

        /* Check status.  */
        if ((status != TX_SUCCESS) || (actual_flags != 0x1))
            break;
    }
}


void    tx_test_thread_6_and_7_entry(ULONG thread_input)
{

UINT    status;


    /* This function is executed from thread 6 and thread 7.  As the loop
       below shows, these function compete for ownership of mutex_0.  */
    while(1)
    {

        /* Get the mutex with suspension.  */
        status =  tx_mutex_get(&tx_test_mutex_0, TX_WAIT_FOREVER);

        /* Check status.  */
        if (status != TX_SUCCESS)
            break;

        /* Get the mutex again with suspension.  This shows
           that an owning thread may retrieve the mutex it
           owns multiple times.  */
        status =  tx_mutex_get(&tx_test_mutex_0, TX_WAIT_FOREVER);

        /* Check status.  */
        if (status != TX_SUCCESS)
            break;

        /* Sleep for 2 ticks to hold the mutex.  */
        tx_thread_sleep(2);

        /* Release the mutex.  */
        status =  tx_mutex_put(&tx_test_mutex_0);

        /* Check status.  */
        if (status != TX_SUCCESS)
            break;

        /* Release the mutex again.  This will actually 
           release ownership since it was obtained twice.  */
        status =  tx_mutex_put(&tx_test_mutex_0);

        /* Check status.  */
        if (status != TX_SUCCESS)
            break;
    }
}
