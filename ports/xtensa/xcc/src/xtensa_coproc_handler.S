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
/*    Xtensa coprocessor handling routines. This code is only active if   */
/*    one or more coprocessors are present.                               */
/*                                                                        */
/*  RELEASE HISTORY                                                       */
/*                                                                        */
/*    DATE              NAME                      DESCRIPTION             */
/*                                                                        */
/*  12-31-2020     Cadence Design Systems   Initial Version 6.1.3         */
/*                                                                        */
/**************************************************************************/


#include <xtensa/config/specreg.h>
#include <xtensa/coreasm.h>

#include "xtensa_context.h"
#include "xtensa_rtos.h"


#if XCHAL_CP_NUM > 0

//-----------------------------------------------------------------------------
//  Coprocessor related state and precomputed values.
//-----------------------------------------------------------------------------

// Table of coprocessor owners, identified by thread's CP save area pointer.
// Zero means coprocessor is not owned.

        .data
        .global _xt_coproc_owner_sa
        .align  16,,XCHAL_CP_MAX << 2   //  minimize crossing cache boundaries
_xt_coproc_owner_sa:
        .rept   XCHAL_CP_MAX
        .word   0
        .endr

// Bitmask table for CP n's enable bit, indexed by coprocessor number.

        .section .rodata, "a"
        .global  _xt_coproc_mask
        .align   16,,8                  // try to keep it all in one cache line
        .set     i, 0
_xt_coproc_mask:
        .rept   XCHAL_CP_MAX
        .long   (i<<16) | (1<<i)        // upper 16-bits = i, lower = bitmask
        .set    i, i+1
        .endr

// Offset to CP n save area in thread's CP save area.

        .global _xt_coproc_sa_offset
        .align  16                      // minimize crossing cache boundaries
_xt_coproc_sa_offset:
        .word   XT_CP0_SA, XT_CP1_SA, XT_CP2_SA, XT_CP3_SA
        .word   XT_CP4_SA, XT_CP5_SA, XT_CP6_SA, XT_CP7_SA


//-----------------------------------------------------------------------------
// _xt_coproc_handler
//
// Handles coprocessor exceptions and manages lazy context switching between
// multiple threads sharing the coprocessor(s).
// Register use:
//   a0 - on entry, return address (must have been called via call0).
//   a1 - pointing to valid exception stack frame.
//   a2 - on entry, must hold coprocessor index. On exit, 0 if OK.
//   a3-a15 - may all be used and trashed by this routine.
//-----------------------------------------------------------------------------

        .text
        .align  4
        .global _xt_coproc_handler

_xt_coproc_handler:

        mov     a7,  a0                         // a7 = return address
        mov     a5,  a2                         // a5 = CP index n

        // Get coprocessor state save area of new owner thread
        call0   XT_RTOS_CP_STATE                // a15 = new owner's save area
        beqz    a15, .L_xt_coproc_invalid       // not in a thread (invalid)
        l32i    a4,  a15, XT_CP_ASA             // actual save area address
        beqz    a4,  .L_xt_coproc_invalid       // thread has no save area

        // Enable the co-processor's bit in CPENABLE
        movi    a0,  _xt_coproc_mask
        rsr     a4,  CPENABLE                   // a4 = CPENABLE
        addx4   a0,  a5, a0                     // a0 = &_xt_coproc_mask[n]
        l32i    a0,  a0, 0                      // a0 = (n << 16) | (1 << n)
        movi    a3,  _xt_coproc_owner_sa
        extui   a2,  a0, 0, 16                  // coprocessor bitmask portion
        or      a4,  a4, a2                     // a4 = CPENABLE | (1 << n)
        wsr     a4,  CPENABLE

        // Get old coprocessor owner thread (save area ptr) and assign new one
        addx4   a3,  a5, a3                     // a3 = &_xt_coproc_owner_sa[n]
        l32i    a2,  a3, 0                      // a2 = old owner's save area
        s32i    a15, a3, 0                      // _xt_coproc_owner_sa[n] = new
        rsync                                   // ensure wsr.CPENABLE is complete

        // Do we need to context-switch this coprocessor ?
        beq     a15, a2, .L_xt_coproc_done      // new owner == old, we're done

        // if no old owner then nothing to save
        beqz    a2,  .L_check_new

        // If old owner not actively using CP then nothing to save.
        l16ui   a4,  a2, XT_CPENABLE            // a4 = old owner's CPENABLE
        bnone   a4,  a0, .L_check_new           // old owner not using CP

