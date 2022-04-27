//
// Armv8-A AArch64 - Basic Mutex Example
// Includes the option (USE_LSE_ATOMIC) to use Large System Extension (LSE) atomics introduced in Armv8.1-A
//
// Copyright (c) 2012-2019 Arm Limited (or its affiliates). All rights reserved.
// Use, modification and redistribution of this file is subject to your possession of a
// valid End User License Agreement for the Arm Product of which these examples are part of
// and your compliance with all applicable terms and conditions of such licence agreement.
//


    .text
    .cfi_sections .debug_frame  // put stack frame info into .debug_frame instead of .eh_frame


    .global _mutex_initialize
    .global _mutex_acquire
    .global _mutex_release

//
// These routines implement the mutex management functions required for running
// the Arm C library in a multi-threaded environment.
//
// They use a value of 0 to represent an unlocked mutex, and 1 for a locked mutex
//
// **********************************************************************
//

    .type _mutex_initialize, "function"
    .cfi_startproc
_mutex_initialize:

    //
    // mark the mutex as unlocked
    //
    mov     w1, #0
    str     w1, [x0]

    //
    // we are running multi-threaded, so set a non-zero return
    // value (function prototype says use 1)
    //
    mov     w0, #1
    ret
    .cfi_endproc

#if !defined(USE_LSE_ATOMIC)

    .type _mutex_acquire, "function"
    .cfi_startproc
_mutex_acquire:

    //
    // send ourselves an event, so we don't stick on the wfe at the
    // top of the loop
    //
    sevl

    //
    // wait until the mutex is available
    //
loop:
    wfe
    ldaxr   w1, [x0]
    cbnz    w1, loop

    //
    // mutex is (at least, it was) available - try to claim it
    //
    mov     w1, #1
    stxr    w2, w1, [x0]
    cbnz    w2, loop

    //
    // OK, we have the mutex, our work is done here
    //
    ret
    .cfi_endproc


    .type _mutex_release, "function"
    .cfi_startproc
_mutex_release:

    mov     w1, #0
    stlr    w1, [x0]
    ret
    .cfi_endproc

#else  // LSE version

    .type _mutex_acquire, "function"
    .cfi_startproc
_mutex_acquire:
    // This uses a "ticket lock".  The lock is stored as a 32-bit value: 
    // - the upper 16-bits record the thread's ticket number ("take a ticket")
    // - the lower 16-bits record the ticket being served    ("now serving")

    // atomically load then increment the thread's ticket number ("take a ticket")
    mov w3, #(1 << 16)
    ldadda w3, w1, [x0]

    // is the ticket now being served?
    eor w2, w1, w1, ror #16
    cbz w2, loop_exit

    // no, so wait for the ticket to be served

    // send a local event to avoid missing an unlock before the exclusive load
    sevl

loop:
    wfe
    ldaxrh  w3, [x0]
    eor w2, w3, w1, lsr #16
    cbnz    w2, loop

    //
    // OK, we have the mutex, our work is done here
    //
loop_exit:
    ret
    .cfi_endproc


    .type _mutex_release, "function"
    .cfi_startproc
_mutex_release:
    mov     w1, #1
    staddlh w1, [x0]
    ret
    .cfi_endproc
#endif
