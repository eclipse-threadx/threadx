/***************************************************************************
 * Copyright (c) 2026 10xEngineers
 *
 * This program and the accompanying materials are made available under the
 * terms of the MIT License which is available at
 * https://opensource.org/licenses/MIT.
 *
 * SPDX-License-Identifier: MIT
 **************************************************************************/

#include "uart.h"
#include "csr.h"
#include "plic.h"
#include <stdint.h>

#define Reg(reg) ((volatile unsigned char *)(UART0 + (reg)))

#define RHR 0
#define THR 0
#define IER 1
#define IER_RX_ENABLE (1<<0)
#define IER_TX_ENABLE (1<<1)
#define FCR 2
#define FCR_FIFO_ENABLE (1<<0)
#define FCR_FIFO_CLEAR (3<<1)
#define LCR 3
#define LCR_EIGHT_BITS (3<<0)
#define LCR_BAUD_LATCH (1<<7)
#define LSR 5
#define LSR_RX_READY (1<<0)
#define LSR_TX_IDLE (1<<5)

#define ReadReg(reg) (*(Reg(reg)))
#define WriteReg(reg, v) (*(Reg(reg)) = (v))

int uart_init(void)
{
    WriteReg(IER, 0x00);
    WriteReg(LCR, LCR_BAUD_LATCH);
    WriteReg(0, 0x03);
    WriteReg(1, 0x00);
    WriteReg(LCR, LCR_EIGHT_BITS);
    WriteReg(FCR, FCR_FIFO_ENABLE | FCR_FIFO_CLEAR);

    plic_irq_enable(UART0_IRQ);
    plic_prio_set(UART0_IRQ, 1);

    return 0;
}

void uart_putc_nolock(int ch)
{
    while ((ReadReg(LSR) & LSR_TX_IDLE) == 0)
        ;
    WriteReg(THR, ch);
}

int uart_putc(int ch)
{
    int intr_enable = riscv_mintr_get();
    riscv_mintr_off();
    uart_putc_nolock(ch);
    riscv_mintr_restore(intr_enable);
    return 1;
}

int uart_puts(const char *str)
{
    int i;
    int intr_enable = riscv_mintr_get();
    riscv_mintr_off();
    for (i = 0; str[i] != 0; i++)
    {
        uart_putc_nolock(str[i]);
    }
    uart_putc_nolock('\n');
    riscv_mintr_restore(intr_enable);
    return i;
}