.L_save_old:
        // We need to save old owner's coprocessor state
        movi    a5,  _xt_coproc_sa_offset

        // Mark old owner state as no longer active (CPENABLE bit n clear)
        xor     a4,  a4, a0                     // clear CP in old owner's CPENABLE
        s16i    a4,  a2, XT_CPENABLE            // update old owner's CPENABLE

        extui   a4,  a0, 16, 5                  // a4 = CP index = n
        addx4   a5,  a4, a5                     // a5 = &_xt_coproc_sa_offset[n]

        // Mark old owner state as saved (CPSTORED bit n set)
        l16ui   a4,  a2, XT_CPSTORED            // a4 = old owner's CPSTORED
        l32i    a5,  a5, 0                      // a5 = XT_CP[n]_SA offset
        or      a4,  a4, a0                     // set CP in old owner's CPSTORED
        s16i    a4,  a2, XT_CPSTORED            // update old owner's CPSTORED
        l32i    a2,  a2, XT_CP_ASA              // ptr to actual (aligned) save area
        extui   a3,  a0, 16, 5                  // a3 = CP index = n
        add     a2,  a2, a5                     // a2 = old owner's area for CP n

        // The config-specific HAL macro invoked below destroys a2-a6.
        // It is theoretically possible for Xtensa processor designers to write TIE
        // that causes more address registers to be affected, but it is generally
        // unlikely. If that ever happens, more registers needs to be saved/restored
        // around this macro invocation, and the value in a15 needs to be recomputed.

        xchal_cpi_store_funcbody

.L_check_new:
        // Check if any state has to be restored for new owner.
        // NOTE: a15 = new owner's save area, cannot be zero when we get here.

        l16ui   a3,  a15, XT_CPSTORED           // a3 = new owner's CPSTORED
        movi    a4,  _xt_coproc_sa_offset
        bnone   a3,  a0,  .L_check_cs           // full CP not saved, check callee-saved
        xor     a3,  a3,  a0                    // CPSTORED bit is set, clear it
        s16i    a3,  a15, XT_CPSTORED           // update new owner's CPSTORED

        // Adjust new owner's save area pointers to area for CP n.
        extui   a3,  a0, 16, 5                  // a3 = CP index = n
        addx4   a4,  a3, a4                     // a4 = &_xt_coproc_sa_offset[n]
        l32i    a4,  a4, 0                      // a4 = XT_CP[n]_SA
        l32i    a5, a15, XT_CP_ASA              // ptr to actual (aligned) save area
        add     a2,  a4, a5                     // a2 = new owner's area for CP

        // The config-specific HAL macro invoked below destroys a2-a6.
        // It is theoretically possible for Xtensa processor designers to write TIE
        // that causes more address registers to be affected, but it is generally
        // unlikely. If that ever happens, more registers needs to be saved/restored
        // around this macro invocation.

        xchal_cpi_load_funcbody

.L_xt_coproc_done:
        movi    a2,  0                          // a2 <- 0 == OK
.L_xt_coproc_err:
        mov     a0,  a7                         // return address
        ret

.L_check_cs:
        // a0 = CP mask in low bits, a15 = new owner's save area.
        l16ui   a2, a15, XT_CP_CS_ST            // a2 = mask of CPs saved
        bnone   a2,  a0, .L_xt_coproc_done      // if no match then done
        and     a2,  a2, a0                     // a2 = which CPs to restore
        extui   a2,  a2, 0, 8                   // extract low 8 bits
        call0   _xt_coproc_restorecs            // restore CP registers
        j       .L_xt_coproc_done

.L_xt_coproc_invalid:
        // Coprocessor exception occurred outside a thread or the thread
        // did not allocate space to save coprocessor state. Return error.
        movi    a2,  1
        j       .L_xt_coproc_err


