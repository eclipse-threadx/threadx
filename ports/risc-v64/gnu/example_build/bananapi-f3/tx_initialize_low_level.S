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
/*    trap_entry                           		     RISC-V64/GNU         */
/*                                                           6.2.1        */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Akif Ejaz, 10xEngineers                       			          */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function is responsible for riscv processor trap handle        */
/*    It will do the contex save and call c trap_handler and do contex    */
/*    load 								                                  */
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

    .global  trap_entry
    .extern  trap_handler
    .extern  _tx_thread_context_save
    .extern  _tx_thread_context_restore

trap_entry:
#if defined(__riscv_float_abi_single) || defined(__riscv_float_abi_double)
    addi    sp, sp, -520                       /* 65 * 8: integer + FP */
#else
    addi    sp, sp, -256                       /* 32 * 8: integer only */
#endif

    sd      x1, 224(sp)                        /* Save RA at offset 28*8 */

    call    _tx_thread_context_save

    csrr    a0, scause
    csrr    a1, sepc
    csrr    a2, stval
    addi    sp, sp, -8
    sd      ra, 0(sp)
    call    trap_handler
    ld      ra, 0(sp)
    addi    sp, sp, 8

    call    _tx_thread_context_restore
    /* never returns */
_trap_err:
    wfi
    j       _trap_err

    .section .text
/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _tx_initialize_low_level                  RISC-V64/GNU              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function is responsible for any low-level processor            */
/*    initialization, including setting up interrupt vectors, setting     */
/*    up a periodic timer interrupt source, saving the system stack       */
/*    pointer for use in ISR processing later, and finding the first      */
/*    available RAM memory address for tx_application_define.             */
/*                                                                        */
/**************************************************************************/

    .global  _tx_initialize_low_level
    .weak    _tx_initialize_low_level
    .extern  _end
    .extern  board_init
    .extern  trap_entry
    .extern  _tx_thread_system_stack_ptr
    .extern  _tx_initialize_unused_memory

_tx_initialize_low_level:
    la      t0, _tx_thread_system_stack_ptr
    sd      sp, 0(t0)                                 /* Save system stack */

    la      t0, _end
    la      t1, _tx_initialize_unused_memory
    sd      t0, 0(t1)                                 /* First free address */

    /* Disable global S-mode interrupts during early init. */
    li      t0, SSTATUS_SIE
    csrrc   zero, sstatus, t0

    /* Set SPIE = 1 so SRET re-enables interrupts. */
    li      t0, SSTATUS_SPIE
    csrrs   zero, sstatus, t0

    /* Enable S-mode timer and external interrupts. */
    li      t0, (SIE_STIE | SIE_SEIE)
    csrrs   zero, sie, t0

#ifdef __riscv_flen
    li      t0, SSTATUS_FS
    csrrs   zero, sstatus, t0
    fscsr   x0
#endif

    addi    sp, sp, -8
    sd      ra, 0(sp)
    call    board_init
    ld      ra, 0(sp)
    addi    sp, sp, 8

    la      t0, trap_entry
    csrw    stvec, t0
    ret
