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
/*    XTENSA CONTEXT SAVE AND RESTORE ROUTINES                            */
/*                                                                        */
/* Low-level functions for handling generic context save and restore of   */
/* registers not specifically addressed by the interrupt vectors and      */
/* handlers. Those registers (not handled by these functions) are PC, PS, */
/* A0, A1 (SP). Except for the calls to RTOS functions, this code is      */
/* generic to Xtensa.                                                     */
/*                                                                        */
/* Note that in Call0 ABI, interrupt handlers are expected to preserve    */
/* the callee-save regs (A12-A15), which is always the case if the        */
/* handlers are coded in C. However A12, A13 are made available as        */
/* scratch registers for interrupt dispatch code, so are presumed saved   */
/* anyway, and are always restored even in Call0 ABI. Only A14, A15 are   */
/* truly handled as callee-save regs.                                     */
/*                                                                        */
/*  RELEASE HISTORY                                                       */
/*                                                                        */
/*    DATE              NAME                      DESCRIPTION             */
/*                                                                        */
/*  12-31-2020     Cadence Design Systems   Initial Version 6.1.3         */
/*                                                                        */
/**************************************************************************/


#include "xtensa_rtos.h"

#ifdef XT_USE_OVLY
#include <xtensa/overlay_os_asm.h>
#endif

    .text

#if XCHAL_HAVE_XEA2

/***************************************************************************

    _xt_context_save

    !! MUST BE CALLED ONLY BY 'CALL0' INSTRUCTION !!

Saves all Xtensa processor state except PC, PS, A0, A1 (SP), A12, A13, in the
interrupt stack frame defined in xtensa_rtos.h.
Its counterpart is _xt_context_restore (which also restores A12, A13).

Caller is expected to have saved PC, PS, A0, A1 (SP), A12, A13 in the frame.
This function preserves A12 & A13 in order to provide the caller with 2 scratch 
regs that need not be saved over the call to this function. The choice of which
2 regs to provide is governed by xthal_window_spill_nw and xthal_save_extra_nw,
to avoid moving data more than necessary. Caller can assign regs accordingly.

Entry Conditions:
    A0  = Return address in caller.
    A1  = Stack pointer of interrupted thread or handler ("interruptee").
    Original A12, A13 have already been saved in the interrupt stack frame.
    Other processor state except PC, PS, A0, A1 (SP), A12, A13, is as at the 
    point of interruption.
    If windowed ABI, PS.EXCM = 1 (exceptions disabled).

Exit conditions:
    A0  = Return address in caller.
    A1  = Stack pointer of interrupted thread or handler ("interruptee").
    A12, A13 as at entry (preserved).
    If windowed ABI, PS.EXCM = 1 (exceptions disabled).

***************************************************************************/

    .global _xt_context_save
    .type   _xt_context_save,@function
    .align  4
