/***************************************************************************
 * Copyright (c) 2025 10xEngineers
 *
 * This program and the accompanying materials are made available under the
 * terms of the MIT License which is available at
 * https://opensource.org/licenses/MIT.
 *
 * SPDX-License-Identifier: MIT
 **************************************************************************/

#ifndef RISCV_UART_H
#define RISCV_UART_H

/* SpacemiT K1 UART0 
 * 
 * DTS node: /soc/serial@d4017000
 *    serial@d4017000 {
 *			power-domains = <0x20 0x00>;
 *			reg-io-width = <0x04>;
 *			clk-fpga = <0xe11130>;
 *			pinctrl-names = "default";
 *			interconnect-names = "dma-mem";
 *			pinctrl-0 = <0x23>;
 *			interconnects = <0x22>;
 *			resets = <0x1d 0x01>;
 *			interrupts = <0x2a>;
 *			clocks = <0x03 0x3a 0x03 0xb4>;
 *			interrupt-parent = <0x1e>;
 *			dma-names = "rx", "tx";
 *			cpuidle,pm-runtime,sleep;
 *			compatible = "spacemit,pxa-uart";
 *			status = "okay";
 *			reg = <0x00 0xd4017000 0x00 0x100>;
 *			dmas = <0x21 0x04 0x01 0x21 0x03 0x01>;
 *			reg-shift = <0x02>;
 *		};


 * K1 User Manual Section 16.3.4 defines the register layout.  Because
 * reg-shift = 2, the logical 16550A register indices (0..7) map to
 * byte offsets 0x00, 0x04, 0x08, ..., 0x1C (stride of 4).
 *
 * Register map (at UART0 + offset):
 *
 *   Offset  Name          R/W   Description
 *   ──────  ──────────    ───   ────────────────────────────────────
 *   0x00    RBR/THR/DLL   R/W   Receive Buffer / Transmit Holding /
 *                                Divisor Latch Low (when LCR.DLAB=1)
 *   0x04    IER/DLH       R/W   Interrupt Enable /
 *                                Divisor Latch High (when LCR.DLAB=1)
 *   0x08    IIR/FCR       R/W   Interrupt Identification (R) /
 *                                FIFO Control (W)
 *   0x0C    LCR           R/W   Line Control
 *   0x10    MCR           R/W   Modem Control
 *   0x14    LSR           R     Line Status
 *   0x18    MSR           R     Modem Status
 *   0x1C    SPR           R/W   Scratchpad
 *
 */

#define UART0                   0xD4017000UL
#define UART0_SIZE              0x100UL        /* 256 bytes, from DTS reg */
#define UART0_IRQ               42             /* DTS: interrupts = <0x2a> */
#define UART0_REG_SHIFT         2              /* DTS: reg-shift = <0x02> */

#define puts                    uart_puts

int uart_init(void);
int uart_putc(int ch);
int uart_puts(const char *str);

#endif /* RISCV_UART_H */
