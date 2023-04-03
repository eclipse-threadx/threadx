/* This test is designed to test trace functionality in ThreadX.  */

#include   <stdio.h>
#include   "tx_api.h"
#include   "tx_thread.h"
#define TX_SOURCE_CODE
#include   "tx_trace.h"

//#define FILE_DUMP

#ifdef __linux__
#define TX_WIN32_MEMORY_SIZE TX_LINUX_MEMORY_SIZE
#define _tx_win32_critical_section              _tx_linux_mutex
#define tx_win32_critical_section_nested_count  tx_linux_mutex_nested_count
#endif /* __linux__ */

/* Define the ISR dispatch.  */

extern VOID    (*test_isr_dispatch)(void);

extern UINT    _tx_trace_interrupt_control(UINT new_posture);
extern VOID    _tx_trace_object_register(UCHAR object_type, VOID *object_ptr, CHAR *object_name, ULONG parameter_1, ULONG parameter_2);
extern VOID    _tx_trace_object_unregister(VOID *object_ptr);

static unsigned long   thread_0_counter =  0;
static unsigned long   thread_1_counter =  0;
static unsigned long   thread_2_counter =  0;
static TX_THREAD       thread_0;
static TX_THREAD       thread_1;
static TX_THREAD       thread_2;

static TX_TIMER        timer_0;

static TX_BLOCK_POOL            block_pool_0;
static TX_BYTE_POOL             byte_pool_0;
static TX_EVENT_FLAGS_GROUP     group_0;
static TX_MUTEX                 mutex_0;
static TX_QUEUE                 queue_0;
static TX_SEMAPHORE             semaphore_0;

/* Define a bunch of semaphores for loading up the trace buffer.  */

static TX_SEMAPHORE             semaphore_1;
static TX_SEMAPHORE             semaphore_2;
static TX_SEMAPHORE             semaphore_3;
static TX_SEMAPHORE             semaphore_4;
static TX_SEMAPHORE             semaphore_5;
static TX_SEMAPHORE             semaphore_6;
static TX_SEMAPHORE             semaphore_7;
static TX_SEMAPHORE             semaphore_8;
static TX_SEMAPHORE             semaphore_9;
static TX_SEMAPHORE             semaphore_10;
static TX_SEMAPHORE             semaphore_11;
static TX_SEMAPHORE             semaphore_12;
static TX_SEMAPHORE             semaphore_13;
static TX_SEMAPHORE             semaphore_14;
static TX_SEMAPHORE             semaphore_15;
static TX_SEMAPHORE             semaphore_16;


static unsigned long error =  0;
static unsigned long full_buffer =  0;
static void          *save_pointer;

#if TX_WIN32_MEMORY_SIZE

static FILE         *trace_dump_file;
static CHAR         trace_dump_file_name[]= "tracedump000.trx";

#endif


/* Define task prototypes.  */

static void    thread_0_entry(ULONG task_input);
static void    thread_1_entry(ULONG task_input);
static void    thread_2_entry(ULONG task_input);


/* Define the trace buffer.   */
UCHAR   trace_buffer[16384];



/* Prototype for test control return.  */

void  test_control_return(UINT status);
UINT    _tx_thread_interrupt_control(UINT new_posture);


void    trace_dump(void)
{

#if FILE_DUMP

/* Win32 automatic dump!  */
UINT    old_interrupt;


    /* Disable interrupts.  */
    old_interrupt =  _tx_thread_smp_protect();

    /* If win32, we can actually dump the file!  */
    trace_dump_file =  fopen(trace_dump_file_name, "wb+");
    
    fwrite(trace_buffer, 1, sizeof(trace_buffer), trace_dump_file);
    
    fclose(trace_dump_file);

    /* Restore interrupts.  */
    _tx_thread_smp_unprotect(old_interrupt);

    /* Prepare for next file dump.  */
    trace_dump_file_name[11]++;
    if (trace_dump_file_name[11] > '9')
    {
        trace_dump_file_name[11] =  '0';
        trace_dump_file_name[10]++;
        
        if (trace_dump_file_name[10] > '9')
        {
            trace_dump_file_name[10] =  '0';
            trace_dump_file_name[9]++;

            if (trace_dump_file_name[9] > '9')
                trace_dump_file_name[9] =  '0';
        }
    }

#endif

}


