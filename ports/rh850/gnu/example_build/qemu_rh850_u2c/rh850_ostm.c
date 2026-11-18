/***************************************************************************
 * Copyright (c) 2026 Eclipse ThreadX contributors
 *
 * This program and the accompanying materials are made available under the
 * terms of the MIT License which is available at
 * https://opensource.org/licenses/MIT.
 *
 * SPDX-License-Identifier: MIT
 **************************************************************************/

#include "rh850_io.h"
#include "rh850_ostm.h"
#include "rh850_ostm_regs.h"

void rh850_ostm_stop(uintptr_t base)
{
	rh850_write8(base + RH850_OSTM_TT, 1);
	while ((rh850_read8(base + RH850_OSTM_TE) & 1U) != 0U) {
	}
}

/* CTL can only be written while the counter is stopped */
void rh850_ostm_start_interval(uintptr_t base, uint32_t cmp, bool irq)
{
	rh850_ostm_stop(base);
	rh850_write8(base + RH850_OSTM_CTL, irq ? RH850_OSTM_CTL_IE : 0U);
	rh850_write32(base + RH850_OSTM_CMP, cmp);
	rh850_write8(base + RH850_OSTM_TS, 1);
}

uint32_t rh850_ostm_count(uintptr_t base)
{
	return rh850_read32(base + RH850_OSTM_CNT);
}

void rh850_ostm_start_freerun(uintptr_t base, bool irq)
{
	rh850_ostm_stop(base);
	rh850_write8(base + RH850_OSTM_CTL,
		     RH850_OSTM_CTL_MD1 | (irq ? RH850_OSTM_CTL_IE : 0U));
	rh850_write32(base + RH850_OSTM_CMP, 0xffffffffU);
	rh850_write8(base + RH850_OSTM_TS, 1);
}

/* Read back so the new CMP is in place before the caller checks CNT */
void rh850_ostm_set_compare(uintptr_t base, uint32_t cmp)
{
	rh850_write32(base + RH850_OSTM_CMP, cmp);
	(void)rh850_read32(base + RH850_OSTM_CMP);
}