//-----------------------------------------------------------------------------
// _tx_thread_coproc_state
//
// Helper function to return the save area for the current thread, if any.
// Returns, in a15, the pointer to the save area if any, else zero.
// If in interrupt context, returns zero. Only uses a15.
// Must be called only via call0.
//-----------------------------------------------------------------------------

        .global     _tx_thread_coproc_state
        .type       _tx_thread_coproc_state,@function
        .align      4

_tx_thread_coproc_state:

        // return ( _tx_thread_system_state == 0 && _tx_thread_current_ptr != 0 
        //          ? (&_tx_thread_current_ptr->tx_thread_cp_state) : 0 )

        movi    a15, _tx_thread_system_state    // check if interrupt state
        l32i    a15, a15, 0
        bnez    a15, 1f
        movi    a15, _tx_thread_current_ptr     // check if thread running
        l32i    a15, a15, 0
        beqz    a15, 2f

        // Return base address of current thread's co-prcoessor save area.
        addi    a15, a15, tx_thread_cp_state
        ret
1:
        movi    a15, 0                          // return error
2:
        ret


//-----------------------------------------------------------------------------
// _xt_coproc_savecs
//
// If there is a current thread and it has a coprocessor state save area, then
// save all callee-saved state into this area. This function is called from the
// solicited context switch handler. It calls a system-specific function to get
// the coprocessor save area base address.
//
// Entry conditions:
//    - The thread being switched out is still the current thread.
//    - CPENABLE state reflects which coprocessors are active.
//    - Registers have been saved/spilled already.
//
// Exit conditions:
//    - All necessary CP callee-saved state has been saved.
//    - Registers a7-a15 have been trashed.
//
// Must be called from assembly code only, using CALL0.
//-----------------------------------------------------------------------------

        .global     _xt_coproc_savecs
        .type       _xt_coproc_savecs,@function
        .align      4
_xt_coproc_savecs:

        // At entry, CPENABLE should be showing which CPs are enabled.

        rsr     a11, CPENABLE                   // a11 = which CPs are enabled
        beqz    a11, .Ldone                     // quick exit if none
        mov     a14, a0                         // save return address
        call0   XT_RTOS_CP_STATE                // get address of CP save area
        mov     a0, a14                         // restore return address
        beqz    a15, .Ldone                     // if none then nothing to do
        l32i    a14, a15, XT_CP_ASA             // a14 = base of aligned save area
        beqz    a14, .Ldone                     // no save area, nothing to do
        s16i    a11, a15, XT_CP_CS_ST           // save mask of CPs being stored
        movi    a13, _xt_coproc_sa_offset       // array of CP save offsets
        l32i    a15, a15, XT_CP_ASA             // a15 = base of aligned save area

#if XCHAL_CP0_SA_SIZE
        bbci.l  a11, 0, 2f                      // CP 0 not enabled
        l32i    a14, a13, 0                     // a14 = _xt_coproc_sa_offset[0]
        add     a12, a14, a15                   // a12 = save area for CP 0
        xchal_cp0_store a12, a7, a8, a9, a10 continue=0 ofs=-1 select=XTHAL_SAS_TIE|XTHAL_SAS_NOCC|XTHAL_SAS_CALE alloc=XTHAL_SAS_ALL
2:
#endif

#if XCHAL_CP1_SA_SIZE
        bbci.l  a11, 1, 2f                      // CP 1 not enabled
        l32i    a14, a13, 4                     // a14 = _xt_coproc_sa_offset[1]
        add     a12, a14, a15                   // a12 = save area for CP 1
        xchal_cp1_store a12, a7, a8, a9, a10 continue=0 ofs=-1 select=XTHAL_SAS_TIE|XTHAL_SAS_NOCC|XTHAL_SAS_CALE alloc=XTHAL_SAS_ALL
2:
#endif

#if XCHAL_CP2_SA_SIZE
        bbci.l  a11, 2, 2f
        l32i    a14, a13, 8
        add     a12, a14, a15
        xchal_cp2_store a12, a7, a8, a9, a10 continue=0 ofs=-1 select=XTHAL_SAS_TIE|XTHAL_SAS_NOCC|XTHAL_SAS_CALE alloc=XTHAL_SAS_ALL
2:
#endif

