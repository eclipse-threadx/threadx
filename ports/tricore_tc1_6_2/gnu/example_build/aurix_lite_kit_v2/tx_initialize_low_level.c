/***************************************************************************
 * Copyright (c) 2026 safecubed GmbH
 * Copyright (c) 2026 Eclipse ThreadX contributors
 *
 * This program and the accompanying materials are made available under the
 * terms of the MIT License which is available at
 * https://opensource.org/licenses/MIT.
 *
 * SPDX-License-Identifier: MIT
 **************************************************************************/

/* Portions of this file were generated with AI assistance. */

/* Low-level initialisation of ThreadX on CPU0 of the TC375 Lite Kit.

   _tx_initialize_low_level runs from tx_kernel_enter before the kernel and
   the application are initialised. It sets the first unused memory, the
   ThreadX timer on compare 0 of STM0 and the dispatch interrupt of the port.
   Interrupts stay disabled until _tx_thread_schedule starts the first
   thread. The linker script shall define __TX_FREE_MEM.  */

#define TX_SOURCE_CODE

#include "tx_api.h"
#include "IfxStm.h"

#define TIMER_STM                   MODULE_STM0
#define TIMER_COMPARATOR            IfxStm_Comparator_0

VOID    _tx_timer_interrupt(VOID);

#if (defined(TX_ENABLE_EXECUTION_CHANGE_NOTIFY) || defined(TX_EXECUTION_PROFILE_ENABLE))
VOID    _tx_execution_isr_enter(VOID);
VOID    _tx_execution_isr_exit(VOID);
#endif

extern VOID     *_tx_initialize_unused_memory;

/* First address of CPU0 data RAM after the variables and the heap, from the
   linker script.  */
extern UCHAR    __TX_FREE_MEM[];

/* STM ticks per ThreadX timer tick.  */
static ULONG    timer_period;


VOID   _tx_initialize_low_level(VOID)
{

    IfxStm_CompareConfig compare_config;

    __disable();

    _tx_initialize_unused_memory = (VOID *) __TX_FREE_MEM;

    timer_period = (ULONG) ((IfxStm_getFrequency(&TIMER_STM) / (float32) TX_TIMER_TICKS_PER_SECOND) + 0.5f);

    IfxStm_initCompareConfig(&compare_config);
    compare_config.comparator          = TIMER_COMPARATOR;
    compare_config.comparatorInterrupt = IfxStm_ComparatorInterrupt_ir0;
    compare_config.ticks               = timer_period;
    compare_config.triggerPriority     = TX_PORT_TRICORE_TIMER_INTERRUPT_PRIORITY;
    compare_config.typeOfService       = IfxSrc_Tos_cpu0;
    (VOID) IfxStm_initCompare(&TIMER_STM, &compare_config);

    IfxSrc_init(&TX_PORT_TRICORE_DISPATCH_SRC, IfxSrc_Tos_cpu0, TX_PORT_TRICORE_DISPATCH_PRIORITY);
    IfxSrc_enable(&TX_PORT_TRICORE_DISPATCH_SRC);
}


IFX_INTERRUPT(TimerInterruptHandler, 0, TX_PORT_TRICORE_TIMER_INTERRUPT_PRIORITY)
{

    ULONG   now;
    ULONG   next;

#if (defined(TX_ENABLE_EXECUTION_CHANGE_NOTIFY) || defined(TX_EXECUTION_PROFILE_ENABLE))
    _tx_execution_isr_enter();
#endif

    IfxStm_clearCompareFlag(&TIMER_STM, TIMER_COMPARATOR);

    /* The next compare follows the previous one by one period. If the counter
       has already passed that value, for example after the first tick was
       delayed by tx_application_define or after a debugger halt, the next
       compare is one period from now and the ticks in between are lost.
       Without this check, the next interrupt would occur only after the
       32-bit counter wraps.  */
    next = IfxStm_getCompare(&TIMER_STM, TIMER_COMPARATOR) + timer_period;
    now  = IfxStm_getLower(&TIMER_STM);
    if ((LONG) (next - now) <= 0)
    {
        next = now + timer_period;
    }
    IfxStm_updateCompare(&TIMER_STM, TIMER_COMPARATOR, next);

    _tx_timer_interrupt();

#if (defined(TX_ENABLE_EXECUTION_CHANGE_NOTIFY) || defined(TX_EXECUTION_PROFILE_ENABLE))
    _tx_execution_isr_exit();
#endif
}
