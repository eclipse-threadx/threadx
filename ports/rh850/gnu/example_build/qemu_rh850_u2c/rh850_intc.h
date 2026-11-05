/***************************************************************************
 * Copyright (c) 2026 Eclipse ThreadX contributors
 *
 * This program and the accompanying materials are made available under the
 * terms of the MIT License which is available at
 * https://opensource.org/licenses/MIT.
 *
 * SPDX-License-Identifier: MIT
 **************************************************************************/

#ifndef RH850_INTC_H
#define RH850_INTC_H

#include <stdint.h>

typedef void (*rh850_isr_t)(void *arg);

#define RH850_INTC_PRIO_LOWEST 15

void rh850_intc_connect(unsigned int ch, rh850_isr_t isr, void *arg);
void rh850_intc_set_priority(unsigned int ch, unsigned int prio);
void rh850_intc_enable(unsigned int ch);
void rh850_intc_disable(unsigned int ch);
void rh850_intc_raise(unsigned int ch);
void rh850_intc_clear(unsigned int ch);

void rh850_intc_dispatch(uint32_t eiic);

#endif
