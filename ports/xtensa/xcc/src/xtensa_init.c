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
/*    Xtensa initialization routines.                                     */
/*                                                                        */
/*  RELEASE HISTORY                                                       */
/*                                                                        */
/*    DATE              NAME                      DESCRIPTION             */
/*                                                                        */
/*  12-31-2020     Cadence Design Systems   Initial Version 6.1.3         */
/*                                                                        */
/**************************************************************************/


#ifdef XT_BOARD
#include    <xtensa/xtbsp.h>
#endif

#include    "xtensa_rtos.h"


#ifdef XT_RTOS_TIMER_INT
#ifndef XT_CLOCK_FREQ

uint32_t _xt_tick_divisor = 0;  /* cached number of cycles per tick */

/*
Compute and initialize at run-time the tick divisor (the number of 
processor clock cycles in an RTOS tick, used to set the tick timer).
Called when the processor clock frequency is not known at compile-time.
*/
void _xt_tick_divisor_init(void)
{
    #ifdef XT_BOARD
    _xt_tick_divisor = xtbsp_clock_freq_hz() / XT_TICK_PER_SEC;
    #else
    #error "No way to obtain processor clock frequency"
    #endif  /* XT_BOARD */
}

#endif /* XT_CLOCK_FREQ */
#endif /* XT_RTOS_TIMER_INT */

