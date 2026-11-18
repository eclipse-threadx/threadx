/***************************************************************************
 * Copyright (c) 2026 Eclipse ThreadX contributors
 *
 * This program and the accompanying materials are made available under the
 * terms of the MIT License which is available at
 * https://opensource.org/licenses/MIT.
 *
 * SPDX-License-Identifier: MIT
 **************************************************************************/

#ifndef RH850_IO_H
#define RH850_IO_H

#include <stdint.h>

static inline uint8_t rh850_read8(uintptr_t addr)
{
	return *(volatile uint8_t *)addr;
}

static inline uint16_t rh850_read16(uintptr_t addr)
{
	return *(volatile uint16_t *)addr;
}

static inline uint32_t rh850_read32(uintptr_t addr)
{
	return *(volatile uint32_t *)addr;
}

static inline void rh850_write8(uintptr_t addr, uint8_t v)
{
	*(volatile uint8_t *)addr = v;
}

static inline void rh850_write16(uintptr_t addr, uint16_t v)
{
	*(volatile uint16_t *)addr = v;
}

static inline void rh850_write32(uintptr_t addr, uint32_t v)
{
	*(volatile uint32_t *)addr = v;
}

#endif
