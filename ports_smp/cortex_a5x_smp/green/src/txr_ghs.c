/*
 *            ThreadX API Runtime Error Support
 *
 *      Copyright 1983-2019 Green Hills Software LLC.
 *
 *  This program is the property of Green Hills Software LLC.,
 *  its contents are proprietary information and no part of it
 *  is to be disclosed to anyone except employees of Green Hills
 *  Software LLC., or as agreed in writing signed by the President
 *  of Green Hills Software LLC.
 */

/* #include "tx_ghs.h" */
#ifndef TX_DISABLE_ERROR_CHECKING
#define TX_DISABLE_ERROR_CHECKING
#endif
#include "tx_api.h"

/* Customized ThreadX API runtime error support routine. */

void _rnerr(int num, int linenum, const char*str, void*ptr, ...);

/* __ghs_rnerr()
   This is the custom runtime error checking routine.
   This implementation uses the existing __rnerr() routine.
   Another implementation could use the .syscall mechanism,
   provided MULTI was modified to understand that.
 */
void __ghs_rnerr(char *errMsg, int stackLevels, int stackTraceDisplay, void *hexVal) {
    TX_INTERRUPT_SAVE_AREA
    int num;
    /*
       Initialize the stack levels value.

       Add 3 to account for the calls to _rnerr, __rnerr, and
       __ghs_rnerr.

       If the implementation changes, calls to __ghs_rnerr
       will not need to be changed.

       Zero is not permitted, so substitute 3 in that case.
       */
    num = (stackLevels+3) & 0xf;
    if (!num) {
        num = 3;
    }
    /*
       Shift the stack levels value to bits 12..15 and
       insert the stack trace display value in bit 11.
       Bits 0..10 are unused.
     */
    num = (num << 12) | (stackTraceDisplay ? 0x800 : 0);

    /* This will mask all interrupts in the RTEC code, which is probably
       unacceptable for many targets. */
    TX_DISABLE
    _rnerr(num, -1, (const char *)hexVal, (void *)errMsg);
    TX_RESTORE
}


/* ThreadX thread stack checking runtime support routine. */

extern char              __ghsbegin_stack[];

void __stkchk(void) {
    int i;
    TX_THREAD   *current_thread_ptr;

    /* Pickup current thread pointer.  */
    TX_THREAD_GET_CURRENT(current_thread_ptr)

    if(current_thread_ptr)
    {
        if((unsigned)(&i) <=
                (unsigned)(current_thread_ptr -> tx_thread_stack_start))
        {
            _rnerr(21, -1, 0, 0);
        }
    }
    else
    {
        if((unsigned)(&i) <= (unsigned)__ghsbegin_stack)
        {
            _rnerr(21, -1, 0, 0);
        }
    }
}