/* Define the timer for this test.  */

static void    timer_entry(ULONG i)
{

    /* Resume thread 1.  */
    tx_thread_resume(&thread_1);
}

/* Define the ISR dispatch routine.  */

static void    test_isr(void)
{

    if ((_tx_thread_smp_protection.tx_thread_smp_protect_in_force != 1) ||
        (_tx_win32_critical_section.tx_win32_critical_section_nested_count > 1))
    {

        /* Make ISR entry event.  */
        tx_trace_isr_enter_insert(2);
    }
    else
    {

        /* Make ISR entry event.  */
        tx_trace_isr_enter_insert(1);
    } 
       
    /* Resume thread 2.  */
    tx_thread_resume(&thread_2);

    /* Make ISR entry event.  */
    tx_trace_isr_exit_insert(1);

    /* Make ISR entry event.  */
    tx_trace_isr_enter_insert(0);

    /* Make ISR entry event.  */
    tx_trace_isr_exit_insert(0);
    
}


/* Define the trace buffer full notify function.  */

static VOID    trace_buffer_full(void *ptr)
{

    /* Dump trace file!  */
    trace_dump();

    full_buffer++;
}

/* Define what the initial system looks like.  */

#ifdef CTEST
void test_application_define(void *first_unused_memory)
#else
void    threadx_trace_basic_application_define(void *first_unused_memory)
#endif
{

INT     status;
CHAR    *pointer;


    /* Setup a pointer.  */
    pointer =  (CHAR *) first_unused_memory;
    
    /* Adjust it forward just to make sure there is some space for the test below.  */
    pointer =  pointer + 200;

    /* Create a bunch of objects before being enabled.   */

    /* Create a timer for the test.  */
    save_pointer =  (void *) pointer;
    tx_timer_create(&timer_0, "timer 0", timer_entry, 0, 2, 2, TX_AUTO_ACTIVATE);
    tx_block_pool_create(&block_pool_0, "block pool 0", 20, pointer, 100);
    pointer = pointer + 100;
    tx_byte_pool_create(&byte_pool_0, "byte pool 0", pointer, 1000);
    pointer =  pointer + 1000;
    tx_event_flags_create(&group_0, "event flags group 0");
    tx_mutex_create(&mutex_0, "mutex 0", TX_NO_INHERIT);
    tx_queue_create(&queue_0, "queue 0", 16, pointer, 400);
    pointer =  pointer + 400;
    tx_semaphore_create(&semaphore_0, "semaphore 0", 1);

    /* Enable event tracing.   */
    _tx_trace_initialize();
    status =  tx_trace_enable(trace_buffer, sizeof(trace_buffer), 16);

#ifdef TX_ENABLE_EVENT_TRACE

    /* Check status.  */
    if (status != TX_SUCCESS)
    {
    
        printf("Running Trace Basic Test............................................ ERROR #1\n");
        test_control_return(1);
    }
#else

    /* Check status.  */
    if (status != TX_FEATURE_NOT_ENABLED)
    {
    
        printf("Running Trace Basic Test............................................ ERROR #2\n");
        test_control_return(1);
    }
#endif

    /* Put system definition stuff in here, e.g. thread creates and other assorted
       create information.  */
    
    status =  tx_thread_create(&thread_0, "thread 0", thread_0_entry, 0,  
            pointer, TEST_STACK_SIZE_PRINTF, 
            16, 16, TX_NO_TIME_SLICE, TX_AUTO_START);
    pointer =  pointer + TEST_STACK_SIZE_PRINTF;

    /* Check for status.  */
    if (status != TX_SUCCESS)
    {

        printf("Running Trace Basic Test............................................ ERROR #3\n");
        test_control_return(1);
    }

    status =  tx_thread_create(&thread_1, "thread 1", thread_1_entry, 1,  
            pointer, TEST_STACK_SIZE_PRINTF, 
            15, 15, TX_NO_TIME_SLICE, TX_DONT_START);
    pointer =  pointer + TEST_STACK_SIZE_PRINTF;

    /* Check for status.  */
    if (status != TX_SUCCESS)
    {

        printf("Running Trace Basic Test............................................ ERROR #4\n");
        test_control_return(1);
    }

    status =  tx_thread_create(&thread_2, "thread 2", thread_2_entry, 2,  
            pointer, TEST_STACK_SIZE_PRINTF, 
            14, 14, TX_NO_TIME_SLICE, TX_DONT_START);
    pointer =  pointer + TEST_STACK_SIZE_PRINTF;

    /* Check for status.  */
    if (status != TX_SUCCESS)
    {

        printf("Running Trace Basic Test............................................ ERROR #5\n");
        test_control_return(1);
    }
}