_xt_context_save:

    s32i    a2,  sp, XT_STK_A2
    s32i    a3,  sp, XT_STK_A3
    s32i    a4,  sp, XT_STK_A4
    s32i    a5,  sp, XT_STK_A5
    s32i    a6,  sp, XT_STK_A6
    s32i    a7,  sp, XT_STK_A7
    s32i    a8,  sp, XT_STK_A8
    s32i    a9,  sp, XT_STK_A9
    s32i    a10, sp, XT_STK_A10
    s32i    a11, sp, XT_STK_A11

    /*
    Call0 ABI callee-saved regs a12-15 do not need to be saved here.
    a12-13 are the caller's responsibility so it can use them as scratch.
    So only need to save a14-a15 here for Windowed ABI (not Call0).
    */
    #ifndef __XTENSA_CALL0_ABI__
    s32i    a14, sp, XT_STK_A14
    s32i    a15, sp, XT_STK_A15
    #endif

    rsr     a3,  SAR
    s32i    a3,  sp, XT_STK_SAR

    #if XCHAL_HAVE_LOOPS
    rsr     a3,  LBEG
    s32i    a3,  sp, XT_STK_LBEG
    rsr     a3,  LEND
    s32i    a3,  sp, XT_STK_LEND
    rsr     a3,  LCOUNT
    s32i    a3,  sp, XT_STK_LCOUNT
    #endif

    #if XCHAL_HAVE_EXCLUSIVE
    /* Save and clear state of ATOMCTL */
    movi    a3,  0
    getex   a3
    s32i    a3,  sp, XT_STK_ATOMCTL
    #endif

    #if XT_USE_SWPRI
    /* Save virtual priority mask */
    movi    a3,  _xt_vpri_mask
    l32i    a3,  a3, 0
    s32i    a3,  sp, XT_STK_VPRI
    #endif

    #if XCHAL_EXTRA_SA_SIZE > 0 || !defined(__XTENSA_CALL0_ABI__)
    mov     a9,  a0                     /* preserve ret addr */
    #endif

    #ifndef __XTENSA_CALL0_ABI__
    /*
    To spill the reg windows, temp. need pre-interrupt stack ptr and a4-15.
    Need to save a9,12,13 temporarily (in frame temps) and recover originals.
    Interrupts need to be disabled below XCHAL_EXCM_LEVEL and window overflow
    and underflow exceptions disabled (assured by PS.EXCM == 1).
    */
    s32i    a12, sp, XT_STK_TMP0        /* temp. save stuff in stack frame */
    s32i    a13, sp, XT_STK_TMP1    
    s32i    a9,  sp, XT_STK_TMP2    

    /*
    Save the overlay state if we are supporting overlays. Since we just saved
    three registers, we can conveniently use them here. Note that as of now,
    overlays only work for windowed calling ABI.
    */
    #ifdef XT_USE_OVLY
    l32i    a9,  sp, XT_STK_PC          /* recover saved PC */
    _xt_overlay_get_state    a9, a12, a13
    s32i    a9,  sp, XT_STK_OVLY        /* save overlay state */
    #endif

    l32i    a12, sp, XT_STK_A12         /* recover original a9,12,13 */
    l32i    a13, sp, XT_STK_A13
    l32i    a9,  sp, XT_STK_A9
    addi    sp,  sp, XT_STK_FRMSZ       /* restore the interruptee's SP */
    call0   xthal_window_spill_nw       /* preserves only a4,5,8,9,12,13 */
    addi    sp,  sp, -XT_STK_FRMSZ
    l32i    a12, sp, XT_STK_TMP0        /* recover stuff from stack frame */
    l32i    a13, sp, XT_STK_TMP1    
    l32i    a9,  sp, XT_STK_TMP2    
    #endif

    #if XCHAL_EXTRA_SA_SIZE > 0
    /*  
    NOTE: Normally the xthal_save_extra_nw macro only affects address
    registers a2-a5. It is theoretically possible for Xtensa processor
    designers to write TIE that causes more address registers to be
    affected, but it is generally unlikely. If that ever happens,
    more registers need to be saved/restored around this macro invocation.
    Here we assume a9,12,13 are preserved.
    Future Xtensa tools releases might limit the regs that can be affected.
    */
    addi    a2,  sp, XT_STK_EXTRA       /* where to save it */
    # if XCHAL_EXTRA_SA_ALIGN > 16
    movi    a3, -XCHAL_EXTRA_SA_ALIGN
    and     a2, a2, a3                  /* align dynamically >16 bytes */
    # endif
    call0   xthal_save_extra_nw         /* destroys a0,2,3,4,5 */
    #endif

    #if XCHAL_EXTRA_SA_SIZE > 0 || !defined(__XTENSA_CALL0_ABI__)
    mov     a0, a9                      /* retrieve ret addr */
    #endif

    ret

