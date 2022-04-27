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


#include "tx_api.h"
#include "tx_port.h"
#include "xtensa_api.h"

#if XCHAL_HAVE_ISL || XCHAL_HAVE_KSL || XCHAL_HAVE_PSL
#include <xtensa/tie/xt_exception_dispatch.h>
#endif


#if XCHAL_HAVE_XEA3
int32_t xt_sw_intnum    = -1;
int32_t xt_timer_intnum = -1;
#endif


/**************************************************************************/
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function is responsible for any low-level processor            */
/*    initialization, including setting up interrupt vectors, setting     */
/*    up a periodic timer interrupt source, saving the system stack       */
/*    pointer for use in ISR processing later, and finding the first      */
/*    available RAM memory address for tx_application_define.             */
/*    It also sets the default heap region for the optional C library.    */
/*                                                                        */
/*  RELEASE HISTORY                                                       */
/*                                                                        */
/*    DATE              NAME                      DESCRIPTION             */
/*                                                                        */
/*  12-31-2020      Cadence Design Systems  Initial Version 6.1.3         */
/*  04-25-2022      Scott Larson            Modified comments and updated */
/*                                            function names,             */
/*                                            resulting in version 6.1.11 */
/*                                                                        */
/**************************************************************************/
VOID   _tx_initialize_low_level(VOID)
{
    extern char   _xt_interrupt_stack_top;
    extern void   _tx_timer_interrupt(void *);
    extern void * _tx_thread_system_stack_ptr;
    extern void * _tx_initialize_unused_memory;
    extern char   _end;

    #ifdef TX_THREAD_SAFE_CLIB
    extern char __stack;
    extern void _tx_clib_init(void);
    #endif

    #if XCHAL_CP_NUM > 0
    extern void _xt_coproc_init(void);
    extern void _xt_coproc_exc(XtExcFrame * fp);
    #endif

    #ifdef TX_ENABLE_STACK_CHECKING
    extern VOID _tx_xtensa_stack_error_handler(TX_THREAD * thread);
    #endif

    #if XCHAL_HAVE_XEA3
    extern void xt_sched_handler(void * arg);
    int32_t i;
    #endif

    TX_INTERRUPT_SAVE_AREA

    /* Disable interrupts - don't want any that interact with ThreadX yet. */
    TX_DISABLE

    /*
    Disable stack limit checking if present. Whatever was set up earlier
    is not going to work for us.
    */
#if XCHAL_HAVE_KSL
    XT_WSR_KSL(0);
#endif
#if XCHAL_HAVE_ISL
    XT_WSR_ISL(0);
#endif

    /* Save the system stack pointer.  */
    _tx_thread_system_stack_ptr = &(_xt_interrupt_stack_top);

    /* Save the first available memory address.  */
    _tx_initialize_unused_memory = (void *)(((UINT)&_end + 15) & ~0xF);

    #ifdef TX_THREAD_SAFE_CLIB
    /*
    Pre-allocate default memory region for the C library heap.
    Bisect the region from first available memory to end of system memory,
    align to 16 byte boundary, and allocate the heap in the upper half.
    */
    _tx_clib_heap_end   = &(__stack);
    _tx_clib_heap_start =
        (void *)(((UINT)_tx_initialize_unused_memory/2 + (UINT)_tx_clib_heap_end/2) & ~0xF);
    #endif

    #if XCHAL_CP_NUM > 0
    /*
    Initialize co-processor management for threads. Leave CPENABLE alone.
    This is called from a normal Xtensa single-threaded run-time environment
    before multi-threading has commenced. All co-processors are enabled.
    It is important NOT to clear CPENABLE yet because tx_application_define() 
    is user code which might use a co-processor. The co-processor exception 
    handler does not expect to be called outside a thread.
    */
    _xt_coproc_init();

    #if XCHAL_HAVE_XEA3
    /* Install the coprocessor exception handler. */
    xt_set_exception_handler(EXCCAUSE_CP_DISABLED, _xt_coproc_exc);
    #endif
    #endif

    #if XCHAL_HAVE_XEA3
    /* Select a software interrupt to use for scheduling. */
    for (i = 0; i < XCHAL_NUM_INTERRUPTS; i++) {
        if ((Xthal_inttype[i] == XTHAL_INTTYPE_SOFTWARE) && (Xthal_intlevel[i] == 1)) {
            xt_sw_intnum = i;
            break;
        }
    }

    if (xt_sw_intnum == -1) {
        __asm__ volatile ("break 1, 1");
    }

    /* Set the interrupt handler and enable the interrupt. */
    xt_set_interrupt_handler(xt_sw_intnum, xt_sched_handler, 0);
    xt_interrupt_enable(xt_sw_intnum);
    #endif

    #ifndef TX_NO_TIMER

    /* Compute tick divisor if clock freq is not compile-time constant. */
    #ifndef XT_CLOCK_FREQ
    xt_tick_divisor_init();
    #endif

    /* Set up the periodic tick timer (assume enough time to complete init). */
    #ifdef XT_CLOCK_FREQ
    XT_WSR_CCOMPARE(XT_RSR_CCOUNT() + XT_TICK_DIVISOR);
    #else
    XT_WSR_CCOMPARE(XT_RSR_CCOUNT() + xt_tick_divisor);
    #endif

    #if XCHAL_HAVE_XEA3
    xt_timer_intnum = XT_TIMER_INTNUM;
    xt_set_interrupt_handler(xt_timer_intnum, _tx_timer_interrupt, 0);
    #endif

    /* Enable the timer interrupt at the device level. */
    xt_interrupt_enable(XT_TIMER_INTNUM);

    #endif /* TX_NO_TIMER */

    /* Initialize C library thread safety support. */
    #ifdef TX_THREAD_SAFE_CLIB
    _tx_clib_init();
    #endif

    /* Install stack overflow notification callback. */
    #ifdef TX_ENABLE_STACK_CHECKING
    tx_thread_stack_error_notify(_tx_xtensa_stack_error_handler);
    #endif
}

