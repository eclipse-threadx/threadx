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

#ifndef RISCV_UART_H
#define RISCV_UART_H

#define UART0 		0x10000000L
#define UART0_IRQ 	10

#define puts		uart_puts
int 	uart_init(void);
int 	uart_putc(int ch);
void 	uart_putc_nolock(int ch);
int 	uart_puts(const char* str);
#endif
