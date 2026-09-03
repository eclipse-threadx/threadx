/***************************************************************************
 * Copyright (c) 2026 10xEngineers
 *
 * This program and the accompanying materials are made available under the
 * terms of the MIT License which is available at
 * https://opensource.org/licenses/MIT.
 *
 * SPDX-License-Identifier: MIT
 **************************************************************************/

/* Trap entry and low-level init for RISC-V QEMU virt regression tests.
   Supports both RV32 and RV64 via __riscv_xlen conditionals. */

#include "csr.h"
#if __riscv_xlen == 32
#include "tx_port.h"
#endif

#if __riscv_xlen == 64
#define STORE   sd
#define LOAD    ld
#define REGBYTES 8
#else
#define STORE   sw
#define LOAD    lw
#define REGBYTES 4
#endif

    .section .text
    .align 4

/**************************************************************************/
/*  trap_entry — saves context, calls C trap_handler, restores context.   */
/**************************************************************************/
    .global  trap_entry
    .extern  trap_handler
    .extern  _tx_thread_context_restore

/* Use a port contract when one exists. Keep legacy ports compatible. */
#ifndef TX_RISCV_TRAP_FRAME_SIZE
#if defined(__riscv_float_abi_single) || defined(__riscv_float_abi_double)
#define TX_RISCV_TRAP_FRAME_SIZE        (65 * REGBYTES)
#define TX_RISCV_TRAP_CALL_FRAME_SIZE   REGBYTES
#else
#define TX_RISCV_TRAP_FRAME_SIZE        (32 * REGBYTES)
#define TX_RISCV_TRAP_CALL_FRAME_SIZE   16
#endif
#endif

    .equ    TX_TRAP_FRAME_SIZE, TX_RISCV_TRAP_FRAME_SIZE
    .equ    TX_TRAP_CALL_FRAME_SIZE, TX_RISCV_TRAP_CALL_FRAME_SIZE

trap_entry:
    addi    sp, sp, -TX_TRAP_FRAME_SIZE

    STORE   x1, (28 * REGBYTES)(sp)

    call    _tx_thread_context_save

    csrr    a0, mcause
    csrr    a1, mepc
    csrr    a2, mtval
    addi    sp, sp, -TX_TRAP_CALL_FRAME_SIZE
    STORE   ra, 0(sp)
    call    trap_handler
    LOAD    ra, 0(sp)
    addi    sp, sp, TX_TRAP_CALL_FRAME_SIZE

    call    _tx_thread_context_restore

_trap_err:
    wfi
    j       _trap_err

/**************************************************************************/
/*  _tx_initialize_low_level — hardware init for QEMU virt.              */
/**************************************************************************/
    .section .text
    .global  _tx_initialize_low_level
    .extern  _end
    .extern  board_init

_tx_initialize_low_level:

    la      t0, _tx_thread_system_stack_ptr
    STORE   sp, 0(t0)

    la      t0, _end
    la      t1, _tx_initialize_unused_memory
    STORE   t0, 0(t1)

    li      t0, MSTATUS_MIE
    csrrc   zero, mstatus, t0
    li      t0, (MSTATUS_MPP_M | MSTATUS_MPIE)
    csrrs   zero, mstatus, t0
    li      t0, (MIE_MTIE | MIE_MEIE)    // No MSIE: software interrupts are never dispatched here
    csrrs   zero, mie, t0

#ifdef __riscv_flen
    li      t0, MSTATUS_FS
    csrrs   zero, mstatus, t0
    fscsr   x0
#endif

    addi    sp, sp, -16
    STORE   ra, 0(sp)
    call    board_init
    LOAD    ra, 0(sp)
    addi    sp, sp, 16

    la      t0, trap_entry
    csrw    mtvec, t0
    ret