#if XCHAL_CP3_SA_SIZE
        bbci.l  a11, 3, 2f
        l32i    a14, a13, 12
        add     a12, a14, a15
        xchal_cp3_store a12, a7, a8, a9, a10 continue=0 ofs=-1 select=XTHAL_SAS_TIE|XTHAL_SAS_NOCC|XTHAL_SAS_CALE alloc=XTHAL_SAS_ALL
2:
#endif

#if XCHAL_CP4_SA_SIZE
        bbci.l  a11, 4, 2f
        l32i    a14, a13, 16
        add     a12, a14, a15
        xchal_cp4_store a12, a7, a8, a9, a10 continue=0 ofs=-1 select=XTHAL_SAS_TIE|XTHAL_SAS_NOCC|XTHAL_SAS_CALE alloc=XTHAL_SAS_ALL
2:
#endif

#if XCHAL_CP5_SA_SIZE
        bbci.l  a11, 5, 2f
        l32i    a14, a13, 20
        add     a12, a14, a15
        xchal_cp5_store a12, a7, a8, a9, a10 continue=0 ofs=-1 select=XTHAL_SAS_TIE|XTHAL_SAS_NOCC|XTHAL_SAS_CALE alloc=XTHAL_SAS_ALL
2:
#endif

#if XCHAL_CP6_SA_SIZE
        bbci.l  a11, 6, 2f
        l32i    a14, a13, 24
        add     a12, a14, a15
        xchal_cp6_store a12, a7, a8, a9, a10 continue=0 ofs=-1 select=XTHAL_SAS_TIE|XTHAL_SAS_NOCC|XTHAL_SAS_CALE alloc=XTHAL_SAS_ALL
2:
#endif

#if XCHAL_CP7_SA_SIZE
        bbci.l  a11, 7, 2f
        l32i    a14, a13, 28
        add     a12, a14, a15
        xchal_cp7_store a12, a7, a8, a9, a10 continue=0 ofs=-1 select=XTHAL_SAS_TIE|XTHAL_SAS_NOCC|XTHAL_SAS_CALE alloc=XTHAL_SAS_ALL
2:
#endif

.Ldone:
        ret


//-----------------------------------------------------------------------------
// _xt_coproc_restorecs
//
// Restore any callee-saved coprocessor state for the incoming thread.
// This function is called from coprocessor exception handling, when giving
// ownership to a thread that solicited a context switch earlier. It calls a
// system-specific function to get the coprocessor save area base address.
//
// Entry conditions:
//    - The incoming thread is set as the current thread.
//    - CPENABLE is set up correctly for all required coprocessors.
//    - a2 = mask of coprocessors to be restored.
//
// Exit conditions:
//    - All necessary CP callee-saved state has been restored.
//    - CPENABLE - unchanged.
//    - Registers a2, a8-a15 have been trashed.
//
// Must be called from assembly code only, using CALL0.
//-----------------------------------------------------------------------------

        .global     _xt_coproc_restorecs
        .type       _xt_coproc_restorecs,@function
        .align      4
_xt_coproc_restorecs:

        mov     a14, a0                         // save return address
        call0   XT_RTOS_CP_STATE                // get address of CP save area
        mov     a0, a14                         // restore return address
        beqz    a15, .Ldone2                    // if none then nothing to do
        l32i    a14, a15, XT_CP_ASA             // a14 = base of aligned save area
        beqz    a14, .Ldone2                    // no save area, nothing to do
        l16ui   a13, a15, XT_CP_CS_ST           // a13 = which CPs have been saved
        xor     a13, a13, a2                    // clear the ones being restored
        s16i    a13, a15, XT_CP_CS_ST           // update saved CP mask
        movi    a13, _xt_coproc_sa_offset       // array of CP save offsets
        l32i    a15, a15, XT_CP_ASA             // a15 = base of aligned save area

#if XCHAL_CP0_SA_SIZE
        bbci.l  a2, 0, 2f                       // CP 0 not enabled
        l32i    a14, a13, 0                     // a14 = _xt_coproc_sa_offset[0]
        add     a12, a14, a15                   // a12 = save area for CP 0
        xchal_cp0_load a12, a8, a9, a10, a11 continue=0 ofs=-1 select=XTHAL_SAS_TIE|XTHAL_SAS_NOCC|XTHAL_SAS_CALE alloc=XTHAL_SAS_ALL
