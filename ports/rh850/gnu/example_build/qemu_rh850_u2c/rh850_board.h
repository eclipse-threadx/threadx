/***************************************************************************
 * Copyright (c) 2026 Eclipse ThreadX contributors
 *
 * This program and the accompanying materials are made available under the
 * terms of the MIT License which is available at
 * https://opensource.org/licenses/MIT.
 *
 * SPDX-License-Identifier: MIT
 **************************************************************************/

#ifndef RH850_BOARD_H
#define RH850_BOARD_H

#include <stdint.h>

void board_init(void);
void board_putc(char c);
void board_exit(int code) __attribute__((noreturn));

#define RH850_LARGE_BSS __attribute__((section(".bss.rh850_large")))

#define RH850_FAULT_EXIT_CODE 99

#endif
