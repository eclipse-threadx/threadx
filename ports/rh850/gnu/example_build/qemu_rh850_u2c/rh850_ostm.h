/***************************************************************************
 * Copyright (c) 2026 Eclipse ThreadX contributors
 *
 * This program and the accompanying materials are made available under the
 * terms of the MIT License which is available at
 * https://opensource.org/licenses/MIT.
 *
 * SPDX-License-Identifier: MIT
 **************************************************************************/

#ifndef RH850_OSTM_H
#define RH850_OSTM_H

#include <stdbool.h>
#include <stdint.h>

void rh850_ostm_start_interval(uintptr_t base, uint32_t cmp, bool irq);
void rh850_ostm_start_freerun(uintptr_t base, bool irq);
void rh850_ostm_set_compare(uintptr_t base, uint32_t cmp);
void rh850_ostm_stop(uintptr_t base);
uint32_t rh850_ostm_count(uintptr_t base);

#endif
