/***************************************************************************
 * Copyright (c) 2026 Eclipse ThreadX contributors
 *
 * This program and the accompanying materials are made available under the
 * terms of the MIT License which is available at
 * https://opensource.org/licenses/MIT.
 *
 * SPDX-License-Identifier: MIT
 **************************************************************************/

#ifndef BOARD_QEMU_RH850_U2C_H
#define BOARD_QEMU_RH850_U2C_H

#include "soc.h"

#define BOARD_CONSOLE_BASE   SOC_RLIN311_BASE
#define BOARD_CONSOLE_CLK_HZ SOC_CLK_HSB_HZ
#define BOARD_CONSOLE_BAUD   115200

#define BOARD_TICK_OSTM_BASE   SOC_OSTM0_BASE
#define BOARD_TICK_IRQ         SOC_IRQ_OSTM0
#define BOARD_TIMER2_OSTM_BASE SOC_OSTM1_BASE
#define BOARD_TIMER2_IRQ       SOC_IRQ_OSTM1
#define BOARD_TIMER_CLK_HZ     SOC_CLK_HSB_HZ

/* Edge channels with no device in this QEMU machine, raised by software */
#define BOARD_SWIRQ0 SOC_IRQ_TAUJ0_I0
#define BOARD_SWIRQ1 SOC_IRQ_TAUJ0_I1

#define BOARD_TEST_EXIT_BASE 0x40000000

#endif