2:
#endif

#if XCHAL_CP1_SA_SIZE
        bbci.l  a2, 1, 2f                       // CP 1 not enabled
        l32i    a14, a13, 4                     // a14 = _xt_coproc_sa_offset[1]
        add     a12, a14, a15                   // a12 = save area for CP 1
        xchal_cp1_load a12, a8, a9, a10, a11 continue=0 ofs=-1 select=XTHAL_SAS_TIE|XTHAL_SAS_NOCC|XTHAL_SAS_CALE alloc=XTHAL_SAS_ALL
2:
#endif

#if XCHAL_CP2_SA_SIZE
        bbci.l  a2, 2, 2f
        l32i    a14, a13, 8
        add     a12, a14, a15
        xchal_cp2_load a12, a8, a9, a10, a11 continue=0 ofs=-1 select=XTHAL_SAS_TIE|XTHAL_SAS_NOCC|XTHAL_SAS_CALE alloc=XTHAL_SAS_ALL
2:
#endif

#if XCHAL_CP3_SA_SIZE
        bbci.l  a2, 3, 2f
        l32i    a14, a13, 12
        add     a12, a14, a15
        xchal_cp3_load a12, a8, a9, a10, a11 continue=0 ofs=-1 select=XTHAL_SAS_TIE|XTHAL_SAS_NOCC|XTHAL_SAS_CALE alloc=XTHAL_SAS_ALL
2:
#endif

#if XCHAL_CP4_SA_SIZE
        bbci.l  a2, 4, 2f
        l32i    a14, a13, 16
        add     a12, a14, a15
        xchal_cp4_load a12, a8, a9, a10, a11 continue=0 ofs=-1 select=XTHAL_SAS_TIE|XTHAL_SAS_NOCC|XTHAL_SAS_CALE alloc=XTHAL_SAS_ALL
2:
#endif

#if XCHAL_CP5_SA_SIZE
        bbci.l  a2, 5, 2f
        l32i    a14, a13, 20
        add     a12, a14, a15
        xchal_cp5_load a12, a8, a9, a10, a11 continue=0 ofs=-1 select=XTHAL_SAS_TIE|XTHAL_SAS_NOCC|XTHAL_SAS_CALE alloc=XTHAL_SAS_ALL
2:
#endif

#if XCHAL_CP6_SA_SIZE
        bbci.l  a2, 6, 2f
        l32i    a14, a13, 24
        add     a12, a14, a15
        xchal_cp6_load a12, a8, a9, a10, a11 continue=0 ofs=-1 select=XTHAL_SAS_TIE|XTHAL_SAS_NOCC|XTHAL_SAS_CALE alloc=XTHAL_SAS_ALL
2:
#endif

#if XCHAL_CP7_SA_SIZE
        bbci.l  a2, 7, 2f
        l32i    a14, a13, 28
        add     a12, a14, a15
        xchal_cp7_load a12, a8, a9, a10, a11 continue=0 ofs=-1 select=XTHAL_SAS_TIE|XTHAL_SAS_NOCC|XTHAL_SAS_CALE alloc=XTHAL_SAS_ALL
2:
#endif

.Ldone2:
        ret


#if XCHAL_HAVE_XEA3

//-----------------------------------------------------------------------------
//  For XEA3, coprocessor exceptions come here. This is a wrapper function that
//  calls _xt_coproc_handler() to do the actual work. We don't want the handler
//  to be interrupted because that might cause a round-robin switch and leave
//  coprocessor context in a confused state. So interrupts are disabled before
//  calling the handler. They will be re-enabled on return from exception.
//-----------------------------------------------------------------------------

        .text
        .global     _xt_coproc_exc
        .type       _xt_coproc_exc,@function
        .align      4

_xt_coproc_exc:
#ifdef __XTENSA_CALL0_ABI__
    addi    a1,  a1, -16                    // reserve 16 bytes on stack
    s32i    a0,  a1, 0                      // save return address
    s32i   a15,  a1, 8                      // must save a15 (see dispatch)
    movi    a3,  PS_DI_MASK
    xps     a3,  a3                         // Set PS.DI, disable interrupts
    l32i    a3,  a2, XT_STK_EXCCAUSE        // a3 <- exccause
    extui   a2,  a3, 8, 4                   // a2 <- CP index
    call0   _xt_coproc_handler
    l32i    a0,  a1, 0                      // restore return address
    l32i   a15,  a1, 8                      // restore a15
    addi    a1,  a1, 16
    ret
