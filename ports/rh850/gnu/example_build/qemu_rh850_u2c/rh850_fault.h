/***************************************************************************
 * Copyright (c) 2026 Eclipse ThreadX contributors
 *
 * This program and the accompanying materials are made available under the
 * terms of the MIT License which is available at
 * https://opensource.org/licenses/MIT.
 *
 * SPDX-License-Identifier: MIT
 **************************************************************************/

#ifndef RH850_FAULT_H
#define RH850_FAULT_H

#include <stdint.h>

void rh850_fault(uint32_t fe, uint32_t cause, uint32_t pc) __attribute__((noreturn));

#endif
