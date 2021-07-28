
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

/**************************************************************************
    XTENSA INFORMATION FOR RTOS TICK TIMER AND CLOCK FREQUENCY

    This header contains timer related definitions and macros for use by
    Xtensa RTOS source files. It includes and uses the Xtensa hardware
    abstraction layer (HAL) to deal with config specifics.

    If the RTOS has no timer interrupt, then there is no tick timer and the
    clock frequency is irrelevant, so all of these macros are left undefined
    and the Xtensa core configuration need not have a timer.
***************************************************************************/

#ifndef XTENSA_TIMER_H
#define XTENSA_TIMER_H

#include    "xtensa_rtos.h"     /* in case this wasn't included directly */

#ifdef XT_RTOS_TIMER_INT        /* skip all this stuff if no timer int */

#ifdef __ASSEMBLER__
#include    <xtensa/coreasm.h>
#else
#include    <xtensa/tie/xt_timer.h>
#endif

#include    <xtensa/corebits.h>
#include    <xtensa/config/system.h>


#if XCHAL_HAVE_XEA3

/*
If the user has not specified a timer by defining XT_TIMER_INDEX, then
select timer 0.
*/
#ifndef XT_TIMER_INDEX
#define XT_TIMER_INDEX    0
#endif

#else /* XEA2 */

/*
Select timer to use for periodic tick, and determine its interrupt number 
and priority. User may specify a timer by defining XT_TIMER_INDEX with -D,
in which case its validity is checked (it must exist in this core and must 
not be on a high priority interrupt - an error will be reported in invalid).
Otherwise select the first low or medium priority interrupt timer available.
*/  
#ifndef XT_TIMER_INDEX
  #if XCHAL_TIMER3_INTERRUPT != XTHAL_TIMER_UNCONFIGURED
    #if XCHAL_INT_LEVEL(XCHAL_TIMER3_INTERRUPT) <= XCHAL_EXCM_LEVEL
      #undef  XT_TIMER_INDEX
      #define XT_TIMER_INDEX    3
    #endif
  #endif
  #if XCHAL_TIMER2_INTERRUPT != XTHAL_TIMER_UNCONFIGURED
    #if XCHAL_INT_LEVEL(XCHAL_TIMER2_INTERRUPT) <= XCHAL_EXCM_LEVEL
      #undef  XT_TIMER_INDEX
      #define XT_TIMER_INDEX    2
    #endif
  #endif
  #if XCHAL_TIMER1_INTERRUPT != XTHAL_TIMER_UNCONFIGURED
    #if XCHAL_INT_LEVEL(XCHAL_TIMER1_INTERRUPT) <= XCHAL_EXCM_LEVEL
      #undef  XT_TIMER_INDEX
      #define XT_TIMER_INDEX    1
    #endif
  #endif
  #if XCHAL_TIMER0_INTERRUPT != XTHAL_TIMER_UNCONFIGURED
    #if XCHAL_INT_LEVEL(XCHAL_TIMER0_INTERRUPT) <= XCHAL_EXCM_LEVEL
      #undef  XT_TIMER_INDEX
      #define XT_TIMER_INDEX    0
    #endif
  #endif
#endif

#endif /* XCHAL_HAVE_XEA3 */

#ifndef XT_TIMER_INDEX
  #error "There is no suitable timer in this Xtensa configuration."
#endif

#define XT_CCOMPARE             (CCOMPARE + XT_TIMER_INDEX)
#define XT_TIMER_INTNUM         XCHAL_TIMER_INTERRUPT(XT_TIMER_INDEX)
#define XT_TIMER_INTPRI         XCHAL_INT_LEVEL(XT_TIMER_INTNUM)

#if XCHAL_HAVE_XEA2
#define XT_TIMER_INTEN          (1 << XT_TIMER_INTNUM)
#endif

#if XT_TIMER_INDEX == 0
#define XT_WSR_CCOMPARE         XT_WSR_CCOMPARE0
#elif XT_TIMER_INDEX == 1
#define XT_WSR_CCOMPARE         XT_WSR_CCOMPARE1
#elif XT_TIMER_INDEX == 2
#define XT_WSR_CCOMPARE         XT_WSR_CCOMPARE2
#endif

#if XT_TIMER_INTNUM == XTHAL_TIMER_UNCONFIGURED
  #error "The timer selected by XT_TIMER_INDEX does not exist in this core."
#elif !XCHAL_HAVE_XEA3 && (XT_TIMER_INTPRI > XCHAL_EXCM_LEVEL)
  #error "The timer interrupt cannot be high priority (use medium or low)."
#endif

/*
Default number of timer ticks per second. This can be redefined as required
either by editing here or by overriding from the command line during build.
*/
#ifndef XT_TICK_PER_SEC
#define XT_TICK_PER_SEC     100
#endif

/*
Set processor clock frequency and determine clock divisor for timer tick.
If using a supported board via the board-independent API defined in xtbsp.h,
this may be left undefined but XT_BOARD must be defined. The frequency and
tick divisor will be computed during run-time initialization.
*/
#ifndef XT_BOARD
#ifndef XT_CLOCK_FREQ
#define XT_CLOCK_FREQ       1000000
#endif
#define XT_TICK_DIVISOR     (XT_CLOCK_FREQ / XT_TICK_PER_SEC)
#else
#ifndef __ASSEMBLER__
extern uint32_t xt_tick_divisor;
extern void     xt_tick_divisor_init(void);
#endif
#define XT_TICK_DIVISOR     xt_tick_divisor
#endif  /* XT_BOARD */

#endif  /* XT_RTOS_TIMER_INT */
#endif  /* XTENSA_TIMER_H */

