/* Small demonstration of the ThreadX module manager.  */

#include "tx_api.h"
#include "txm_module.h"
#include <stddef.h>


/* Define the ThreadX module manager thread and stack...  */

TX_THREAD               module_manager;
#define MODULE_MANAGER_STACK_SIZE (1024 * 4)
ALIGN_TYPE              module_manager_stack[MODULE_MANAGER_STACK_SIZE / sizeof(ALIGN_TYPE)];


/* Define the module instance object  */

TXM_MODULE_INSTANCE     my_module;


/* The module code should be loaded here.  */

#define MODULE_CODE (0x80180000)


/* Define the object pool area.  */

UCHAR                   object_memory[1024 * 16];


/* Define the module data pool area.  */

#define MODULE_DATA_SIZE (1024 * 32)
ALIGN_TYPE MODULE_DATA [MODULE_DATA_SIZE / sizeof(ALIGN_TYPE)];


/* Define the count of memory faults.  */

ULONG                   memory_faults = 0;


/* Define thread prototypes.  */

void    module_manager_entry(ULONG thread_input);


#ifdef TX_ENABLE_EVENT_TRACE

UCHAR   event_buffer[65536];

#endif


/* Define main entry point.  */

int main(void)
{

    /* Initialize timer for ThreadX.  */
    extern void init_timer(void);    /* in timer_interrupts.c  */
    init_timer();

    /* Enter the ThreadX kernel.  */
    tx_kernel_enter();

    /* The functions doesn't really return*/
    return 0;
}

ULONG simple_thread_counter = 0;
TX_THREAD simple_thread;
#define SIMPLE_THREAD_STACK_SIZE (1024 * 4)
ALIGN_TYPE simple_thread_stack[SIMPLE_THREAD_STACK_SIZE / sizeof(ALIGN_TYPE)];
void simple_thread_entry(ULONG thread_input)
{
    while(1)
    {
        simple_thread_counter++;
        tx_thread_sleep(2);
    }
}

/* Define what the initial system looks like.  */

void tx_application_define(void *first_unused_memory)
{
    tx_thread_create(
            &module_manager,
            "Module Manager Thread",
            module_manager_entry, 0,
            module_manager_stack, MODULE_MANAGER_STACK_SIZE,
            1, 1,
            TX_NO_TIME_SLICE,
            TX_AUTO_START);

    tx_thread_create(
            &simple_thread,
            "Simple Thread",
            simple_thread_entry, 0,
            simple_thread_stack, SIMPLE_THREAD_STACK_SIZE,
            1, 1,
            TX_NO_TIME_SLICE,
            TX_AUTO_START);
}


/* Define the test threads.  */

void module_manager_entry(ULONG thread_input)
{

    /* Initialize the module manager.   */
    txm_module_manager_initialize((void *) MODULE_DATA, MODULE_DATA_SIZE);

    txm_module_manager_object_pool_create(object_memory, sizeof(object_memory));

    /* Load the module with absolute address linkage, in this example it is placed there by the multiple image download.  */
    txm_module_manager_absolute_load(&my_module, "my module", (void *) MODULE_CODE);

    /* Start the module.  */
    txm_module_manager_start(&my_module);

    /* Sleep for a while....  */
    {
        int i;
        for (i = 0; i < 10; i++)
        {
            tx_thread_sleep(10);
        }
    }

    /* Stop the module.  */
    txm_module_manager_stop(&my_module);

    /* Unload the module.  */
    txm_module_manager_unload(&my_module);

    /* Load the module again.  */
    txm_module_manager_absolute_load(&my_module, "my module", (void *) MODULE_CODE);

    /* Start the module again.  */
    txm_module_manager_start(&my_module);

    /* Now just spin...  */
    while(1)
    {

        tx_thread_sleep(10);
    }
}