/*******************************************************************************

_xt_context_restore

    !! MUST BE CALLED ONLY BY 'CALL0' INSTRUCTION !!

Restores all Xtensa processor state except PC, PS, A0, A1 (SP) (and in Call0
ABI, A14, A15 which are preserved by all interrupt handlers) from an interrupt 
stack frame defined in xtensa_rtos.h .
Its counterpart is _xt_context_save (whose caller saved A12, A13).

Caller is responsible to restore PC, PS, A0, A1 (SP).

Entry Conditions:
    A0  = Return address in caller.
    A1  = Stack pointer of interrupted thread or handler ("interruptee").

Exit conditions:
    A0  = Return address in caller.
    A1  = Stack pointer of interrupted thread or handler ("interruptee").
    Other processor state except PC, PS, A0, A1 (SP), is as at the point 
    of interruption.

*******************************************************************************/

    .global _xt_context_restore
    .type   _xt_context_restore,@function
    .align  4
_xt_context_restore:

    #if XCHAL_EXTRA_SA_SIZE > 0
    /*  
    NOTE: Normally the xthal_restore_extra_nw macro only affects address
    registers a2-a5. It is theoretically possible for Xtensa processor
    designers to write TIE that causes more address registers to be
    affected, but it is generally unlikely. If that ever happens,
    more registers need to be saved/restored around this macro invocation.
    Here we only assume a13 is preserved.
    Future Xtensa tools releases might limit the regs that can be affected.
    */
    mov     a13, a0                     /* preserve ret addr */
    addi    a2,  sp, XT_STK_EXTRA       /* where to find it */
    # if XCHAL_EXTRA_SA_ALIGN > 16
    movi    a3, -XCHAL_EXTRA_SA_ALIGN
    and     a2, a2, a3                  /* align dynamically >16 bytes */
    # endif
    call0   xthal_restore_extra_nw      /* destroys a0,2,3,4,5 */
    mov     a0,  a13                    /* retrieve ret addr */
    #endif

    #if XCHAL_HAVE_LOOPS
    l32i    a2,  sp, XT_STK_LBEG
    l32i    a3,  sp, XT_STK_LEND
    wsr     a2,  LBEG
    l32i    a2,  sp, XT_STK_LCOUNT
    wsr     a3,  LEND
    wsr     a2,  LCOUNT
    #endif

    #if XCHAL_HAVE_EXCLUSIVE
    /* Restore state of ATOMCTL */
    l32i    a2,  sp, XT_STK_ATOMCTL
    getex   a2
    #endif

    #ifdef XT_USE_OVLY
    /*
    If we are using overlays, this is a good spot to check if we need
    to restore an overlay for the incoming task. Here we have a bunch
    of registers to spare. Note that this step is going to use a few
    bytes of storage below SP (SP-20 to SP-32) if an overlay is going
    to be restored.
    */
    l32i    a2,  sp, XT_STK_PC          /* retrieve PC */
    l32i    a3,  sp, XT_STK_PS          /* retrieve PS */
    l32i    a4,  sp, XT_STK_OVLY        /* retrieve overlay state */
    l32i    a5,  sp, XT_STK_A1          /* retrieve stack ptr */
    _xt_overlay_check_map    a2, a3, a4, a5, a6
    s32i    a2,  sp, XT_STK_PC          /* save updated PC */
    s32i    a3,  sp, XT_STK_PS          /* save updated PS */
    #endif

    #ifdef XT_USE_SWPRI
    /* Restore virtual interrupt priority and interrupt enable */
    movi    a3,  _xt_intdata
    l32i    a4,  a3, 0                  /* a4 = _xt_intenable */
    l32i    a5,  sp, XT_STK_VPRI        /* a5 = saved _xt_vpri_mask */
    and     a4,  a4, a5
    wsr     a4,  INTENABLE              /* update INTENABLE */
    s32i    a5,  a3, 4                  /* restore _xt_vpri_mask */
    #endif

    l32i    a3,  sp, XT_STK_SAR
    l32i    a2,  sp, XT_STK_A2
    wsr     a3,  SAR
    l32i    a3,  sp, XT_STK_A3
    l32i    a4,  sp, XT_STK_A4
    l32i    a5,  sp, XT_STK_A5
    l32i    a6,  sp, XT_STK_A6
    l32i    a7,  sp, XT_STK_A7
    l32i    a8,  sp, XT_STK_A8
    l32i    a9,  sp, XT_STK_A9
    l32i    a10, sp, XT_STK_A10
    l32i    a11, sp, XT_STK_A11

    /*
    Call0 ABI callee-saved regs a12-15 do not need to be restored here.
    However a12-13 were saved for scratch before XT_RTOS_INT_ENTER(), 
    so need to be restored anyway, despite being callee-saved in Call0.
    */
    l32i    a12, sp, XT_STK_A12
    l32i    a13, sp, XT_STK_A13
    #ifndef __XTENSA_CALL0_ABI__
    l32i    a14, sp, XT_STK_A14
    l32i    a15, sp, XT_STK_A15
    #endif

    ret

