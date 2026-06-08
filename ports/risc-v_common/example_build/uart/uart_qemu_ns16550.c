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

/* NS16550-compatible UART driver for QEMU virt machine (RV32 and RV64).  */

#include "uart.h"
#include "csr.h"
#include "plic.h"
#include <stdint.h>

/* Memory-mapped UART control registers.  */
#define Reg(reg)            ((volatile unsigned char *)(UART0 + (reg)))

#define RHR 0               /* receive holding register (for input bytes)  */
#define THR 0               /* transmit holding register (for output bytes)  */
#define IER 1               /* interrupt enable register  */
#define IER_RX_ENABLE (1<<0)
#define IER_TX_ENABLE (1<<1)
#define FCR 2               /* FIFO control register  */
#define FCR_FIFO_ENABLE (1<<0)
#define FCR_FIFO_CLEAR (3<<1) /* clear the content of the two FIFOs  */
#define ISR 2               /* interrupt status register  */
#define LCR 3               /* line control register  */
#define LCR_EIGHT_BITS (3<<0)
#define LCR_BAUD_LATCH (1<<7) /* special mode to set baud rate  */
#define LSR 5               /* line status register  */
#define LSR_RX_READY (1<<0) /* input is waiting to be read from RHR  */
#define LSR_TX_IDLE  (1<<5) /* THR can accept another character to send  */

#define ReadReg(reg)        (*(Reg(reg)))
#define WriteReg(reg, v)    (*(Reg(reg)) = (v))

int uart_init(void)
{
    /* Disable interrupts.  */
    WriteReg(IER, 0x00);

    /* Special mode to set baud rate.  */
    WriteReg(LCR, LCR_BAUD_LATCH);

    /* LSB for baud rate of 38.4K.  */
    WriteReg(0, 0x03);

    /* MSB for baud rate of 38.4K.  */
    WriteReg(1, 0x00);

    /* Leave set-baud mode, set word length to 8 bits, no parity.  */
    WriteReg(LCR, LCR_EIGHT_BITS);

    /* Reset and enable FIFOs.  */
    WriteReg(FCR, FCR_FIFO_ENABLE | FCR_FIFO_CLEAR);

    /* Enable UART0 in PLIC.  */
    plic_irq_enable(UART0_IRQ);

    /* Set UART0 priority in PLIC.  */
    plic_prio_set(UART0_IRQ, 1);

    puts("[UART0] : Uart Init Done, this is Test output!");
    return 0;
}

/* uart_putc_nolock is static inline so it can be inlined into uart_putc
   and uart_puts without introducing a public symbol.  */
static inline void uart_putc_nolock(int ch)
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
        uart_putc_nolock(str[i]);
    uart_putc_nolock('\n');
    riscv_mintr_restore(intr_enable);
    return i;
}
