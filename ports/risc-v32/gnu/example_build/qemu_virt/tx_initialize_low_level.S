/***************************************************************************
 * Copyright (c) 2025 10xEngineers
 *
 * This program and the accompanying materials are made available under the
 * terms of the MIT License which is available at
 * https://opensource.org/licenses/MIT.
 *
 * SPDX-License-Identifier: MIT
 **************************************************************************/

#include "csr.h"

    .section .text
    .align 4
/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    trap_entry                                          RISC-V32/GNU    */
/*                                                          6.4.x         */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Akif Ejaz, 10xEngineers                                             */
/*    Wei-Chen Lai, National Cheng Kung University                        */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function is responsible for the RISC-V trap entry. It saves    */
/*    the interrupted thread's context, calls the C trap_handler, then    */
/*    restores context via _tx_thread_context_restore.                    */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    None                                                                */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    None                                                                */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    trap_handler                                                        */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    hardware exception                                                  */
/*                                                                        */
/**************************************************************************/


/**************************************************************************/
/**************************************************************************/
/**                                                                       */
/** ThreadX Component                                                     */
/**                                                                       */
/**   Initialize                                                          */
/**                                                                       */
/**************************************************************************/
/**************************************************************************/
    .global  trap_entry
    .extern  trap_handler
    .extern  _tx_thread_context_restore
trap_entry:
#if defined(__riscv_flen) && ((__riscv_flen == 32)||(__riscv_flen == 64))
    addi    sp, sp, -65*4               /* Allocate space for all registers (FP enabled) */
#else
    addi    sp, sp, -32*4               /* Allocate space for all registers (no FP) */
#endif

    sw      x1, 28*4(sp)                /* Save RA before call clobbers it */

    call    _tx_thread_context_save

    csrr    a0, mcause
    csrr    a1, mepc
    csrr    a2, mtval
    addi    sp, sp, -4
    sw      ra, 0(sp)
    call    trap_handler
    lw      ra, 0(sp)
    addi    sp, sp, 4
    call    _tx_thread_context_restore
    /* _tx_thread_context_restore does not return. */
_err:
    wfi
    j       _err

    .section .text
/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _tx_initialize_low_level                           RISC-V32/GNU     */
/*                                                            6.4.x       */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Akif Ejaz, 10xEngineers                                             */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function is responsible for any low-level processor            */
/*    initialization, including setting up interrupt vectors, setting     */
/*    up a periodic timer interrupt source, saving the system stack       */
/*    pointer for use in ISR processing later, and finding the first      */
/*    available RAM memory address for tx_application_define.             */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    None                                                                */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    None                                                                */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    None                                                                */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    _tx_initialize_kernel_enter           ThreadX entry function        */
/*                                                                        */
/*  RELEASE HISTORY                                                       */
/*                                                                        */
/*    DATE              NAME                      DESCRIPTION             */
/*                                                                        */
/*  12-29-2025        Akif Ejaz        Adapted for RV32 from RV64 port    */
/*                                                                        */
/**************************************************************************/
/* VOID   _tx_initialize_low_level(VOID)
{  */
    .global  _tx_initialize_low_level
    .weak    _tx_initialize_low_level
    .extern  _end
    .extern  board_init
_tx_initialize_low_level:
    la      t0, _tx_thread_system_stack_ptr
    sw      sp, 0(t0)                   /* Save system stack pointer */

    la      t0, _end                    /* Pickup first free address */
    la      t1, _tx_initialize_unused_memory
    sw      t0, 0(t1)                   /* Save unused memory address */

    li      t0, MSTATUS_MIE
    csrrc   zero, mstatus, t0           /* Clear MSTATUS_MIE */
    li      t0, (MSTATUS_MPP_M | MSTATUS_MPIE)
    csrrs   zero, mstatus, t0           /* Set MSTATUS_MPP, MPIE */
    li      t0, (MIE_MTIE | MIE_MSIE | MIE_MEIE)
    csrrs   zero, mie, t0               /* Enable timer/software/external interrupts */
#ifdef __riscv_flen
    li      t0, MSTATUS_FS
    csrrs   zero, mstatus, t0           /* Enable F/D ISA */
    fscsr   x0
#endif
    addi    sp, sp, -4
    sw      ra, 0(sp)
    call    board_init
    lw      ra, 0(sp)
    addi    sp, sp, 4

    la      t0, trap_entry
    csrw    mtvec, t0
    ret
