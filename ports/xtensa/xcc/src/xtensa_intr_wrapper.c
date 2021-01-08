/**************************************************************************/
/*   Copyright (c) Cadence Design Systems, Inc.                           */
/*                                                                        */
/* Permission is hereby granted, free of charge, to any person obtaining  */
/* a copy of this software and associated documentation files (the        */
/* "Software"), to deal in the Software without restriction, including    */
/* without limitation the rights to use, copy, modify, merge, publish,    */
/* distribute, sublicense, and/or sell copies of the Software, and to     */
/* permit persons to whom the Software is furnished to do so, subject to  */
/* the following conditions:                                              */
/*                                                                        */
/* The above copyright notice and this permission notice shall be         */
/* included in all copies or substantial portions of the Software.        */
/*                                                                        */
/* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,        */
/* EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF     */
/* MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. */
/* IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY   */
/* CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,   */
/* TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE      */
/* SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.                 */
/**************************************************************************/

/**************************************************************************/
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*  Xtensa-specific interrupt handler wrapper.                            */
/*                                                                        */
/*  RELEASE HISTORY                                                       */
/*                                                                        */
/*    DATE              NAME                      DESCRIPTION             */
/*                                                                        */
/*  12-31-2020     Cadence Design Systems   Initial Version 6.1.3         */
/*                                                                        */
/**************************************************************************/


#include <xtensa/config/core.h>
#include <xtensa/core-macros.h>

#include "xtensa_rtos.h"
#include "xtensa_api.h"

#include "tx_api.h"
#include "tx_thread.h"

#ifdef TX_ENABLE_EXECUTION_CHANGE_NOTIFY
#include "tx_execution_profile.h"
#endif


#if (XCHAL_HAVE_XEA3 && XCHAL_HAVE_INTERRUPTS)

/* Table of interrupt hooks. Used for testing ONLY. */
#ifdef XT_INTEXC_HOOKS
volatile XT_INTEXC_HOOK _xt_intexc_hooks[XT_INTEXC_HOOK_NUM];
#endif


/* Handler table is in xtensa_intr_asm.S */

typedef struct xt_handler_table_entry {
    void * handler;
    void * arg;
} xt_handler_table_entry;

extern xt_handler_table_entry _xt_interrupt_table[XCHAL_NUM_INTERRUPTS + 1];
extern int32_t xt_sw_intnum;

static int32_t xt_wflag;


/**************************************************************************/
/*    Wrapper for interrupt handlers. Argument is (intnum << 2).          */
/*    Execution comes here from the dispatch code if the wrapper is       */
/*    enabled.                                                            */
/**************************************************************************/
void
xt_interrupt_wrapper(void * arg)
{
    uint32_t                 intnum = (uint32_t)(arg) >> 2;
    xt_handler_table_entry * entry;
    xt_handler               handler;

    /* Increment interrupt nest counter. */
    _tx_thread_system_state++;

#ifdef TX_ENABLE_EXECUTION_CHANGE_NOTIFY
    /* Call the ISR enter function to indicate an ISR is executing. */
    _tx_execution_isr_enter();
#endif

    /* Load handler address and argument from table. Note that the
       first entry in the table points to this wrapper, so we have
       to skip ahead one.
     */
    entry = _xt_interrupt_table + intnum + 1;
    handler = (xt_handler) entry->handler;

    (*handler)(entry->arg);

#ifdef TX_ENABLE_EXECUTION_CHANGE_NOTIFY
    /* Call the ISR exit function to indicate an ISR is complete. */
    _tx_execution_isr_exit();
#endif

    /* If a context switch is pending, trigger the SW interrupt
       to process the switch. Set an internal flag so we don't
       trigger the sw interrupt again when handling it.
     */
    if (xt_wflag != 0) {
        xt_wflag = 0;
    }
    else if (_tx_thread_current_ptr != _tx_thread_execute_ptr) {
        xt_wflag = 1;
        xt_interrupt_trigger(xt_sw_intnum);
    }

    /* Decrement interrupt nest counter. */
    _tx_thread_system_state--;
}

#endif /* XCHAL_HAVE_XEA3 && XCHAL_HAVE_INTERRUPTS */

