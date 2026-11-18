/***************************************************************************
 * Copyright (c) 2026 Eclipse ThreadX contributors
 *
 * This program and the accompanying materials are made available under the
 * terms of the MIT License which is available at
 * https://opensource.org/licenses/MIT.
 *
 * SPDX-License-Identifier: MIT
 **************************************************************************/

#ifndef RH850_RLIN3_H
#define RH850_RLIN3_H

#include <stdint.h>

void rh850_rlin3_init(uintptr_t base, uint32_t clk_hz, uint32_t baud);
void rh850_rlin3_putc(uintptr_t base, char c);

#endif
