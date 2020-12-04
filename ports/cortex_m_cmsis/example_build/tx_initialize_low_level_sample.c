/**************************************************************************/

/*                                                                        */
/*       Copyright (c) Microsoft Corporation. All rights reserved.        */
/*                                                                        */
/*       This software is licensed under the Microsoft Software License   */
/*       Terms for Microsoft Azure RTOS. Full text of the license can be  */
/*       found in the LICENSE file at https://aka.ms/AzureRTOS_EULA       */
/*       and in the root directory of this software.                      */
/*                                                                        */
/**************************************************************************/

/**************************************************************************/
/**************************************************************************/
/**                                                                       */
/** ThreadX Component                                                     */
/**                                                                       */
/**   Initialize                                                          */
/**                                                                       */
/**************************************************************************/
/**************************************************************************/

#define TX_SOURCE_CODE

/* Include necessary system files.  */

#include "tx_api.h"
#include "tx_initialize.h"
#include "tx_thread.h"
#include "tx_timer.h"

/* SysTick must not be higher priority (lower numerical value) than maximum
 * ThreadX interrupt priority. */

#if TX_PORT_CFG_SYSTICK_IPL < TX_PORT_MAX_IPL
 #undef TX_PORT_CFG_SYSTICK_IPL
 #define TX_PORT_CFG_SYSTICK_IPL    TX_PORT_MAX_IPL
#endif

/* Define the location of the begining of the free RAM  */

#if defined(__ARMCC_VERSION)           /* AC6 compiler */
extern uint32_t Image$$RAM_END$$Limit;
 #define TX_FREE_MEMORY_START    &Image$$RAM_END$$Limit
#elif   defined(__GNUC__)              /* GCC compiler */
extern void * __RAM_segment_used_end__;
 #define TX_FREE_MEMORY_START    (&__RAM_segment_used_end__)
#elif defined(__ICCARM__)              /* IAR compiler */
extern void * __tx_free_memory_start;
 #define TX_FREE_MEMORY_START    (&__tx_free_memory_start)

/* __tx_free_memory_start must be placed at the end of RAM in section FREE_MEM. */
 #pragma section="FREE_MEM"
__root void * __tx_free_memory_start @"FREE_MEM";
#endif

extern void * __Vectors[];
#define TX_VECTOR_TABLE    __Vectors

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _tx_initialize_low_level                          Cortex-M/CMSIS    */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function is responsible for any low-level processor            */
/*    initialization, including setting up interrupt vectors, setting     */
/*    up a periodic timer interrupt source, saving the system stack       */
/*    pointer for use in ISR processing later, and finding the first      */
/*    available RAM memory address for tx_application_define.             */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    None                                                                */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    None                                                                */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    None                                                                */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    _tx_initialize_kernel_enter           ThreadX entry function        */
/*                                                                        */
/**************************************************************************/

VOID _tx_initialize_low_level (VOID)
{
    /* Ensure that interrupts are disabled.  */
#ifdef TX_PORT_USE_BASEPRI
    __set_BASEPRI(TX_PORT_MAX_IPL << (8U - __NVIC_PRIO_BITS));
#else
    __disable_irq();
#endif

    /* Set base of available memory to end of non-initialized RAM area.  */
    _tx_initialize_unused_memory = TX_UCHAR_POINTER_ADD(TX_FREE_MEMORY_START, 4);

    /* Set system stack pointer from vector value.  */
    _tx_thread_system_stack_ptr = TX_VECTOR_TABLE[0];

#if defined(__ARM_ARCH_7EM__) || defined(__ARM_ARCH_8M_MAIN__)

    /* Enable the cycle count register.  */
    DWT->CTRL |= (uint32_t) DWT_CTRL_CYCCNTENA_Msk;
#endif

#ifndef TX_NO_TIMER

    /* Configure SysTick based on user configuration (100 Hz by default).  */
    extern uint32_t SystemCoreClock;
    SysTick_Config(SystemCoreClock / TX_TIMER_TICKS_PER_SECOND);
    NVIC_SetPriority(SysTick_IRQn, TX_PORT_CFG_SYSTICK_IPL); // Set User configured Priority for Systick Interrupt
#endif

    /* Configure the handler priorities. */
    NVIC_SetPriority(SVCall_IRQn, UINT8_MAX);                // Note: SVC must be lowest priority, which is 0xFF
    NVIC_SetPriority(PendSV_IRQn, UINT8_MAX);                // Note: PnSV must be lowest priority, which is 0xFF
}