#endif /* XCHAL_HAVE_XEA3 */


/*******************************************************************************

_xt_coproc_init

Initializes global co-processor management data, setting all co-processors
to "unowned". Leaves CPENABLE as it found it (does NOT clear it).

Called during initialization of the RTOS, before any threads run.

This may be called from normal Xtensa single-threaded application code which
might use co-processors. The Xtensa run-time initialization enables all 
co-processors. They must remain enabled here, else a co-processor exception
might occur outside of a thread, which the exception handler doesn't expect.

Entry Conditions:
    Xtensa single-threaded run-time environment is in effect.
    No thread is yet running.

Exit conditions:
    None.

Obeys ABI conventions per prototype:
    void _xt_coproc_init(void)

*******************************************************************************/

#if XCHAL_CP_NUM > 0

    .global _xt_coproc_init
    .type   _xt_coproc_init,@function
    .align  4
_xt_coproc_init:
    ENTRY0

    /* Initialize thread co-processor ownerships to 0 (unowned). */
    movi    a2, _xt_coproc_owner_sa         /* a2 = base of owner array */
    addi    a3, a2, XCHAL_CP_MAX << 2       /* a3 = top+1 of owner array */
    movi    a4, 0                           /* a4 = 0 (unowned) */
1:  s32i    a4, a2, 0
    addi    a2, a2, 4
    bltu    a2, a3, 1b

    RET0

#endif


/*******************************************************************************

_xt_coproc_release

Releases any and all co-processors owned by a given thread. The thread is 
identified by it's co-processor state save area defined in xtensa_context.h .

Must be called before a thread's co-proc save area is deleted to avoid
memory corruption when the exception handler tries to save the state.
May be called when a thread terminates or completes but does not delete
the co-proc save area, to avoid the exception handler having to save the 
thread's co-proc state before another thread can use it (optimization).

Entry Conditions:
    A2  = Pointer to base of co-processor state save area.

Exit conditions:
    None.

Obeys ABI conventions per prototype:
    void _xt_coproc_release(void * coproc_sa_base)

*******************************************************************************/

#if XCHAL_CP_NUM > 0

    .global _xt_coproc_release
    .type   _xt_coproc_release,@function
    .align  4
_xt_coproc_release:
    ENTRY0                                  /* a2 = base of save area */

    movi    a3, _xt_coproc_owner_sa         /* a3 = base of owner array */
    addi    a4, a3, XCHAL_CP_MAX << 2       /* a4 = top+1 of owner array */
    movi    a5, 0                           /* a5 = 0 (unowned) */

#if XCHAL_HAVE_XEA3
    movi    a6, PS_DI
    xps     a6, a6                          /* lock interrupts */
#else
    rsil    a6, XCHAL_EXCM_LEVEL            /* lock interrupts */
#endif

1:  l32i    a7, a3, 0                       /* a7 = owner at a3 */
    bne     a2, a7, 2f                      /* if (coproc_sa_base == owner) */
    s32i    a5, a3, 0                       /*   owner = unowned */
2:  addi    a3, a3, 1<<2                    /* a3 = next entry in owner array */
    bltu    a3, a4, 1b                      /* repeat until end of array */

3:
    wsr     a6, PS                          /* restore interrupts */
    rsync

    RET0

#endif

