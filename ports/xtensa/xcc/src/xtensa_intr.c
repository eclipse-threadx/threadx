/*******************************************************************************
Copyright (c) 2006-2019 Cadence Design Systems Inc.

Permission is hereby granted, free of charge, to any person obtaining
a copy of this software and associated documentation files (the
"Software"), to deal in the Software without restriction, including
without limitation the rights to use, copy, modify, merge, publish,
distribute, sublicense, and/or sell copies of the Software, and to
permit persons to whom the Software is furnished to do so, subject to
the following conditions:

The above copyright notice and this permission notice shall be included
in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
******************************************************************************/

/******************************************************************************
  Xtensa-specific interrupt and exception functions for RTOS ports.
  Also see xtensa_intr_asm.S.
******************************************************************************/

#include <stdlib.h>

#include <xtensa/config/core.h>
#include <xtensa/core-macros.h>

#include "xtensa_api.h"


#if XCHAL_HAVE_EXCEPTIONS

/* Handler table is in xtensa_intr_asm.S */

extern xt_exc_handler _xt_exception_table[XCHAL_EXCCAUSE_NUM];


/*
  Default handler for unhandled exceptions.
*/
void xt_unhandled_exception(XtExcFrame *frame)
{
    (void) frame; /* Keep compiler happy */
    exit(-1);
}


/*
  This function registers a handler for the specified exception.
  The function returns the address of the previous handler.
  On error, it returns 0.
*/
xt_exc_handler xt_set_exception_handler(uint32_t n, xt_exc_handler f)
{
    xt_exc_handler old;

    if (n >= XCHAL_EXCCAUSE_NUM) {
        return 0;       /* invalid exception number */
    }

    old = _xt_exception_table[n];

    if (f != NULL) {
        _xt_exception_table[n] = f;
    }
    else {
        _xt_exception_table[n] = &xt_unhandled_exception;
    }

    return old;
}

#endif

#if XCHAL_HAVE_INTERRUPTS

#if XCHAL_HAVE_XEA2
/* Defined in xtensa_intr_asm.S */
extern uint32_t _xt_intenable;
extern uint32_t _xt_vpri_mask;
#endif

/* Handler table is in xtensa_intr_asm.S */

typedef struct xt_handler_table_entry {
    void * handler;
    void * arg;
} xt_handler_table_entry;

#if (XT_USE_INT_WRAPPER || XCHAL_HAVE_XEA3)
extern xt_handler_table_entry _xt_interrupt_table[XCHAL_NUM_INTERRUPTS + 1];
#else
extern xt_handler_table_entry _xt_interrupt_table[XCHAL_NUM_INTERRUPTS];
#endif


/*
  Default handler for unhandled interrupts.
*/
void xt_unhandled_interrupt(void * arg)
{
    (void) arg; /* Keep compiler happy */
    exit(-1);
}


/*
  This function registers a handler for the specified interrupt. The "arg"
  parameter specifies the argument to be passed to the handler when it is
  invoked. The function returns the address of the previous handler.
  On error, it returns 0.
*/
xt_handler xt_set_interrupt_handler(uint32_t n, xt_handler f, void * arg)
{
    xt_handler_table_entry * entry;
    xt_handler               old;

    if (n >= XCHAL_NUM_INTERRUPTS) {
        return 0;       /* invalid interrupt number */
    }

#if XCHAL_HAVE_XEA2
    if (Xthal_intlevel[n] > XCHAL_EXCM_LEVEL) {
        return 0;       /* priority level too high to safely handle in C */
    }
#endif

#if (XT_USE_INT_WRAPPER || XCHAL_HAVE_XEA3)
    entry = _xt_interrupt_table + n + 1;
#else
    entry = _xt_interrupt_table + n;
#endif
    old   = entry->handler;

    if (f != NULL) {
        entry->handler = f;
        entry->arg     = arg;
    }
    else {
        entry->handler = &xt_unhandled_interrupt;
        entry->arg     = (void*)n;
    }

    return old;
}


/*
  This function enables the interrupt whose number is specified as
  the argument.
*/
void xt_interrupt_enable(uint32_t intnum)
{
#if XCHAL_HAVE_XEA2
    uint32_t ps = XT_RSIL(15);

    // New INTENABLE = (_xt_intenable | mask) & _xt_vpri_mask.
    _xt_intenable |= (1 << intnum);
    XT_WSR_INTENABLE(_xt_intenable & _xt_vpri_mask);
    XT_WSR_PS(ps);
    XT_RSYNC();
#else
    xthal_interrupt_enable(intnum);
#endif
}


/*
  This function disables the interrupt whose number is specified as
  the argument.
*/
void xt_interrupt_disable(uint32_t intnum)
{
#if XCHAL_HAVE_XEA2
    uint32_t ps = XT_RSIL(15);

    // New INTENABLE = (_xt_intenable & ~mask) & _xt_vpri_mask.
    _xt_intenable &= ~(1 << intnum);
    XT_WSR_INTENABLE(_xt_intenable & _xt_vpri_mask);
    XT_WSR_PS(ps);
    XT_RSYNC();
#else
    xthal_interrupt_disable(intnum);
#endif
}


/*
  This function triggers the specified interrupt.
*/
void xt_interrupt_trigger(uint32_t intnum)
{
    xthal_interrupt_trigger(intnum);
}


/*
  This function clears the specified interrupt.
*/
void xt_interrupt_clear(uint32_t intnum)
{
    xthal_interrupt_clear(intnum);
}


#endif /* XCHAL_HAVE_INTERRUPTS */

