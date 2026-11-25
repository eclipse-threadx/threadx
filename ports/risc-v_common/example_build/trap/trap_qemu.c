/***************************************************************************
 * Copyright (c) 2024 Microsoft Corporation
 * Copyright (c) 2026-present Eclipse ThreadX contributors
 *
 * This program and the accompanying materials are made available under the
 * terms of the MIT License which is available at
 * https://opensource.org/licenses/MIT.
 *
 * SPDX-License-Identifier: MIT
 **************************************************************************/

/* Trap handler for QEMU virt machine (RV32 and RV64).
 *
 * mcause constants use __riscv_xlen to resolve correctly for both ISAs:
 *   RV32: interrupt bit = bit 31 (0x80000000)
 *   RV64: interrupt bit = bit 63 (0x8000000000000000)
 *
 */

#include "csr.h"
#include <stdint.h>
#include "uart.h"
#include "hwtimer.h"
#include "plic.h"
#include <tx_port.h>
#include <tx_api.h>
#include <tx_thread.h>

#define MCAUSE_INT_BIT          ((uintptr_t)1 << (__riscv_xlen - 1))

#define OS_IS_INTERRUPT(mcause) ((mcause) & MCAUSE_INT_BIT)
#define OS_IS_TICK_INT(mcause)  ((mcause) == (MCAUSE_INT_BIT | 7u))
#define OS_IS_SOFT_INT(mcause)  ((mcause) == (MCAUSE_INT_BIT | 3u))
#define OS_IS_EXT_INT(mcause)   ((mcause) == (MCAUSE_INT_BIT | 11u))

/* Synchronous exception codes (interrupt bit clear), privileged spec table
   "Machine cause register (mcause) values after trap".  */
#define CAUSE_BREAKPOINT        3u
#define CAUSE_ECALL_U           8u
#define CAUSE_ECALL_S           9u
#define CAUSE_ECALL_M           11u

/* Saved mepc lives in slot 30 of the interrupt frame that the trap entry
   and _tx_thread_context_save build.  One slot is XLEN/8 bytes on both
   ports, so a uintptr_t index works for RV32 and RV64.  */
#define TX_RISCV_FRAME_MEPC_SLOT 30

extern void _tx_timer_interrupt(void);

static void print_hex(uintptr_t val)
{
    const char digits[] = "0123456789ABCDEF";
    uart_putc('0');
    uart_putc('x');
    for (int i = (int)(sizeof(uintptr_t) * 2) - 1; i >= 0; i--)
    {
        int d = (int)((val >> (i * 4)) & 0xFu);
        uart_putc(digits[d]);
    }
    uart_putc('\n');
}

static int trap_skip_instruction(uintptr_t mepc)
{
    TX_THREAD  *thread_ptr = _tx_thread_current_ptr;
    uintptr_t  *frame;
    uintptr_t   length;

    if ((TX_THREAD_GET_SYSTEM_STATE() != 1u) || (thread_ptr == TX_NULL))
        return -1;

    /* A 32-bit instruction has its two low bits set; a compressed one does
       not.  ecall and ebreak are 32-bit, c.ebreak is 16-bit.  */
    length = ((*(volatile uint16_t *)mepc & 3u) == 3u) ? 4u : 2u;

    frame = (uintptr_t *)thread_ptr->tx_thread_stack_ptr;
    frame[TX_RISCV_FRAME_MEPC_SLOT] = mepc + length;
    return 0;
}

void trap_handler(uintptr_t mcause, uintptr_t mepc, uintptr_t mtval)
{
    if (OS_IS_INTERRUPT(mcause))
    {
        if (OS_IS_TICK_INT(mcause))
        {
            hwtimer_handler();
            _tx_timer_interrupt();
        }
        else if (OS_IS_EXT_INT(mcause))
        {
            int ret = plic_irq_intr();
            if (ret)
            {
                uart_puts("[INTERRUPT]: handler irq error!");
                while (1) ;
            }
        }
        else
        {
            uart_puts("[INTERRUPT]: unhandled interrupt, halting");
            uart_puts("mcause:");
            print_hex(mcause);
            while (1) ;
        }
    }
    else
    {
        uart_puts("[EXCEPTION]");
        uart_puts("mcause:");
        print_hex(mcause);
        uart_puts("mepc:");
        print_hex(mepc);
        uart_puts("mtval:");
        print_hex(mtval);

        if ((mcause == CAUSE_BREAKPOINT) || (mcause == CAUSE_ECALL_U) ||
            (mcause == CAUSE_ECALL_S) || (mcause == CAUSE_ECALL_M))
        {
            if (trap_skip_instruction(mepc) == 0)
            {
                uart_puts("[EXCEPTION]: ecall/ebreak skipped, resuming");
                return;
            }
            uart_puts("[EXCEPTION]: ecall/ebreak outside a thread, halting");
        }
        else
        {
            uart_puts("[EXCEPTION]: unhandled exception, halting");
        }
        while (1) ;
    }
}
