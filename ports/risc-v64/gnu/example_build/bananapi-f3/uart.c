/***************************************************************************
 * Copyright (c) 2025 10xEngineers
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

/*
 * The K1 UART Register layout 
 *
 *   Offset  Name          Access        Description
 *   ──────  ──────        ──────        ──────────────────────────────
 *   0x00    RBR/THR       R/W           Receive Buffer / Transmit Hold
 *   0x00    DLL           R/W (DLAB=1)  Divisor Latch Low
 *   0x04    IER           R/W           Interrupt Enable
 *   0x04    DLH           R/W (DLAB=1)  Divisor Latch High
 *   0x08    IIR           R             Interrupt Identification
 *   0x08    FCR           W             FIFO Control
 *   0x0C    LCR           R/W           Line Control
 *   0x10    MCR           R/W           Modem Control
 *   0x14    LSR           R             Line Status
 *   0x18    MSR           R             Modem Status
 *   0x1C    SPR           R/W           Scratchpad
 */

#define REG(off)                  (*(volatile uint32_t *)(UART0 + (off)))

#define RHR                       0x00
#define THR                       0x00
#define DLL                       0x00
#define IER                       0x04
#define DLH                       0x04
#define IIR                       0x08
#define FCR                       0x08
#define LCR                       0x0C
#define MCR                       0x10
#define LSR                       0x14


#define IER_RAVIE                 (1u << 0)   /* Receiver data available IRQ enable */
#define IER_TIE                   (1u << 1)   /* Transmit data request IRQ enable   */
#define IER_RLSE                  (1u << 2)   /* Receiver line status IRQ enable    */
#define IER_UUE                   (1u << 6)   /* UART unit enable                   */

#define FCR_TRFIFOE               (1u << 0)   /* Transmit/Receive FIFO enable       */
#define FCR_RESETRF               (1u << 1)   /* Reset receive FIFO                 */
#define FCR_RESETTF               (1u << 2)   /* Reset transmit FIFO                */
#define FCR_FIFO_CLEAR            (FCR_RESETRF | FCR_RESETTF)

#define LCR_WLS_8                 (3u << 0)   /* 8-bit word                         */
#define LCR_DLAB                  (1u << 7)   /* Divisor latch access bit           */

#define LSR_DR                    (1u << 0)   /* Data ready in receive FIFO         */
#define LSR_TDRQ                  (1u << 5)   /* Transmit holding/FIFO ready        */
#define LSR_TEMT                  (1u << 6)   /* Transmitter empty                  */

#define ReadReg(off)              (REG(off) & 0xFF)
#define WriteReg(off, v)          (REG(off) = (uint32_t)((v) & 0xFF))

int uart_init(void)
{
    /*
     * If a prior boot stage (BootROM / OpenSBI / U-Boot) already
     * configured the UART, keep its baud-rate divisor so we keep
     * a working console.  Just make sure interrupts are masked,
     * the FIFOs are clean and the unit-enable bit is set.
     */
    WriteReg(IER, 0x00);

    /*
     * Program the baud-rate divisor for 115200 baud.
     * The PXA UART functional clock on K1 is 14.7456 MHz:
     *   14,745,600 / (16 × 8) = 115,200 baud → DLL = 8, DLH = 0.
     */
    WriteReg(LCR, LCR_DLAB);
    WriteReg(DLL, 0x08);
    WriteReg(DLH, 0x00);

    WriteReg(LCR, LCR_WLS_8);
    WriteReg(FCR, FCR_TRFIFOE | FCR_FIFO_CLEAR);

    /* Enable the UART transmit/receive engines (K1-specific). */
    WriteReg(IER, IER_UUE);

    plic_irq_enable(UART0_IRQ);
    plic_prio_set(UART0_IRQ, 1);

    puts("[uart] UART0 initialized");
    return 0;
}

static inline void uart_putc_nolock(int ch)
{
    while ((ReadReg(LSR) & LSR_TDRQ) == 0)
        ;
    WriteReg(THR, ch);
}

int uart_putc(int ch)
{
    int intr_enable = riscv_intr_get();
    riscv_intr_off();
    uart_putc_nolock(ch);
    riscv_intr_restore(intr_enable);
    return 1;
}

int uart_puts(const char *str)
{
    int i;
    int intr_enable = riscv_intr_get();
    riscv_intr_off();
    for (i = 0; str[i] != 0; i++)
        uart_putc_nolock(str[i]);
    uart_putc_nolock('\r');
    uart_putc_nolock('\n');
    riscv_intr_restore(intr_enable);
    return i;
}
