/***************************************************************************
 * Copyright (c) 2026 Eclipse ThreadX contributors
 *
 * This program and the accompanying materials are made available under the
 * terms of the MIT License which is available at
 * https://opensource.org/licenses/MIT.
 *
 * SPDX-License-Identifier: MIT
 **************************************************************************/

#ifndef RH850_INTC_REGS_H
#define RH850_INTC_REGS_H

#define RH850_EIC_STRIDE   2
#define RH850_EIC_EIP_MASK 0x0f
#define RH850_EIC_EIOV     (1 << 5)
#define RH850_EIC_EITB     (1 << 6)
#define RH850_EIC_EIMK     (1 << 7)
/* EIRF is bit 12, bit 4 of the upper byte */
#define RH850_EIC_EIRF     (1 << 12)

#define RH850_EIIC_EIINT   0x1000

#endif
