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
#include "rh850_rlin3.h"
#include "rh850_rlin3_regs.h"

/* baud = clk / (LBRP01 + 1) / 16 with LWBR = 0; LBFC = 0 gives 8N1 */
void rh850_rlin3_init(uintptr_t base, uint32_t clk_hz, uint32_t baud)
{
	uint32_t div = (clk_hz + (RH850_RLIN3_SAMPLES * baud) / 2U) /
		       (RH850_RLIN3_SAMPLES * baud);

	if (div == 0U) {
		div = 1U;
	}

	rh850_write8(base + RH850_RLIN3_LCUC, 0);
	while ((rh850_read8(base + RH850_RLIN3_LMST) & RH850_RLIN3_LMST_OMM0) != 0U) {
	}

	rh850_write8(base + RH850_RLIN3_LWBR, 0);
	rh850_write16(base + RH850_RLIN3_LBRP01, (uint16_t)(div - 1U));
	rh850_write8(base + RH850_RLIN3_LMD, RH850_RLIN3_LMD_UART);
	rh850_write8(base + RH850_RLIN3_LBFC, 0);
	rh850_write8(base + RH850_RLIN3_LEDE, 0);
	rh850_write8(base + RH850_RLIN3_LUOR1, 0);

	rh850_write8(base + RH850_RLIN3_LCUC, RH850_RLIN3_LCUC_OM0);
	while ((rh850_read8(base + RH850_RLIN3_LMST) & RH850_RLIN3_LMST_OMM0) == 0U) {
	}
	rh850_write8(base + RH850_RLIN3_LUOER,
		     RH850_RLIN3_LUOER_UTOE | RH850_RLIN3_LUOER_UROE);
}

void rh850_rlin3_putc(uintptr_t base, char c)
{
	rh850_write8(base + RH850_RLIN3_LUTDR, (uint8_t)c);
	while ((rh850_read8(base + RH850_RLIN3_LST) & RH850_RLIN3_LST_UTS) != 0U) {
	}
}
