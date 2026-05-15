/***************************************************************************
 * Copyright (c) 2026 10xEngineers
 *
 * This program and the accompanying materials are made available under the
 * terms of the MIT License which is available at
 * https://opensource.org/licenses/MIT.
 *
 * SPDX-License-Identifier: MIT
 **************************************************************************/

/* Trap handler for RISC-V QEMU virt regression tests.
   Calls test_interrupt_dispatch() BEFORE _tx_timer_interrupt() to match
   the Linux port's behavior (generate_test_file.sh injection order). */

#include "csr.h"
#include <stdint.h>
#include <stdio.h>
#include "uart.h"
#include "hwtimer.h"
#include "plic.h"
#include <tx_port.h>
#include <tx_api.h>
#include <tx_trace.h>

/* Interrupt bit is the MSB of mcause */
#define MCAUSE_INT_BIT  ((uintptr_t)1 << (sizeof(uintptr_t) * 8 - 1))

#define OS_IS_INTERUPT(mcause)  ((mcause) & MCAUSE_INT_BIT)
#define OS_IS_TICK_INT(mcause)  ((mcause) == (MCAUSE_INT_BIT | 7u))
#define OS_IS_SOFT_INT(mcause)  ((mcause) == (MCAUSE_INT_BIT | 3u))
#define OS_IS_EXT_INT(mcause)   ((mcause) == (MCAUSE_INT_BIT | 11u))

extern void _tx_timer_interrupt(void);
extern void test_interrupt_dispatch(void) __attribute__((weak));
extern void _exit(int code) __attribute__((noreturn));

void trap_handler(uintptr_t mcause, uintptr_t mepc, uintptr_t mtval)
{
    (void)mepc;
    (void)mtval;

    if (OS_IS_INTERUPT(mcause))
    {
        if (OS_IS_TICK_INT(mcause))
        {
            hwtimer_handler();

            /* Trace ISR enter event (matches Linux port behavior). */
            _tx_trace_isr_enter_insert(0);

            /* Call test ISR dispatch BEFORE timer interrupt (matches Linux). */
            if (test_interrupt_dispatch)
                test_interrupt_dispatch();

            _tx_timer_interrupt();

            /* Trace ISR exit event. */
            _tx_trace_isr_exit_insert(0);
        }
        else if (OS_IS_EXT_INT(mcause))
        {
            int ret = plic_irq_intr();
            if (ret)
            {
                uart_puts("[INTERRUPT]: handler irq error!");
                while (1)
                    ;
            }
        }
        else
        {
            uart_puts("[INTERRUPT]: unhandled interrupt!");
            while (1)
                ;
        }
    }
    else
    {
        uart_puts("[EXCEPTION]: unhandled exception!");
        printf("  mcause=0x%lx mepc=0x%lx mtval=0x%lx\n",
               (unsigned long)mcause, (unsigned long)mepc, (unsigned long)mtval);
        _exit(1);
    }
}