#else
    entry   a1,  48                         // reserve 16 bytes on stack
    s32i    a0,  a1, 0                      // save return address
    movi    a3,  PS_DI_MASK
    xps     a3,  a3                         // Set PS.DI, disable interrupts
    l32i    a3,  a2, XT_STK_EXCCAUSE        // a3 <- exccause
    extui   a2,  a3, 8, 4                   // a2 <- CP index
    call0   _xt_coproc_handler
    l32i    a0,  a1, 0                      // restore return address
    retw
#endif

#endif // XCHAL_HAVE_XEA3


#if XCHAL_HAVE_XEA2

//-----------------------------------------------------------------------------
//  XEA2 coprocessor exception dispatcher. Save enough state to be able to call
//  the coprocessor handler, then restore and return.
//-----------------------------------------------------------------------------

        .text
        .global     _xt_coproc_exc
        .type       _xt_coproc_exc,@function
        .align      4
  
_xt_coproc_exc:
    
        mov     a0, sp                          // Allocate stack frame
        addi    sp, sp, -XT_STK_FRMSZ
        s32i    a0, sp, XT_STK_A1               // save SP
#if XCHAL_HAVE_WINDOWED
        s32e    a0, sp, -12                     // for debug backtrace
#endif  
        rsr     a0, PS
        s32i    a0, sp, XT_STK_PS               // save PS
        rsr     a0, EPC_1
        s32i    a0, sp, XT_STK_PC               // save PC
        rsr     a0, EXCSAVE_1
        s32i    a0, sp, XT_STK_A0               // retrieve and save a0
#if XCHAL_HAVE_WINDOWED
        s32e    a0, sp, -16                     // for debug backtrace
#endif
        s32i    a2, sp, XT_STK_A2
        s32i    a3, sp, XT_STK_A3
        s32i    a4, sp, XT_STK_A4
        s32i    a5, sp, XT_STK_A5
        s32i    a6, sp, XT_STK_A6
        s32i    a7, sp, XT_STK_A7
        s32i    a8, sp, XT_STK_A8
        s32i    a9, sp, XT_STK_A9
        s32i    a10, sp, XT_STK_A10
        s32i    a11, sp, XT_STK_A11
        s32i    a12, sp, XT_STK_A12
        s32i    a13, sp, XT_STK_A13
        s32i    a14, sp, XT_STK_A14
        s32i    a15, sp, XT_STK_A15

        rsr     a3, EXCCAUSE                    // a3 <- exccause
        addi    a2, a3, -EXCCAUSE_CP0_DISABLED  // a2 <- CP index
        call0   _xt_coproc_handler

        mov     a0, a2                          // save return value
        l32i    a2, sp, XT_STK_A2
        l32i    a3, sp, XT_STK_A3
        l32i    a4, sp, XT_STK_A4
        l32i    a5, sp, XT_STK_A5
        l32i    a6, sp, XT_STK_A6
        l32i    a7, sp, XT_STK_A7
        l32i    a8, sp, XT_STK_A8
        l32i    a9, sp, XT_STK_A9
        l32i    a10, sp, XT_STK_A10
        l32i    a11, sp, XT_STK_A11
        l32i    a12, sp, XT_STK_A12
        l32i    a13, sp, XT_STK_A13
        l32i    a14, sp, XT_STK_A14
        l32i    a15, sp, XT_STK_A15
        bnez    a0, .Lfail                      // abort if failure
        l32i    a0, sp, XT_STK_PC
        wsr     a0, EPC_1                       // restore PC
        l32i    a0, sp, XT_STK_PS
        wsr     a0, PS                          // restore PS
        l32i    a0, sp, XT_STK_A0
        addi    a1, a1, XT_STK_FRMSZ            // deallocate stack frame
        rfe

.Lfail:
        call0   _xt_panic

#endif // XCHAL_HAVE_XEA2

#endif // XCHAL_CP_NUM > 0

