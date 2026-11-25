/***************************************************************************
 * Copyright (c) 2025 10xEngineers
 *
 * This program and the accompanying materials are made available under the
 * terms of the MIT License which is available at
 * https://opensource.org/licenses/MIT.
 *
 * SPDX-License-Identifier: MIT
 **************************************************************************/

/*
 * S-mode trap handler 
 */

#include "csr.h"
#include "uart.h"
#include "hwtimer.h"
#include "plic.h"
#include <stdint.h>
#include <tx_port.h>
#include <tx_api.h>

/* scause bit 63: interrupt flag. */
#define SCAUSE_INTERRUPT        0x8000000000000000ull

#define SCAUSE_S_TIMER_INT      (SCAUSE_INTERRUPT | 5u)
#define SCAUSE_S_EXTERNAL_INT   (SCAUSE_INTERRUPT | 9u)

extern void _tx_timer_interrupt(void);

void trap_handler(uintptr_t cause, uintptr_t epc, uintptr_t tval)
{
    (void)epc;
    (void)tval;

    if (cause & SCAUSE_INTERRUPT)
    {
        if (cause == SCAUSE_S_TIMER_INT)
        {
            hwtimer_handler();
            _tx_timer_interrupt();
        }
        else if (cause == SCAUSE_S_EXTERNAL_INT)
        {
            if (plic_irq_intr() != 0)
            {
                puts("[trap] PLIC dispatch failed");
                while (1)
                    ;
            }
        }
        else
        {
            puts("[trap] unhandled S-mode interrupt");
            while (1)
                ;
        }
    }
    else
    {
        puts("[trap] unhandled synchronous exception");
        while (1)
            ;
    }
}