/* Define the test threads.  */

static void    thread_0_entry(ULONG thread_input)
{

UINT    status;
UINT    old_interrupt;
CHAR    *pointer;
TX_INTERRUPT_SAVE_AREA
ULONG   object;

    /* Coverage for build without TraceX enabled.  */
    tx_interrupt_save =  _tx_trace_interrupt_control(TX_INT_DISABLE);
    _tx_trace_interrupt_control(tx_interrupt_save);
#ifndef TX_ENABLE_EVENT_TRACE
    _tx_trace_object_register(0, TX_NULL, TX_NULL, 0, 0);
    _tx_trace_object_register(1, TX_NULL, TX_NULL, 0, 0);
    _tx_trace_object_register(1, &object, TX_NULL, 0, 0);
    _tx_trace_object_register(1, &object, "fake name", 0, 0);
    _tx_trace_object_register(1, &object, "fake name", 1, 0);
#endif
    _tx_trace_object_register(1, &object, "fake name", 1, 1);
    _tx_trace_object_unregister(TX_NULL);
    _tx_trace_object_unregister(&object);


    /* Increment thread 0 counter.  */
    thread_0_counter++;

    /* Inform user of success getting to this test.  */
    printf("Running Trace Basic Test............................................ ");

    /* Delete the additional objects.  */
    tx_timer_delete(&timer_0);
    tx_block_pool_delete(&block_pool_0);
    tx_byte_pool_delete(&byte_pool_0);
    tx_event_flags_delete(&group_0);
    tx_mutex_delete(&mutex_0);
    tx_queue_delete(&queue_0);
    tx_semaphore_delete(&semaphore_0);

    /* Create all the semaphores.  */
    tx_semaphore_create(&semaphore_1, "semaphore 1", 1);
    tx_semaphore_create(&semaphore_2, "semaphore 2", 1);
    tx_semaphore_create(&semaphore_3, "semaphore 3", 1);
    tx_semaphore_create(&semaphore_4, "semaphore 4", 1);
    tx_semaphore_create(&semaphore_5, "semaphore 5", 1);
    tx_semaphore_create(&semaphore_6, "semaphore 6", 1);
    tx_semaphore_create(&semaphore_7, "semaphore 7", 1);
    tx_semaphore_create(&semaphore_8, "semaphore 8", 1);
    tx_semaphore_create(&semaphore_9, "semaphore 9", 1);
    tx_semaphore_create(&semaphore_10, "semaphore 10", 1);
    tx_semaphore_create(&semaphore_11, "semaphore 11", 1);
    tx_semaphore_create(&semaphore_12, "semaphore 12", 1);
    tx_semaphore_create(&semaphore_13, "semaphore 13", 1);
    tx_semaphore_create(&semaphore_14, "semaphore 14", 1);
    tx_semaphore_create(&semaphore_15, "semaphore 15", 1);
    tx_semaphore_create(&semaphore_16, "semaphore 16", 1);

    /* Delete them all.  */
    tx_semaphore_delete(&semaphore_1);
    tx_semaphore_delete(&semaphore_2);
    tx_semaphore_delete(&semaphore_3);
    tx_semaphore_delete(&semaphore_4);
    tx_semaphore_delete(&semaphore_5);
    tx_semaphore_delete(&semaphore_6);
    tx_semaphore_delete(&semaphore_7);
    tx_semaphore_delete(&semaphore_8);
    tx_semaphore_delete(&semaphore_9);
    tx_semaphore_delete(&semaphore_10);
    tx_semaphore_delete(&semaphore_11);
    tx_semaphore_delete(&semaphore_12);
    tx_semaphore_delete(&semaphore_13);
    tx_semaphore_delete(&semaphore_14);
    tx_semaphore_delete(&semaphore_15);
    tx_semaphore_delete(&semaphore_16);

    /* Create one over again.  */
    tx_semaphore_create(&semaphore_1, "semaphore 1", 1);

    /* Now, create them all again.  */
    pointer =  (CHAR *) save_pointer;
    tx_block_pool_create(&block_pool_0, "block pool 0", 20, pointer, 100);
    pointer = pointer + 100;
    tx_byte_pool_create(&byte_pool_0, "byte pool 0", pointer, 1000);
    pointer =  pointer + 1000;
    tx_event_flags_create(&group_0, "event flags group 0");
    tx_mutex_create(&mutex_0, "mutex 0", TX_NO_INHERIT);
    tx_queue_create(&queue_0, "queue 0", 16, pointer, 400);
    pointer =  pointer + 400;
    tx_semaphore_create(&semaphore_0, "semaphore 0", 1);

    /* Attempt to enable event tracing again.   */
    status =  tx_trace_enable(trace_buffer, sizeof(trace_buffer), 8);

#ifdef TX_ENABLE_EVENT_TRACE

    /* Check status.  */
    if (status != TX_NOT_DONE)
    {
    
        printf("ERROR #6\n");
        test_control_return(1);
    }
#else

    /* Check status.  */
    if (status != TX_FEATURE_NOT_ENABLED)
    {
    
        printf("ERROR #7\n");
        test_control_return(1);
    }

    /* Attempt to enable event tracing again.   */
    status =  tx_trace_enable(TX_NULL, 0, 8);

    /* Check status.  */
    if (status != TX_FEATURE_NOT_ENABLED)
    {
    
        printf("ERROR #8\n");
        test_control_return(1);
    }

    /* Attempt to enable event tracing again.   */
    status =  tx_trace_enable(TX_NULL, 1, 0);

    /* Check status.  */
    if (status != TX_FEATURE_NOT_ENABLED)
    {
    
        printf("ERROR #9\n");
        test_control_return(1);
    }

    /* Attempt to enable event tracing again.   */
    status =  tx_trace_enable(TX_NULL, 1, 8);

    /* Check status.  */
    if (status != TX_FEATURE_NOT_ENABLED)
    {
    
        printf("ERROR #10\n");
        test_control_return(1);
    }

#endif

    /* Filter all events.  */
    status =  tx_trace_event_filter(TX_TRACE_ALL_EVENTS);

#ifdef TX_ENABLE_EVENT_TRACE

    /* Check status.  */
    if (status != TX_SUCCESS)
    {

        /* Trace error.  */
        printf("ERROR #11\n");
        test_control_return(1);
    }
#else

    /* Check status.  */
    if (status != TX_FEATURE_NOT_ENABLED)
    {

        /* Trace error.  */
        printf("ERROR #12\n");
        test_control_return(1);
    }
    
    /* Filter all events.  */
    status =  tx_trace_event_filter(0);

    /* Check status.  */
    if (status != TX_FEATURE_NOT_ENABLED)
    {

        /* Trace error.  */
        printf("ERROR #13\n");
        test_control_return(1);
    }
    
#endif

    /* Unfilter all events.  */
    status =  tx_trace_event_unfilter(TX_TRACE_ALL_EVENTS);

#ifdef TX_ENABLE_EVENT_TRACE

    /* Check status.  */
    if (status != TX_SUCCESS)
    {

        /* Trace error.  */
        printf("ERROR #14\n");
        test_control_return(1);
    }
#else

    /* Check status.  */
    if (status != TX_FEATURE_NOT_ENABLED)
    {

        /* Trace error.  */
        printf("ERROR #15\n");
        test_control_return(1);
    }

    /* Unfilter all events.  */
    status =  tx_trace_event_unfilter(0);

    /* Check status.  */
    if (status != TX_FEATURE_NOT_ENABLED)
    {

        /* Trace error.  */
        printf("ERROR #16\n");
        test_control_return(1);
    }

#endif
    
    /* Register the trace buffer full notification routine.  */
    status =  tx_trace_buffer_full_notify(trace_buffer_full);

#ifdef TX_ENABLE_EVENT_TRACE

    /* Check status.  */
    if (status != TX_SUCCESS)
    {

        /* Trace error.  */
        printf("ERROR #17\n");
        test_control_return(1);
    }
#else

    /* Check status.  */
    if (status != TX_FEATURE_NOT_ENABLED)
    {

        /* Trace error.  */
        printf("ERROR #18\n");
        test_control_return(1);
    }
    
    /* Check the NULL path with trace disabled.  */
    status =  tx_trace_buffer_full_notify(TX_NULL);

    /* Check status.  */
    if (status != TX_FEATURE_NOT_ENABLED)
    {

        /* Trace error.  */
        printf("ERROR #19\n");
        test_control_return(1);
    }
    
#endif

    /* Create a timer for the test.  */
    tx_timer_create(&timer_0, "timer 0", timer_entry, 0, 2, 2, TX_AUTO_ACTIVATE);

    /* Setup the ISR.  */
    test_isr_dispatch =  test_isr;

    while (full_buffer < 40)
    {

if ((_tx_thread_smp_protection.tx_thread_smp_protect_in_force != 0) ||
    (_tx_win32_critical_section.tx_win32_critical_section_nested_count != 0))
    test_isr_dispatch =  test_isr;

        if (full_buffer < 20)
        {

            /* Disable interrupts.  */
            old_interrupt =  tx_interrupt_control(TX_INT_DISABLE);

if ((_tx_thread_smp_protection.tx_thread_smp_protect_in_force != 0) ||
    (_tx_win32_critical_section.tx_win32_critical_section_nested_count != 1))
    test_isr_dispatch =  test_isr;

			/* Restore interrupts.  */
            tx_interrupt_control(old_interrupt);

if ((_tx_thread_smp_protection.tx_thread_smp_protect_in_force != 0) ||
    (_tx_win32_critical_section.tx_win32_critical_section_nested_count != 0))
    test_isr_dispatch =  test_isr;
		}
        
        /* Insert user event.  */
        status =  tx_trace_user_event_insert(1027, 1, 2, 3, 4);

if ((_tx_thread_smp_protection.tx_thread_smp_protect_in_force != 0) ||
    (_tx_win32_critical_section.tx_win32_critical_section_nested_count != 0))
    test_isr_dispatch =  test_isr;


#ifdef TX_ENABLE_EVENT_TRACE

        /* Check status.  */
        if (status != TX_SUCCESS)
        {

            /* Trace error.  */
            printf("ERROR #20\n");
            test_control_return(1);
        }
#else

        /* Check status.  */
        if (status != TX_FEATURE_NOT_ENABLED)
        {

            /* Trace error.  */
            printf("ERROR #21\n");
            test_control_return(1);
        }

        /* Insert user event.  */
        status =  tx_trace_user_event_insert(0, 1, 2, 3, 4);

        /* Check status.  */
        if (status != TX_FEATURE_NOT_ENABLED)
        {

            /* Trace error.  */
            printf("ERROR #22\n");
            test_control_return(1);
        }

        /* Insert user event.  */
        status =  tx_trace_user_event_insert(0, 0, 2, 3, 4);

        /* Check status.  */
        if (status != TX_FEATURE_NOT_ENABLED)
        {

            /* Trace error.  */
            printf("ERROR #23\n");
            test_control_return(1);
        }

        /* Insert user event.  */
        status =  tx_trace_user_event_insert(0, 0, 0, 3, 4);

        /* Check status.  */
        if (status != TX_FEATURE_NOT_ENABLED)
        {

            /* Trace error.  */
            printf("ERROR #24\n");
            test_control_return(1);
        }

        /* Insert user event.  */
        status =  tx_trace_user_event_insert(0, 0, 0, 0, 4);

        /* Check status.  */
        if (status != TX_FEATURE_NOT_ENABLED)
        {

            /* Trace error.  */
            printf("ERROR #25\n");
            test_control_return(1);
        }

        /* Insert user event.  */
        status =  tx_trace_user_event_insert(0, 0, 0, 0, 0);

        /* Check status.  */
        if (status != TX_FEATURE_NOT_ENABLED)
        {

            /* Trace error.  */
            printf("ERROR #26\n");
            test_control_return(1);
        }

        break;
#endif
    }        

    /* Clear the ISR.  */
    test_isr_dispatch =  TX_NULL;

    /* Disable the trace buffer.  */
    status = tx_trace_disable();

#ifdef TX_ENABLE_EVENT_TRACE

    /* Check status.  */
    if (status != TX_SUCCESS)
    {

        /* Trace error.  */
        printf("ERROR #27\n");
        test_control_return(1);
    }
#else

    /* Check status.  */
    if (status != TX_FEATURE_NOT_ENABLED)
    {

        /* Trace error.  */
        printf("ERROR #28\n");
        test_control_return(1);
    }
#endif

    /* Attempt to disable again, just to get the TX_NOT_DONE error code.  */
    status =  tx_trace_disable();
    
#ifdef TX_ENABLE_EVENT_TRACE

    /* Check status.  */
    if (status != TX_NOT_DONE)
    {

        /* Trace error.  */
        printf("ERROR #29\n");
        test_control_return(1);
    }
#else

    /* Check status.  */
    if (status != TX_FEATURE_NOT_ENABLED)
    {

        /* Trace error.  */
        printf("ERROR #30\n");
        test_control_return(1);
    }
#endif
    
    /* Attempt to enable event tracing with a bogus size.   */
    status =  tx_trace_enable(trace_buffer, 1, 8);

#ifdef TX_ENABLE_EVENT_TRACE

    /* Check status.  */
    if (status != TX_SIZE_ERROR)
    {
    
        /* Trace error.  */
        printf("ERROR #31\n");
        test_control_return(1);
    }
#else

    /* Check status.  */
    if (status != TX_FEATURE_NOT_ENABLED)
    {
    
        /* Trace error.  */
        printf("ERROR #32\n");
        test_control_return(1);
    }
#endif


    /* Attempt to make an user event when tracing is not enabled.   */
    status =  tx_trace_user_event_insert(1027, 1, 2, 3, 4);

#ifdef TX_ENABLE_EVENT_TRACE

    /* Check status.  */
    if (status != TX_NOT_DONE)
    {
    
        /* Trace error.  */
        printf("ERROR #33\n");
        test_control_return(1);
    }
#else

    /* Check status.  */
    if (status != TX_FEATURE_NOT_ENABLED)
    {
    
        /* Trace error.  */
        printf("ERROR #34\n");
        test_control_return(1);
    }
#endif


    /* Check status.  */
    if (error)
    {

        /* Trace error.  */
        printf("ERROR #35\n");
        test_control_return(1);
    }
    else
    {
    
        /* Successful test.  */
        printf("SUCCESS!\n");
        test_control_return(0);
    }
}


static void    thread_1_entry(ULONG task_input)
{

    while(1)
    {
    
        thread_1_counter++;
        tx_thread_suspend(&thread_1);    
    }
}



static void    thread_2_entry(ULONG task_input)
{

    while(1)
    {
    
        thread_2_counter++;
        tx_thread_suspend(&thread_2);    
    }
}
